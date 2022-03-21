#include "imagewidget.h"

#include <QtGui/QPainter>

namespace ICEGUI_NS
{

ImageWidget::ImageWidget(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f),
	m_scaledType(SIMPLE_SCALED), m_aspectRatioMode(Qt::KeepAspectRatio)
{

}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::setScaledType(ScaledType scaledType)
{
	m_scaledType = scaledType;
}

void ImageWidget::setAspectRatioMode(Qt::AspectRatioMode aspectRatioMode)
{
	m_aspectRatioMode = aspectRatioMode;
}

void ImageWidget::setImage(const QImage& image)
{
	m_image = image.copy();
	resizeImage(m_image);
}

const QImage& ImageWidget::getImage() const
{
	return m_image;
}

void ImageWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawImage(QRect(0, 0, m_imageUsed.width(), m_imageUsed.height()), m_imageUsed);
	QWidget::paintEvent(event);
}

void ImageWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	resizeImage(m_image);
}

void ImageWidget::resizeImage(const QImage& image)
{
	if(m_scaledType == SIMPLE_SCALED)
	{
		m_imageUsed = image.scaled(size(), m_aspectRatioMode);
	}
	else if(m_scaledType == SCALED_TO_HEIGHT)
	{
		m_imageUsed = image.scaledToHeight(height());
	}
	else if(m_scaledType == SCALED_TO_WIDTH)
	{
		m_imageUsed = image.scaledToWidth(width());
	}
	else
	{
		m_imageUsed = image;
	}
	repaint();
}

} // namespace ICEGUI_NS
