#include "imagecutupthread.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "imagecutupsettings.h"

namespace ICEIMAGE_NS
{

ImageCutUpThread::ImageCutUpThread(QSharedPointer<GdalImageBuilder> imageBuilder,
	const QString& outDir, const QSizeF& tileSize, int maxSteps, int jpegQuality,
	QObject* parent) : QThread(parent), m_imageBuilder(imageBuilder), m_outDir(outDir),
	m_tileSize(tileSize), m_maxSteps(maxSteps), m_jpegQuality(jpegQuality),
	m_errorFlag(false), m_stopFlag(false)
{

}

ImageCutUpThread::~ImageCutUpThread()
{

}

bool ImageCutUpThread::isNoError() const
{
	return !m_errorFlag;
}

void ImageCutUpThread::stop()
{
	m_stopFlag = true;
}

void ImageCutUpThread::run()
{
	if(m_imageBuilder.isNull())
	{
		m_errorFlag = true;
		return;
	}
	m_errorFlag = false;
	int widthMax = m_imageBuilder->getImageSizeWidth();
	int heightMax = m_imageBuilder->getImageSizeHeight();
	emit maximumCalculated(m_maxSteps);
	QFileInfo fileInfo(m_imageBuilder->getFilename());
	QFileInfo imageCutUpSettingsFileInfo(QDir(m_outDir), fileInfo.completeBaseName() +
		"." + ImageCutUpSettings::suffix);
	ImageCutUpSettings imageCutUpSettings(imageCutUpSettingsFileInfo, this);
	imageCutUpSettings.setImageSize(QSize(widthMax, heightMax));
	imageCutUpSettings.setImageFilename(fileInfo.absoluteFilePath());

	int stepCount = 0;
	int indexX = 1;
	QList<TileImage> tileImageList;
	for(double n=0; n<widthMax; n = n + m_tileSize.width())
	{
		int x1 = qRound(n);
		int x2 = qRound(n + m_tileSize.width());
		int indexY = 1;
		for(double m=0; m<heightMax; m = m + m_tileSize.height())
		{
			if(m_stopFlag)
			{
				return;
			}
			int y1 = qRound(m);
			int y2 = qRound(m + m_tileSize.height());
			QRect rect = QRect(QPoint(x1, y1), QPoint(x2, y2));

			//TODO refac m_imageBuilder->adjustedRect(rect)
			cv::Mat matImage = m_imageBuilder->createMatCutUpImage(ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE,
				m_imageBuilder->adjustedRect(rect));
			if(!matImage.empty())
			{
				QString imageCutUpFilename = fileInfo.completeBaseName() + "_" +
					QString::number(indexY) + "_" + QString::number(indexX) +
					"." + fileInfo.suffix();
				QFileInfo resultFileInfo(QDir(m_outDir), imageCutUpFilename);
				QString filePath = resultFileInfo.absoluteFilePath();
				TileImage tileImage;
				tileImage.x = x1;
				tileImage.y = y1;
				std::vector<int> params;
				if(fileInfo.suffix() == "jpg")
				{
					params.push_back(CV_IMWRITE_JPEG_QUALITY);
					params.push_back(m_jpegQuality);
				}
				if(cv::imwrite(filePath.toStdString().c_str(), matImage, params))
				{
					tileImage.filename = imageCutUpFilename;
				}
				else
				{
					m_errorFlag = true;
				}
				tileImageList.append(tileImage);
			}
			stepCount++;
			emit valueChanged(stepCount);
			indexY++;
		}
		indexX++;
	}
	imageCutUpSettings.setTileImageList(tileImageList);
}

} // namespace ICEIMAGE_NS
