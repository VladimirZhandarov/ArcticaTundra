//==============================================================================
// Название:        Диалоговое окно "Яркость\контрастность"
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: shchav $
// Создан:    2011.11.08
// Изменен:   $Date: 2013-04-19 18:46:52 +0400 (РџС‚, 19 Р°РїСЂ 2013) $
// Модуль:    icegui
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: brightnesscontrastdialog.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "brightnesscontrastdialog.h"

#include <iceimage/imagebuilder.h>

#include <QtGui/QCloseEvent>

namespace ICEGUI_NS
{

BrightnessContrastDialog::BrightnessContrastDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_imageBuilder(imageBuilder)
{
	ui.setupUi(this);

	Q_ASSERT(!m_imageBuilder.isNull());
	m_startBrightness = m_imageBuilder->getBrightness();
	ui.brightnessSlider->setValue(m_startBrightness);
	m_startContrast = m_imageBuilder->getContrast();
	ui.contrastSlider->setValue(m_startContrast);
	m_startIntensity = m_imageBuilder->getIntensity();
	ui.intensitySlider->setValue(m_startIntensity);
}

BrightnessContrastDialog::~BrightnessContrastDialog()
{

}

int BrightnessContrastDialog::getBrightness() const
{
	return ui.brightnessSlider->value();
}

int BrightnessContrastDialog::getContrast() const
{
	return ui.contrastSlider->value();
}

int BrightnessContrastDialog::getIntensity() const
{
	return ui.contrastSlider->value();
}

void BrightnessContrastDialog::brightnessSlider_valueChanged(int value)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setBrightness(value);
}

void BrightnessContrastDialog::contrastSlider_valueChanged(int contrast)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setContrast(contrast);
}

void BrightnessContrastDialog::intensitySlider_valueChanged(int intensity)
{
	Q_ASSERT(m_imageBuilder);
	m_imageBuilder->setIntensity(intensity);
}

void BrightnessContrastDialog::reject()
{
	brightnessSlider_valueChanged(m_startBrightness);
	contrastSlider_valueChanged(m_startContrast);
	intensitySlider_valueChanged(m_startIntensity);
	QDialog::reject();
}

} // namespace ICEGUI_NS
