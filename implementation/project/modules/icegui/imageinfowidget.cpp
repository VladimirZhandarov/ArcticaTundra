#include "imageinfowidget.h"

#include <QtGui/QPainter>

namespace ICEGUI_NS
{

ImageInfoWidget::ImageInfoWidget(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f),
	m_index(-1)
{
	ui.setupUi(this);
}

ImageInfoWidget::~ImageInfoWidget()
{

}

void ImageInfoWidget::setScaledType(ImageWidget::ScaledType scaledType)
{
	ui.imageWidget->setScaledType(scaledType);
}

void ImageInfoWidget::setAspectRatioMode(Qt::AspectRatioMode aspectRatioMode)
{
	ui.imageWidget->setAspectRatioMode(aspectRatioMode);
}

void ImageInfoWidget::setImage(int index, const QString& name, const QString& filename,
	const QImage& image)
{
	m_index = index;
	ui.maskName->setText(name);
	m_filename = filename;
	ui.imageWidget->setImage(image);
	ui.heigthL->setText(tr("Высота: ") + QString::number(image.height()));
	ui.widthL->setText(tr("Ширина: ") + QString::number(image.width()));
}

const QImage& ImageInfoWidget::getImage() const
{
	return ui.imageWidget->getImage();
}

const QString& ImageInfoWidget::getFilename() const
{
	return m_filename;
}

int ImageInfoWidget::getIndex() const
{
	return m_index;
}

} // namespace ICEGUI_NS
