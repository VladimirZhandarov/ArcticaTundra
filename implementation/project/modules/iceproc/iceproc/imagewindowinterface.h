#ifndef IMAGEWINDOWINTERFACE_H
#define IMAGEWINDOWINTERFACE_H

#include "iceprocdef.h"

#include <QtGui/QMainWindow>

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEPROC_NS
{

class ICEPROC_EXPORT ImageWindowInterface : public QMainWindow
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	* @param flags флаги окна
	*/
	ImageWindowInterface(QWidget* parent = 0, Qt::WFlags flags = 0);

	/**
	* Деструктор
	*/
	~ImageWindowInterface();

	virtual QString getCurrentFilename() const = 0;
	virtual QImage getCurrentSelectedImage(const QRect& rect = QRect()) const = 0;
	virtual QRect getCurrentSelectedRect() const = 0;
	virtual QImage getCurrentImage() const = 0;
	virtual QSharedPointer<ICEIMAGE_NS::ImageBuilder> getCurrentImageBuilder() const = 0;
};

} // namespace ICEPROC_NS

#endif // IMAGEWINDOWINTERFACE_H
