#include "objecttablewidget.h"

#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>

#include <QtGui/QAbstractProxyModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include "contoursortproxymodel.h"

#include <iceimage/contouritemmodel.h>
#include <iceimage/imagebuilder.h>

//TODO refac
#include <icecore/global.h>
#include <iceodb/odb/lineview.hxx>

#include <icecore/core.h>
#include <icecore/settings.h>

#include <iceqwt/iceunitratioqwttransform.h>

namespace ICEGUI_NS
{

ObjectTableWidget::ObjectTableWidget(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	int index, QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f), m_imageBuilder(imageBuilder),
	m_index(index), m_transformation(0), m_halfArcLength(false)
{
	ui.setupUi(this);

	ui.maskViewWidget->setReadOnly(true);
	ui.maskViewWidget->setLabelText(tr("Заливка"));
	ui.lineViewWidget->setReadOnly(true);

	ICEIMAGE_NS::ContourItemModel* contourItemModel = m_imageBuilder->getContourItemModel(m_index);
	if(!contourItemModel)
	{
		return;
	}
	ui.maskViewWidget->setMaskView(contourItemModel->getMaskView());
	ui.lineViewWidget->setLineView(contourItemModel->getLineView());

	connect(contourItemModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(onCurrentContourDataChanged(const QModelIndex&, const QModelIndex&)));

	ui.treeView->setAlternatingRowColors(true);

	connect(ui.treeView, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(onTreeViewClicked(const QModelIndex&)));

	ui.treeView->setModel(contourItemModel);
	ui.treeView->sortByColumn(0, Qt::AscendingOrder);

	if(ui.treeView->selectionModel())
	{
		connect(ui.treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
			this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}
	ui.totalObjectCountValueL->setText(QString::number(contourItemModel->getObjectCount()));
	ui.totalAreaValueL->setText(QString("%1 %2").arg(contourItemModel->getObjectAreaSum()).
		arg(contourItemModel->getSqUnitName()));
	ui.totalAreaValueInPercentL->setText(QString("%1%").arg(contourItemModel->getObjectAreaPercent()));
	ui.totalArcLengthValueL->setText(QString("%1").arg(contourItemModel->getArcLengthSum()));
	onSelectionChanged(QItemSelection(), QItemSelection());
}

ObjectTableWidget::~ObjectTableWidget()
{

}

bool ObjectTableWidget::isNull() const
{
	Q_ASSERT(!m_imageBuilder.isNull());
	return (m_imageBuilder->getContourItemModel(m_index) == 0);
}

QString ObjectTableWidget::getFilterName() const
{
	Q_ASSERT(!m_imageBuilder.isNull());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = m_imageBuilder->getContourItemModel(m_index);
	if(contourItemModel)
	{
		return contourItemModel->getFilterName();
	}
	else
	{
		return "";
	}
}

QString ObjectTableWidget::getName() const
{
	Q_ASSERT(!m_imageBuilder.isNull());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = m_imageBuilder->getContourItemModel(m_index);
	if(contourItemModel)
	{
		return contourItemModel->getName();
	}
	else
	{
		return "";
	}
}

void ObjectTableWidget::updateState(ICEQWT_NS::IceUnitRatioQwtTransform* transformation,
	bool halfArcLength)
{
	m_transformation = transformation;
	m_halfArcLength = halfArcLength;
	if(m_halfArcLength)
	{
		ui.totalArcLengthL->setText(tr("Пол периметра:"));
	}
	else
	{
		ui.totalArcLengthL->setText(tr("Периметр:"));
	}
	Q_ASSERT(!m_imageBuilder.isNull());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = m_imageBuilder->getContourItemModel(m_index);
	if(!contourItemModel)
	{
		return;
	}
	if(m_transformation)
	{
		//TODO refac
		contourItemModel->setTransformation(m_transformation->ratio(), m_transformation->unit());
	}
	else
	{
		contourItemModel->setTransformation(1.0, 0);
	}
	onCurrentContourDataChanged(QModelIndex(), QModelIndex());
}

void ObjectTableWidget::currentRowChange()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(ui.treeView->model());
	if(!sortModel || !ui.treeView->selectionModel())
	{
		return;
	}
	Q_ASSERT(!m_imageBuilder.isNull());
	//TODO refac
	QAbstractItemModel* contourItemModel = ui.treeView->model();
	ICEIMAGE_NS::ContourItemModel* contourItemModel2 = m_imageBuilder->getContourItemModel(m_index);
	if(!contourItemModel || !contourItemModel2)
	{
		return;
	}
	for(int i=0; i<contourItemModel->rowCount(); i++)
	{
		QModelIndex modelIndex = contourItemModel->index(i, 0);
		if(modelIndex.isValid() &&
			(contourItemModel->data(modelIndex).toInt() -1)== contourItemModel2->getOutsideSelectedIndex())
		{
			ui.treeView->selectionModel()->setCurrentIndex(modelIndex,
				QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
			ui.treeView->scrollTo(modelIndex);
			onSelectionChanged(QItemSelection(), QItemSelection());
			return;
		}
	}
	ui.treeView->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Clear);
	onSelectionChanged(QItemSelection(), QItemSelection());
}

QString ObjectTableWidget::createHtmlFile(const QStringList& fileNameList, const QString& destinationDir,
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder, ICEIMAGE_NS::UNIT unit,
	double divK, bool halfArcLength, const QList<ICEIMAGE_NS::ContourItemModel*>& summaryContourItemModelList)
{
	if(imageBuilder.isNull() || fileNameList.isEmpty() || destinationDir.isEmpty())
	{
		return "";
	}
	QDir dir(destinationDir);

	QString fileHtmlPath = dir.absoluteFilePath("summary.html"); //TODO refac
	if(summaryContourItemModelList.isEmpty())
	{
		QFileInfo fileInfo(fileNameList.first());
		fileHtmlPath = dir.absoluteFilePath(fileInfo.completeBaseName() + ".html");
	}
	QFile htmlFile(fileHtmlPath);
	if(!htmlFile.open(QFile::WriteOnly | QFile::Truncate))
	{
		return false;
	}
	int maskListSize = imageBuilder->getMaskListSize();
	QTextStream stream(&htmlFile);
	stream.setCodec(QTextCodec::codecForName("Windows-1251"));

	stream << tabHtmlLine("<?xml version=\"1.0\" encoding=\"windows-1251\"?>", 0);
	stream << tabHtmlLine("<html>", 0);
	stream << tabHtmlLine("<head>", 1);
	if(summaryContourItemModelList.isEmpty())
	{
		QFileInfo fileInfo(fileNameList.first());
		stream << tabHtmlLine("<title>" + tr("Отчет к изображению ") +
			fileInfo.completeBaseName() + "." + fileInfo.suffix() + "</title>", 2);
	}
	else
	{
		stream << tabHtmlLine("<title>" + tr("Суммарный отчет к изображениям") +
			"</title>", 2);
	}
	stream << tabHtmlLine("<style type=\"text/css\">", 2);
	stream << tabHtmlLine("table {", 3);
	stream << tabHtmlLine("width: 100%;", 4);
	stream << tabHtmlLine("}", 3);
	stream << tabHtmlLine("td {", 3);
	stream << tabHtmlLine("border: 1px solid #333;", 4);
	stream << tabHtmlLine("padding: 5px;", 4);
	stream << tabHtmlLine("opacity: 1.0;", 4);
	stream << tabHtmlLine("filter: alpha(Opacity=100);", 4);
	stream << tabHtmlLine("}", 3);
	for(int i=0; i<maskListSize; i++)
	{
		stream << tabHtmlLine(".mask" + QString::number(i + 1) + "Color {", 3);
		std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = imageBuilder->getColorMaskView(i);
		if(maskView.get())
		{
			if(maskView->isUseImage())
			{
				double alphaValue = maskView->getAlphaImageChannel();
				alphaValue = alphaValue / 255;
				stream << tabHtmlLine("opacity: " + QString::number(alphaValue) +
					";", 4);
				int alphaPercent = alphaValue * 100;
				stream << tabHtmlLine("filter: alpha(Opacity=" + QString::number(alphaPercent) +
					");", 4);
			}
			else
			{
				QColor color = maskView->getColor();
				stream << tabHtmlLine(QString("background: #%1%2%3").arg(color.red(), 2, 16, QChar('0')).
					arg(color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')) +
					";", 4);
				double alphaValue = color.alphaF();
				stream << tabHtmlLine("opacity: " + QString::number(alphaValue) +
					";", 4);
				int alphaPercent = alphaValue * 100;
				stream << tabHtmlLine("filter: alpha(Opacity=" + QString::number(alphaPercent) +
					");", 4);
			}
		}
		stream << tabHtmlLine("}", 3);

		ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
		if(contourItemModel)
		{
			stream << tabHtmlLine(".object" + QString::number(i + 1) + "Color {", 3);
			std::tr1::shared_ptr<ICEODB_NS::MaskView> maskViewContour = contourItemModel->getMaskView();
			if(maskViewContour.get() && !maskViewContour->isUseImage())
			{
				QColor color = maskViewContour->getColor();
				stream << tabHtmlLine(QString("background: #%1%2%3").arg(color.red(), 2, 16, QChar('0')).
					arg(color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')) +
					";", 4);
				double alphaValue = color.alphaF();
				stream << tabHtmlLine("opacity: " + QString::number(alphaValue) +
					";", 4);
				int alphaPercent = alphaValue * 100;
				stream << tabHtmlLine("filter: alpha(Opacity=" + QString::number(alphaPercent) +
					");", 4);
			}
			stream << tabHtmlLine("}", 3);

			stream << tabHtmlLine(".line" + QString::number(i + 1) + "Color {", 3);
			std::tr1::shared_ptr<ICEODB_NS::LineView> lineViewContour = contourItemModel->getLineView();
			if(lineViewContour.get() && !lineViewContour->isUseImage())
			{
				QColor color = lineViewContour->getColor();
				stream << tabHtmlLine(QString("background: #%1%2%3").arg(color.red(), 2, 16, QChar('0')).
					arg(color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')) +
					";", 4);
				double alphaValue = color.alphaF();
				stream << tabHtmlLine("opacity: " + QString::number(alphaValue) +
					";", 4);
				int alphaPercent = alphaValue * 100;
				stream << tabHtmlLine("filter: alpha(Opacity=" + QString::number(alphaPercent) +
					");", 4);
			}
			stream << tabHtmlLine("}", 3);
		}
	}
	stream << tabHtmlLine("</style>", 2);
	stream << tabHtmlLine("</head>", 1);
	stream << tabHtmlLine("<body>", 1);
	if(summaryContourItemModelList.isEmpty())
	{
		stream << tabHtmlLine("<p>" + tr("Имя исходного файла: ") +
			imageBuilder->getFilename() + "</p>", 2);
		stream << tabHtmlLine("<p>" + tr("Размер исходного изображения в пикселях: ") +
			QString::number(imageBuilder->getImageSizeWidth()) + tr(" x ") +
			QString::number(imageBuilder->getImageSizeHeight()) + "</p>", 2);

		if(unit != ICEIMAGE_NS::NONE)
		{
			//TODO refac
			QString unitString = ICEQWT_NS::IceUnitRatioQwtTransform::unitName((ICEIMAGE_NS::UNIT)unit);
			stream << tabHtmlLine("<p>" + tr("Размер исходного изображения метрический: ") +
				QString::number(imageBuilder->getImageSizeWidth() / divK) +
				tr(" x ") + QString::number(imageBuilder->getImageSizeHeight() / divK) +
				" " + unitString + "</p>", 2);
			stream << tabHtmlLine("<p>" + tr("Масштаб на местности, ") + unitString +
				tr("/пиксель: ") + QString::number(1.0 / divK) + "</p>", 2);
		}
		else
		{
			stream << tabHtmlLine("<p>" + tr("Размер исходного изображения метрический: нет</p>"), 2);
		}
		QFileInfo sourceFileInfo(imageBuilder->getFilename());
		stream << tabHtmlLine("<p>" + tr("Размер исходного изображения в файловой системе: ") +
			ICEIMAGE_NS::BaseImageBuilder::createMemoryString(sourceFileInfo.size()) +
			"</p>", 2);
		QFileInfo fileInfo(fileNameList.first());
		stream << tabHtmlLine("<p>" + tr("Имя сохраняемого файла результирующего изображения: ") +
			fileInfo.completeBaseName() + "." + fileInfo.suffix() + "</p>", 2);
	}
	else
	{
		for(int i=0;i<fileNameList.size();i++)
		{
			QFileInfo fileInfo(fileNameList[i]);
			stream << tabHtmlLine("<p>" + tr("Имя сохраняемого файла результирующего изображения: ") +
				fileInfo.completeBaseName() + "." + fileInfo.suffix() + "</p>", 2);
		}
	}
	stream << tabHtmlLine("<p>" + tr("Дата и время обработки: ") +
		QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") + "</p>", 2);

	stream << tabHtmlLine("<table border=\"1\">", 2);
	stream << tabHtmlLine("<tr>", 3);
	stream << tabHtmlLine("<td colspan=\"3\">" + tr("Оптические маски") + "</td>", 4);
	stream << tabHtmlLine("<td colspan=\"7\">" + tr("Объекты и типы объектов") + "</td>", 4);
	stream << tabHtmlLine("<td rowspan=\"2\">" + tr("Фильтры") + "</td>", 4);
	stream << tabHtmlLine("</tr>", 3);
	stream << tabHtmlLine("<tr>", 3);
	stream << tabHtmlLine("<td>" + tr("№") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Цвет") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Имя маски") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Цвет заливки") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Цвет контура") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Имя типа объектов") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Количество объектов") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Площадь") + "</td>", 4);
	stream << tabHtmlLine("<td>" + tr("Доля в %") + "</td>", 4);
	if(!halfArcLength)
	{
		stream << tabHtmlLine("<td>" + tr("Длина контуров") + "</td>", 4);
	}
	else
	{
		stream << tabHtmlLine("<td>" + tr("Пол длины контуров") + "</td>", 4);
	}
	stream << tabHtmlLine("</tr>", 3);
	for(int i=0; i<maskListSize; i++)
	{
		stream << tabHtmlLine("<tr>", 3);
		stream << tabHtmlLine("<td>" + QString::number(i + 1) + "</td>", 4);
		std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = imageBuilder->getColorMaskView(i);
		QString maskFilterName = "";
		QString objectFilterName = "";
		if(maskView.get())
		{
			if(!maskView->isUseImage())
			{
				stream << tabHtmlLine("<td class=\"mask" + QString::number(i + 1) +
					"Color\"/>", 4);
			}
			else
			{
				stream << tabHtmlLine("<td>" + tr("Нет") + "</td>", 4);
			}
			maskFilterName = maskView->getFilterParamNameList().join("->");
		}
		else
		{
			stream << tabHtmlLine("<td/>", 4);
		}
		stream << tabHtmlLine("<td>" + imageBuilder->getMaskName(i) + "</td>", 4);
		ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
		std::tr1::shared_ptr<ICEODB_NS::MaskView> maskViewContour;
		std::tr1::shared_ptr<ICEODB_NS::LineView> lineViewContour;
		if(contourItemModel)
		{
			maskViewContour = contourItemModel->getMaskView();
			lineViewContour = contourItemModel->getLineView();
		}
		if(maskViewContour.get())
		{
			if(!maskViewContour->isUseImage())
			{
				stream << tabHtmlLine("<td class=\"object" + QString::number(i + 1) +
					"Color\"/>", 4);
			}
			else
			{
				stream << tabHtmlLine("<td>" + tr("Нет") + "</td>", 4);
			}
			objectFilterName = maskViewContour->getFilterParamNameList().join("->");
		}
		else
		{
			stream << tabHtmlLine("<td/>", 4);
		}
		if(lineViewContour.get())
		{
			if(!lineViewContour->isUseImage())
			{
				stream << tabHtmlLine("<td class=\"line" + QString::number(i + 1) +
					"Color\"/>", 4);
			}
			else
			{
				stream << tabHtmlLine("<td>" + tr("Нет") + "</td>", 4);
			}
		}
		else
		{
			stream << tabHtmlLine("<td/>", 4);
		}
		stream << tabHtmlLine("<td>" + imageBuilder->getContoursName(i) + "</td>", 4);
		
		if(!summaryContourItemModelList.isEmpty())
		{
			if(i < summaryContourItemModelList.size())
			{
				contourItemModel = summaryContourItemModelList[i];
			}
			else
			{
				contourItemModel = 0;
			}
		}
		if(contourItemModel)
		{
			stream << tabHtmlLine("<td>" + QString::number(contourItemModel->getObjectCount()) +
				"</td>", 4);
			stream << tabHtmlLine("<td>" + QString::number(contourItemModel->getObjectAreaSum()) +
				" " + contourItemModel->getSqUnitName() + "</td>", 4);
			stream << tabHtmlLine("<td>" + QString::number(contourItemModel->getObjectAreaPercent()) +
				"</td>", 4);
			if(!halfArcLength)
			{
				stream << tabHtmlLine("<td>" + QString::number(contourItemModel->getArcLengthSum()) +
					" " + contourItemModel->getUnitName() + "</td>", 4);
			}
			else
			{
				stream << tabHtmlLine("<td>" + QString::number(contourItemModel->getArcLengthSum() / 2) +
					" " + contourItemModel->getUnitName() + "</td>", 4);
			}
		}
		else
		{
			stream << tabHtmlLine("<td>-</td>", 4);
			stream << tabHtmlLine("<td>-</td>", 4);
			stream << tabHtmlLine("<td>-</td>", 4);
			stream << tabHtmlLine("<td>-</td>", 4);
		}
		stream << tabHtmlLine("<td>" + maskFilterName + "<br/>" + objectFilterName +
			"</td>", 4);
		stream << tabHtmlLine("</tr>", 3);
	}
	stream << tabHtmlLine("</table>", 2);
	stream << tabHtmlLine("<pre>", 2);
	for(int i=0; i<maskListSize; i++)
	{
		ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
		if(contourItemModel)
		{
			stream << tabHtmlLine("<input type=\"hidden\" name=\"skippedCount" +
				QString::number(i + 1) + "\" value=\"" +
				QString::number(contourItemModel->getSkippedAreaSum()) + "\"/>", 3);
			stream << tabHtmlLine("<input type=\"hidden\" name=\"unit" +
				QString::number(i + 1) + "\" value=\"" +
				QString::number(contourItemModel->getUnit()) + "\"/>", 3);
			stream << tabHtmlLine("<input type=\"hidden\" name=\"divK" +
				QString::number(i + 1) + "\" value=\"" +
				QString::number(1.0 / contourItemModel->getDivK()) + "\"/>", 3);
		}
	}
	stream << tabHtmlLine("</pre>", 2);
	stream << tabHtmlLine("</body>", 1);
	stream << tabHtmlLine("</html>", 0);

	stream.flush();
	htmlFile.close();
	return "";
}

QString ObjectTableWidget::createCsvFiles(const QString& filename, const QString& destinationDir,
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder)
{
	if(imageBuilder.isNull() || filename.isEmpty() || destinationDir.isEmpty())
	{
		return "";
	}
	QFileInfo fileInfo(filename);
	QDir dir(destinationDir);
	for(int i=0; i<imageBuilder->getContourItemModelListSize(); i++)
	{
		ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
		if(contourItemModel)
		{
			QString fileCsvPath = dir.absoluteFilePath(fileInfo.completeBaseName() +
				"_" + QString::number(i + 1) + ".csv");
			ContourSortProxyModel proxyModel;
			proxyModel.setSourceModel(contourItemModel);
			if(!ObjectTableWidget::createCsvFile(fileCsvPath, contourItemModel,
				&proxyModel, imageBuilder->getFilename(),
				QSize(imageBuilder->getImageSizeWidth(), imageBuilder->getImageSizeHeight())))
			{
				return fileCsvPath;
			}
		}
	}
	return "";
}

bool ObjectTableWidget::createCsvFile(const QString& filename, ICEIMAGE_NS::ContourItemModel* contourItemModel,
	QAbstractItemModel* dataModel, const QString& imageFilename, const QSize& imageSize)
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(dataModel);
	if(filename.isEmpty() || !contourItemModel || !sortModel)
	{
		return false;
	}

	QStringList headerInfo;
	headerInfo << tr("Имя исходного файла:") + ";" + imageFilename;
	headerInfo << tr("Имя фильтра:") + ";" + contourItemModel->getFilterName();
	headerInfo << tr("Имя типа объектов:") + ";" + contourItemModel->getName();
	double divK = contourItemModel->getDivK();
	if(contourItemModel->getUnit() != ICEIMAGE_NS::NONE)
	{
		headerInfo << tr("Масштаб на местности, ") + contourItemModel->getUnitName() +
			tr("/пиксель:") + ";" + QString::number(1.0 / divK);
	}
	headerInfo << tr("Общая площадь снимка:") + ";" + QString::number(imageSize.width() / divK *
		imageSize.height() / divK) + " " + contourItemModel->getSqUnitName();
	headerInfo << tr("Cуммарная площадь найденных объектов:") + ";" +
		QString::number(contourItemModel->getObjectAreaSum()) + " " +
		contourItemModel->getSqUnitName();
	headerInfo << tr("Cуммарная отброшенная площадь:") + ";" +
		QString::number(contourItemModel->getSkippedAreaSum()) + " " +
		contourItemModel->getSqUnitName();
	headerInfo << tr("Доля площади найденных объектов от общей площади:") +
		";" + QString::number(contourItemModel->getObjectAreaPercent()) + "%";
	headerInfo << tr("Суммарная длина периметров найденных объектов:") +
		";" + QString::number(contourItemModel->getArcLengthSum()) + "%";

	QFile file(filename);
	QFileInfo fileInfo(filename);
	if(!fileInfo.exists())
	{
		QDir dir(fileInfo.absolutePath());
		if(!dir.mkpath(QString("./")))
		{
			return false;
		}
	}
	if(!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		return false;
	}
	QTextStream stream(&file);
	stream.setCodec(QTextCodec::codecForName("Windows-1251"));

	for(int i=0; i<headerInfo.size(); i++)
	{
		stream << headerInfo[i] << ";" << "\n";
	}

	//записываем заголовок
	int count = sortModel->columnCount();
	for(int i=0; i<count; i++)
	{
		stream << sortModel->headerData(i, Qt::Horizontal).toString() << ";";
	}
	stream << "\n";

	//записываем значения. по строкам
	int size = sortModel->rowCount();
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<count; j++)
		{
			QVariant val = sortModel->data(sortModel->index(i, j));
			if(val.isValid())
			{
				if(j != ICEIMAGE_NS::ContourItemModel::COMMENT_COL)
				{
					stream << QString::number(val.toDouble()).replace(".", ",") << ";";
				}
				else
				{
					stream << val.toString() << ";";
				}
			}
			else
			{
				stream << "\t" << ";";
			}
		}
		stream << "\n";
	}
	stream.flush();
	file.close();
	return true;
}

