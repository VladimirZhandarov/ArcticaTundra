#include "imageresizedialog.h"

#include <iceimage/imagebuilder.h>

namespace ICEGUI_NS
{

ImageResizeDialog::ImageResizeDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_imageBuilder(imageBuilder),
	m_lastChangedWidth(true)
{
	ui.setupUi(this);
	Q_ASSERT(!m_imageBuilder.isNull());
	ui.widthValueL->setText(QString::number(m_imageBuilder->getImageSizeWidth()));
	ui.heightValueL->setText(QString::number(m_imageBuilder->getImageSizeHeight()));
	ui.widthPointSB->setValue(m_imageBuilder->getImageSizeWidth());
	ui.heightPointSB->setValue(m_imageBuilder->getImageSizeHeight());
}

ImageResizeDialog::~ImageResizeDialog()
{

}

void ImageResizeDialog::accept()
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setResizeFactor(ui.widthPercentDSB->value() / 100,
		ui.heightPercentDSB->value() / 100);
	m_imageBuilder->apply(true);
	QDialog::accept();
}

void ImageResizeDialog::widthPointSB_valueChanged(int value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	double persent = ((double)value) / m_imageBuilder->getImageSizeWidth() * 100;
	ui.widthPercentDSB->setValue(persent);
	m_lastChangedWidth = true;
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.heightPointSB->setValue(value / factor);
		ui.heightPercentDSB->setValue(persent);
	}
	blockChildSignals(false);
}

void ImageResizeDialog::heightPointSB_valueChanged(int value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	double persent = ((double)value) / m_imageBuilder->getImageSizeHeight() * 100;
	ui.heightPercentDSB->setValue(persent);
	m_lastChangedWidth = false;
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.widthPointSB->setValue(value * factor);
		ui.widthPercentDSB->setValue(persent);
	}
	blockChildSignals(false);
}

void ImageResizeDialog::widthPercentDSB_valueChanged(double value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	ui.widthPointSB->setValue(m_imageBuilder->getImageSizeWidth() * value / 100);
	m_lastChangedWidth = true;
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.heightPointSB->setValue(m_imageBuilder->getImageSizeHeight() * value / 100);
		ui.heightPercentDSB->setValue(value);
	}
	blockChildSignals(false);
}

void ImageResizeDialog::heightPercentDSB_valueChanged(double value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	ui.heightPointSB->setValue(m_imageBuilder->getImageSizeHeight() * value / 100);
	m_lastChangedWidth = false;
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.widthPointSB->setValue(m_imageBuilder->getImageSizeWidth() * value / 100);
		ui.widthPercentDSB->setValue(value);
	}
	blockChildSignals(false);
}

void ImageResizeDialog::keepAspectRatioChB_stateChanged(int state)
{
	if(state != Qt::Unchecked)
	{
		if(m_lastChangedWidth)
		{
			widthPointSB_valueChanged(ui.widthPointSB->value());
		}
		else
		{
			heightPointSB_valueChanged(ui.heightPointSB->value());
		}
	}
}

void ImageResizeDialog::blockChildSignals(bool block)
{
	ui.heightPointSB->blockSignals(block);
	ui.widthPointSB->blockSignals(block);
	ui.widthPercentDSB->blockSignals(block);
	ui.heightPercentDSB->blockSignals(block);
}

} // namespace ICEGUI_NS
