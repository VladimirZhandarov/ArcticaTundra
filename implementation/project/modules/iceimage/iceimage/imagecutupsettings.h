#ifndef IMAGECUTUPSETTINGS_H
#define IMAGECUTUPSETTINGS_H

#include "iceimagedef.h"

#include <QtCore/QSettings>

#include <QtCore/QFileInfo>
#include <QtCore/QSize>
#include <QtCore/QStringList>

namespace ICEIMAGE_NS
{

struct TileImage
{
	QString filename;
	int x;
	int y;
};

class ICEIMAGE_EXPORT ImageCutUpSettings : public QSettings
{
	Q_OBJECT
	static QString imageFilenameKey;
	static QString imageWidthKey;
	static QString imageHeightKey;
	static QString imageCutUpArrayKey;
	static QString imageCutUpFilenameKey;
	static QString imageCutUpXKey;
	static QString imageCutUpYKey;

public:
	static QString suffix;

	/**
	* Конструктор
	* @param fileInfo файл настроек разрезки изображения
	* @param parent родительский объект
	*/
	ImageCutUpSettings(const QFileInfo& fileInfo, QObject* parent = 0);

	/**
	* Деструктор
	*/
	~ImageCutUpSettings();

	void setImageFilename(const QString& filename);
	QString getImageFilename() const;

	void setImageSize(const QSize& size);
	QSize getImageSize() const;

	void setTileImageList(const QList<TileImage>& tileImageList);
	QList<TileImage> getTileImageList();
};

} // namespace ICEIMAGE_NS

#endif // IMAGECUTUPSETTINGS_H
