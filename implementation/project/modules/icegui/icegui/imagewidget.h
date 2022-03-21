#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>

#include <QtGui/QImage>

namespace ICEGUI_NS
{

class ImageWidget : public QWidget
{
	Q_OBJECT
public:
	enum ScaledType { NO_SCALED = 0, SIMPLE_SCALED, SCALED_TO_HEIGHT, SCALED_TO_WIDTH };

	ImageWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ImageWidget();

	void setScaledType(ScaledType scaledType);
	void setAspectRatioMode(Qt::AspectRatioMode aspectRatioMode);
	
	void setImage(const QImage& image);
	const QImage& getImage() const;

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);
	void resizeImage(const QImage& image);

	ScaledType m_scaledType;
	Qt::AspectRatioMode m_aspectRatioMode;
	QImage m_image;
	QImage m_imageUsed;
};

} // namespace ICEGUI_NS

#endif // IMAGEWIDGET_H
