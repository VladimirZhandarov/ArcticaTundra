#include "imagebuffer.h"

ImageBuffer::ImageBuffer(QObject* parent) : QObject(parent), m_currentShowImageIndex(0),
	m_lastReadyImageIndex(1)
{

}

ImageBuffer::~ImageBuffer()
{

}

void ImageBuffer::setImage(const QImage& image)
{
	QMutexLocker locker(&m_mutex);
	int index = 0;
	for(; index<NUM_OF_IMAGES_IN_BUFFER; index++)
	{
		if(index != m_currentShowImageIndex && index != m_lastReadyImageIndex)
		{
			break;
		}
	}
	Q_ASSERT(index < NUM_OF_IMAGES_IN_BUFFER);
	m_images[index] = image;
	m_lastReadyImageIndex = index;
}

const QImage& ImageBuffer::getImage()
{
	QMutexLocker locker(&m_mutex);
	m_currentShowImageIndex = m_lastReadyImageIndex;
	Q_ASSERT(m_currentShowImageIndex < NUM_OF_IMAGES_IN_BUFFER);
	return m_images[m_currentShowImageIndex];
}