void ObjectTableWidget::onCurrentContourDataChanged(const QModelIndex& /*topLeft*/,
	const QModelIndex& /*bottomRight*/)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = m_imageBuilder->getContourItemModel(m_index);
	if(!contourItemModel)
	{
		return;
	}
	ui.totalObjectCountValueL->setText(QString::number(contourItemModel->getObjectCount()));
	ui.totalAreaValueL->setText(QString("%1 %2").arg(contourItemModel->getObjectAreaSum()).
		arg(contourItemModel->getSqUnitName()));
	ui.totalAreaValueInPercentL->setText(QString("%1%").arg(contourItemModel->getObjectAreaPercent()));
	ui.totalArcLengthValueL->setText(QString("%1").arg(contourItemModel->getArcLengthSum()));
	onSelectionChanged(QItemSelection(), QItemSelection());
}

void ObjectTableWidget::onSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(ui.treeView->model());
	if(!sortModel || !ui.treeView->selectionModel())
	{
		return;
	}
	Q_ASSERT(!m_imageBuilder.isNull());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = m_imageBuilder->getContourItemModel(m_index);
	if(!contourItemModel)
	{
		return;
	}
	QModelIndexList rows = ui.treeView->selectionModel()->selectedRows();
	double areaSum = 0.0;
	double areaPercent = 0.0;
	double arcLengthSum = 0.0;
	if(!rows.isEmpty())
	{
		QModelIndexList sourceRows;
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			sourceRows.append(sortModel->mapToSource(*iter));
		}
		areaSum = contourItemModel->getObjectAreaSum(sourceRows);
		areaPercent = contourItemModel->getObjectAreaPercent(sourceRows);
		arcLengthSum = contourItemModel->getArcLengthSum(sourceRows);
	}
	ui.selectedObjectCountValueL->setText(QString::number(rows.count()));
	ui.selectedAreaValueL->setText(QString("%1 %2").arg(areaSum).
		arg(contourItemModel->getSqUnitName()));
	ui.selectedAreaValueInPercentL->setText(QString("%1%").arg(areaPercent));
	ui.selectedArcLengthValueL->setText(QString("%1 %2").arg(arcLengthSum).
		arg(contourItemModel->getUnitName()));
}

