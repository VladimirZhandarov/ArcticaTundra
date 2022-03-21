#include "imagecutdownthread.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>

#include <QtXml/QDomDocument>

#include "imagecutupsettings.h"
#include "contouritemmodel.h"

#include <iceodb/odb/lineview.hxx>

namespace ICEIMAGE_NS
{

ImageCutDownThread::ImageCutDownThread(QSharedPointer<GdalImageBuilder> imageBuilder,
	const QFileInfo& fileInfo, QObject* parent) : QThread(parent),
	m_imageBuilder(imageBuilder), m_fileInfo(fileInfo), m_errorFlag(false),
	m_stopFlag(false), m_unit(NONE), m_divK(1.0)
{

}

ImageCutDownThread::~ImageCutDownThread()
{

}

bool ImageCutDownThread::isNoError() const
{
	return !m_errorFlag;
}

UNIT ImageCutDownThread::getUnit() const
{
	return m_unit;
}

double ImageCutDownThread::getDivK() const
{
	return m_divK;
}

void ImageCutDownThread::stop()
{
	m_stopFlag = true;
}

void ImageCutDownThread::run()
{
	if(m_imageBuilder.isNull() || !m_fileInfo.exists())
	{
		m_errorFlag = true;
		return;
	}
	if(!m_imageBuilder->createCutDownImage(m_fileInfo.absoluteFilePath()))
	{
		m_errorFlag = true;
		return;
	}
	m_errorFlag = false;
	ImageCutUpSettings imageCutUpSettings(m_fileInfo, this);
	QList<TileImage> imageTileList = imageCutUpSettings.getTileImageList();
	if(imageTileList.empty())
	{
		m_errorFlag = true;
		return;
	}
	QSize imageSize = imageCutUpSettings.getImageSize();
	emit maximumCalculated(imageTileList.size());

	m_unit = NONE;
	m_divK = 1.0;

	int stepCount = 0;
	QList<ICEIMAGE_NS::ContourItemModel*> contourItemModelList;
	bool firstRead = true;
	for(int i=0; i<imageTileList.size(); i++)
	{
		int x1 = imageTileList[i].x;
		int y1 = imageTileList[i].y;
		QFileInfo tileFileInfo(m_fileInfo.dir(), imageTileList[i].filename);
		int index = 0;
		bool csvFileExist = false;
		do 
		{
			QFileInfo csvFileInfo(tileFileInfo.dir(), tileFileInfo.completeBaseName() +
				"_" + QString::number(index + 1) + ".csv"); //TODO refac
			csvFileExist = csvFileInfo.exists();
			ICEIMAGE_NS::ContourItemModel* contourItemModel = 0;
			if(csvFileExist) //TODO refac
			{
				if(index < contourItemModelList.size())
				{
					contourItemModel = contourItemModelList[index];
				}
				else
				{
					contourItemModel = m_imageBuilder->createEmptyContourItemModel(
						imageSize.width(), imageSize.height());
					contourItemModelList.append(contourItemModel);
				}
				loadFromCsvFile(csvFileInfo, contourItemModel); //TODO refac
				index++;
			}
		}
		while(csvFileExist);

		QFileInfo htmlFileInfo(tileFileInfo.dir(), tileFileInfo.completeBaseName() + ".html"); //TODO refac
		loadFromHtmlFile(htmlFileInfo, contourItemModelList, imageSize, firstRead);
		firstRead = false;
		cv::Mat matImage = cv::imread(tileFileInfo.absoluteFilePath().toStdString().c_str());
		if(!matImage.empty())
		{
			m_imageBuilder->addCutDownImage(matImage, QRect(x1, y1, matImage.cols,
				matImage.rows));
		}
		else
		{
			m_errorFlag = true;
		}
		stepCount++;
		emit valueChanged(stepCount);
	}
	m_imageBuilder->flushCutDownImage();
}

void ImageCutDownThread::loadFromCsvFile(const QFileInfo& csvFileInfo,
	ICEIMAGE_NS::ContourItemModel* contourItemModel)
{
	if(!csvFileInfo.exists() || !contourItemModel)
	{
		return;
	}
	QFile file(csvFileInfo.absoluteFilePath());
	if(!file.open(QFile::ReadOnly | QFile::Truncate))
	{
		return;
	}
	QTextStream stream(&file);
	stream.setCodec(QTextCodec::codecForName("Windows-1251"));
	double divK = 1.0;
	UNIT unit = NONE;
	while(!stream.atEnd())
	{
		QStringList textList = stream.readLine().split(";");
		if(textList.size() < 5)//TODO refac 4
		{
			if(textList.size() >= 2 && textList[0].startsWith(tr("Масштаб"))) //TODO magic
			{
				int unitNameIndex1 = textList[0].indexOf(", ");
				int unitNameIndex2 = textList[0].indexOf("/");
				if(unitNameIndex1 != -1 && unitNameIndex2 != -1)
				{
					QString unitName = textList[0].mid(unitNameIndex1 + 2,
						unitNameIndex2 - unitNameIndex1 - 2);
					if(unitName == tr("км"))
					{
						unit = KM;
					}
					else if(unitName == tr("м"))
					{
						unit = M;
					}
					else if(unitName == tr("см"))
					{
						unit = SM;
					}
					else if(unitName == tr("мм"))
					{
						unit = MM;
					}
				}
				bool doubleOk = false;
				divK = textList[1].toDouble(&doubleOk);
				if(!doubleOk)
				{
					divK = 1.0;
				}
				contourItemModel->setTransformation(1.0 / divK, unit);
				m_unit = unit;
				m_divK = divK;
			}
			continue;
		}
		bool intOk = false;
		textList[0].toInt(&intOk);//TODO refac magic numbers
		if(!intOk)
		{
			continue;
		}
		double objectArea = textList[1].toDouble();
		double arcLength = textList[2].toDouble();
		double contourArea = textList[3].toDouble();
		QString comment = textList[4];
		contourItemModel->addContourItemInfo(arcLength / divK , contourArea / (divK * divK),
			objectArea / (divK * divK), comment);
	}
	file.close();
}

void ImageCutDownThread::loadFromHtmlFile(const QFileInfo& htmlFileInfo,
	QList<ICEIMAGE_NS::ContourItemModel*>& contourItemModelList,
	const QSize& imageSize, bool firstRead)
{
	if(m_imageBuilder.isNull() || !htmlFileInfo.exists())
	{
		return;
	}
	QDomDocument htmlDocument;
	QFile file(htmlFileInfo.absoluteFilePath());
	if(!file.open(QFile::ReadOnly | QFile::Truncate))
	{
		return;
	}
	if(!htmlDocument.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();
	QDomElement htmlElement = htmlDocument.firstChildElement("html");
	QDomElement headElement = htmlElement.firstChildElement("head");
	QDomElement styleElement = headElement.firstChildElement("style");

	QString styleString = styleElement.firstChild().toText().data().trimmed();
	QStringList list = styleString.split(QRegExp("[{}]"), QString::SkipEmptyParts);
	QMap<QString, QColor> styleColorMap;
	for(int i=0; i<list.size() - 1; i=i+2)
	{
		QString keyString = list[i].trimmed();
		if(!keyString.startsWith("."))
		{
			continue;
		}
		QStringList valueList = list[i + 1].trimmed().split(QRegExp("[:;]"), QString::SkipEmptyParts);
		QStringList trimmedValueList;
		for(int j=0; j<valueList.size(); j++)
		{
			trimmedValueList.append(valueList[j].trimmed());
		}
		QColor color;
		int backgroundIndex = trimmedValueList.indexOf("background");
		if(backgroundIndex != -1 && backgroundIndex + 1 < valueList.size())
		{
			QString colorString = trimmedValueList[backgroundIndex + 1];
			if(colorString.startsWith("#") && colorString.length() == 7)
			{
				colorString = colorString.right(colorString.length() - 1);
				QString redString = colorString.left(2);
				color.setRed(redString.toInt(0, 16));
				QString greenString = colorString.mid(2, 2);
				color.setGreen(greenString.toInt(0, 16));
				QString blueString = colorString.right(2);
				color.setBlue(blueString.toInt(0, 16));
			}
		}
		int opacityIndex = trimmedValueList.indexOf("opacity");
		if(opacityIndex != -1 && opacityIndex + 1 < valueList.size())
		{
			QString opacityString = trimmedValueList[opacityIndex + 1];
			color.setAlphaF(opacityString.toDouble());
		}
		if(keyString.startsWith("."))
		{
			keyString = keyString.right(keyString.length() - 1);
		}
		styleColorMap.insert(keyString, color);
	}

	QDomElement bodyElement = htmlElement.firstChildElement("body");

	QDomElement preElement = bodyElement.firstChildElement("pre");
	QMap<QString, double> skippedPointCountAndDivKMap;
	for(QDomElement inputElement = preElement.firstChildElement("input");
		!inputElement.isNull(); inputElement = inputElement.nextSiblingElement("input"))
	{
		QString key = inputElement.attribute("name");
		double value = inputElement.attribute("value").toDouble();
		skippedPointCountAndDivKMap.insert(key, value);
	}

	QDomElement tableElement = bodyElement.firstChildElement("table");

	for(QDomElement tableTrElement = tableElement.firstChildElement("tr");
		!tableTrElement.isNull(); tableTrElement = tableTrElement.nextSiblingElement("tr"))
	{
		QDomElement tableCountElement = tableTrElement.firstChildElement("td");
		if(tableCountElement.isNull())
		{
			continue;
		}
		bool rowNumberOk = false;
		int rowNumber = tableCountElement.firstChild().toText().data().toInt(&rowNumberOk);
		if(rowNumberOk && rowNumber > 0)
		{
			QDomElement maskColorElement = tableCountElement.nextSiblingElement("td");
			if(maskColorElement.isNull())
			{
				continue;
			}
			QString maskColorName = maskColorElement.attribute("class");
			QString maskColorNotUsed = maskColorElement.firstChild().toText().data();

			QDomElement maskNameElement = maskColorElement.nextSiblingElement("td");
			if(maskNameElement.isNull())
			{
				continue;
			}
			QString maskName = maskNameElement.firstChild().toText().data();

			QDomElement objectColorElement = maskNameElement.nextSiblingElement("td");
			if(objectColorElement.isNull())
			{
				continue;
			}
			QString objectColorName = objectColorElement.attribute("class");
			QString objectColorNotUsed = objectColorElement.firstChild().toText().data();

			QDomElement lineColorElement = objectColorElement.nextSiblingElement("td");
			if(lineColorElement.isNull())
			{
				continue;
			}
			QString lineColorName = lineColorElement.attribute("class");
			QString lineColorNotUsed = lineColorElement.firstChild().toText().data();

			QDomElement objectNameElement = lineColorElement.nextSiblingElement("td");
			if(objectNameElement.isNull())
			{
				continue;
			}
			QString objectName = objectNameElement.firstChild().toText().data();

			QDomElement objectCountElement = objectNameElement.nextSiblingElement("td");
			if(objectCountElement.isNull())
			{
				continue;
			}
			QString objectCountString = objectCountElement.firstChild().toText().data();
			int objectCount = 0;
			if(objectCountString != "-")
			{
				objectCount = objectCountString.toInt();
			}

			QDomElement objectSumAreaElement = objectCountElement.nextSiblingElement("td");
			if(objectSumAreaElement.isNull())
			{
				continue;
			}
			QString objectSumAreaString = objectSumAreaElement.firstChild().toText().data();
			double objectSumArea = 0.0;
			QString objectSumAreaUnit = tr("писк."); //TODO refac
			if(objectSumAreaString != "-")
			{
				QStringList objectSumAreaStringList = objectSumAreaString.split(" ");
				if(objectSumAreaStringList.count() > 0)
				{
					objectSumArea = objectSumAreaStringList[0].toDouble();
				}
				if(objectSumAreaStringList.count() > 1)
				{
					objectSumAreaUnit = objectSumAreaStringList[1];
				}
			}

			QDomElement objectSumAreaPercentElement = objectSumAreaElement.nextSiblingElement("td");
			if(objectSumAreaPercentElement.isNull())
			{
				continue;
			}
			QString objectSumAreaPercentString = objectSumAreaPercentElement.firstChild().toText().data();
			double objectSumAreaPercent = 0.0;
			if(objectSumAreaPercentString != "-")
			{
				objectSumAreaPercent = objectSumAreaPercentString.toDouble();
			}

			QDomElement objectSumArcLengthElement = objectSumAreaPercentElement.nextSiblingElement("td");
			if(objectSumArcLengthElement.isNull())
			{
				continue;
			}
			QString objectSumArcLengthString = objectSumArcLengthElement.firstChild().toText().data();
			double objectSumArcLength = 0.0;
			QString objectSumArcLengthUnit = tr("писк."); //TODO refac
			if(objectSumArcLengthString != "-")
			{
				QStringList objectSumArcLengthStringList = objectSumArcLengthString.split(" ");
				if(objectSumArcLengthStringList.count() > 0)
				{
					objectSumArcLength = objectSumArcLengthStringList[0].toDouble();
				}
				if(objectSumArcLengthStringList.count() > 1)
				{
					objectSumArcLengthUnit = objectSumArcLengthStringList[1];
				}
			}

			QDomElement filterParamNameElement = objectSumArcLengthElement.nextSiblingElement("td");
			if(filterParamNameElement.isNull())
			{
				continue;
			}
			QStringList filterParamNameList = filterParamNameElement.firstChild().toText().data().split("<br/>");

			//TODO refac
			std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView;
			QColor maskColor = styleColorMap.value(maskColorName);
			QStringList maskFilterParamNameList;
			if(filterParamNameList.size() > 0)
			{
				maskFilterParamNameList.append(filterParamNameList[0]);
			}
			if(maskColorNotUsed.isEmpty())
			{
				maskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(new ICEODB_NS::MaskView(
					maskName, maskColor, false, 255, maskFilterParamNameList));
			}
			else
			{
				maskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(new ICEODB_NS::MaskView(
					maskName, QColor(), true, maskColor.alpha(), maskFilterParamNameList));
			}

			ICEIMAGE_NS::ContourItemModel* contourItemModel = 0;
			if(objectSumAreaPercentString != "-")
			{
				if((rowNumber - 1) < contourItemModelList.size())
				{
					contourItemModel = contourItemModelList[rowNumber - 1];
				}
				else
				{
					contourItemModel = m_imageBuilder->createEmptyContourItemModel(
						imageSize.width(), imageSize.height());
					contourItemModelList.append(contourItemModel);
				}
			}
			if(firstRead)
			{
				m_imageBuilder->addMaskView(maskView);

				if(objectSumAreaPercentString != "-")
				{
					std::tr1::shared_ptr<ICEODB_NS::MaskView> objectMaskView;
					QColor objectColor = styleColorMap.value(objectColorName);
					QStringList objectFilterParamNameList;
					if(filterParamNameList.size() > 1)
					{
						objectFilterParamNameList.append(filterParamNameList[1]);
					}
					if(objectColorNotUsed.isEmpty())
					{
						objectMaskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(
							new ICEODB_NS::MaskView(objectName, objectColor, false,
							255, objectFilterParamNameList));
					}
					else
					{
						objectMaskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(
							new ICEODB_NS::MaskView(objectName, QColor(), true,
							objectColor.alpha(), objectFilterParamNameList));
					}
					Q_ASSERT(contourItemModel);
					contourItemModel->setMaskView(objectMaskView);
					std::tr1::shared_ptr<ICEODB_NS::LineView> lineView;
					QColor lineColor = styleColorMap.value(lineColorName);
					if(lineColorNotUsed.isEmpty())
					{
						lineView = std::tr1::shared_ptr<ICEODB_NS::LineView>(
							new ICEODB_NS::LineView(objectName, lineColor, false,
							255, objectFilterParamNameList));
					}
					else
					{
						lineView = std::tr1::shared_ptr<ICEODB_NS::LineView>(
							new ICEODB_NS::LineView(objectName, QColor(), true,
							lineColor.alpha(), objectFilterParamNameList));
					}
					Q_ASSERT(contourItemModel);
					contourItemModel->setLineView(lineView);
				}
			}
			if(contourItemModel)
			{
				// TODO refac
				double skippedPointCount = skippedPointCountAndDivKMap.value("skippedCount" +
					rowNumber);
				int unit = skippedPointCountAndDivKMap.value("unit" + QString::number(rowNumber), NONE);
				double divK = skippedPointCountAndDivKMap.value("divK" + QString::number(rowNumber), 1.0);
				contourItemModel->addSumData(objectCount, objectSumArea * divK * divK,
					objectSumArcLength * divK, skippedPointCount);
				contourItemModel->setTransformation(1.0 / divK, unit);
				m_unit = (UNIT)unit;
				m_divK = divK;
			}
		}
	}
}

} // namespace ICEIMAGE_NS
