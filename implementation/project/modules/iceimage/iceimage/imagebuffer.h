#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QtCore/QObject>

#include <QtCore/QMutex>

#include <QtGui/QImage>

#define NUM_OF_IMAGES_IN_BUFFER 3

class ImageBuffer : public QObject
{
public:
	/**
	* Конструктор
	* @param parent родительский объект
	*/
	ImageBuffer(QObject* parent = 0);

	/**
	* Деструктор
	*/
	~ImageBuffer();

	void setImage(const QImage& image);
	const QImage& getImage();

private:
	QImage m_images[NUM_OF_IMAGES_IN_BUFFER];
	QMutex m_mutex;
	int m_currentShowImageIndex;
	int m_lastReadyImageIndex;
};

#endif // IMAGEBUFFER_H