void ObjectTableWidget::onTreeViewClicked(const QModelIndex& index)
{
	QAbstractItemModel* proxyModel = dynamic_cast<QAbstractProxyModel*>(ui.treeView->model());
	if(!proxyModel && !index.isValid())
	{
		return;
	}
	QModelIndex workIndex = proxyModel->index(index.row(), 0, index.parent());
	bool state = proxyModel->data(workIndex, Qt::CheckStateRole).toBool();
	if(state)
	{
		proxyModel->setData(workIndex, Qt::Unchecked, Qt::CheckStateRole);
	}
	else
	{
		proxyModel->setData(workIndex, Qt::Checked, Qt::CheckStateRole);
	}
}

void ObjectTableWidget::saveToFilePB_clicked()
{
	QString filename =  QFileDialog::getSaveFileName(this, tr("Сохранить данные объектов как..."),
		".", tr("Файл Excel (*.csv);;Текстовый файл (*.txt);;HTML файл (*.html)"));
	if(filename.isEmpty())
	{
		return;
	}
	Q_ASSERT(!m_imageBuilder.isNull());
	QFileInfo fileInfo(filename);
	if(fileInfo.suffix() == "html")
	{
		QString fileHtmlPathError = ICEGUI_NS::ObjectTableWidget::createHtmlFile(
			QStringList() << fileInfo.absoluteFilePath(), fileInfo.absolutePath(),
			m_imageBuilder, m_transformation->unit(), m_transformation->ratio(),
			m_halfArcLength);
		if(fileHtmlPathError.isEmpty())
		{
			QMessageBox::information(this, tr("Экспорт данных"),
				tr("Информация об объектах экспортирована успешно."), tr("Закрыть"));
		}
	}
	else
	{
		if(createCsvFile(filename, m_imageBuilder->getContourItemModel(m_index),
			ui.treeView->model(), m_imageBuilder->getFilename(),
			QSize(m_imageBuilder->getImageSizeWidth(), m_imageBuilder->getImageSizeHeight())))
		{
			QMessageBox::information(this, tr("Экспорт данных"),
				tr("Информация об объектах экспортирована успешно."), tr("Закрыть"));
		}
	}
}

void ObjectTableWidget::onSelectAll()
{
	ui.treeView->selectAll();
	onSelectionChanged(QItemSelection(), QItemSelection());
}

void ObjectTableWidget::onUnselectAll()
{
	ui.treeView->unselectAll();
	onSelectionChanged(QItemSelection(), QItemSelection());
}

QString ObjectTableWidget::tabHtmlLine(const QString& line, unsigned int tabNumber)
{
	if(line.isEmpty())
	{
		return "";
	}
	QString tabString = "";
	for(unsigned int i=0; i<tabNumber; i++)
	{
		tabString += "\t";
	}
	return tabString + line + "\n";
}
} // namespace ICEGUI_NS
