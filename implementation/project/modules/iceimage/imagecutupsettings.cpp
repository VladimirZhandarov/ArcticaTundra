#include "imagecutupsettings.h"

namespace ICEIMAGE_NS
{

QString ImageCutUpSettings::imageFilenameKey = "imageFilename";
QString ImageCutUpSettings::imageWidthKey = "imageWidth";
QString ImageCutUpSettings::imageHeightKey = "imageHeight";
QString ImageCutUpSettings::imageCutUpArrayKey = "imageCutUp";
QString ImageCutUpSettings::imageCutUpFilenameKey = "filename";
QString ImageCutUpSettings::imageCutUpXKey = "x";
QString ImageCutUpSettings::imageCutUpYKey = "y";
QString ImageCutUpSettings::suffix = "icl";

ImageCutUpSettings::ImageCutUpSettings(const QFileInfo& fileInfo, QObject* parent) :
	QSettings(fileInfo.absoluteFilePath(), QSettings::IniFormat, parent)
{

}

ImageCutUpSettings::~ImageCutUpSettings()
{

}

void ImageCutUpSettings::setImageFilename(const QString& filename)
{
	setValue(imageFilenameKey, filename);
}

QString ImageCutUpSettings::getImageFilename() const
{
	return value(imageFilenameKey).toString();
}

void ImageCutUpSettings::setImageSize(const QSize& size)
{
	setValue(imageWidthKey, size.width());
	setValue(imageHeightKey, size.height());
}

QSize ImageCutUpSettings::getImageSize() const
{
	int width = value(imageWidthKey).toInt();
	int height = value(imageHeightKey).toInt();
	return QSize(width, height);
}

void ImageCutUpSettings::setTileImageList(const QList<TileImage>& tileImageList)
{
	beginWriteArray(imageCutUpArrayKey);
	for(int i=0; i<tileImageList.size(); i++)
	{
		setArrayIndex(i);
		setValue(imageCutUpFilenameKey, tileImageList[i].filename);
		setValue(imageCutUpXKey, tileImageList[i].x);
		setValue(imageCutUpYKey, tileImageList[i].y);
	}
	endArray();
}

QList<TileImage> ImageCutUpSettings::getTileImageList()
{
	QList<TileImage> result;
	int size = beginReadArray(imageCutUpArrayKey);
	for(int i=0; i<size; i++)
	{
		TileImage tileImage;
		setArrayIndex(i);
		tileImage.filename = value(imageCutUpFilenameKey).toString();
		tileImage.x = value(imageCutUpXKey).toInt();
		tileImage.y = value(imageCutUpYKey).toInt();
		result.append(tileImage);
	}
	endArray();
	return result;
}

} // namespace ICEIMAGE_NS
