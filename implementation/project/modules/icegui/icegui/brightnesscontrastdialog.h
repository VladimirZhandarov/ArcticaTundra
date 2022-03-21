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
// Имя файла: brightnesscontrastdialog.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef BRIGHTNESSCONTRASTDIALOG_H
#define BRIGHTNESSCONTRASTDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_brightnesscontrastdialog.h"

class QCloseEvent;

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT BrightnessContrastDialog : public QDialog
{
	Q_OBJECT
public:
	BrightnessContrastDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~BrightnessContrastDialog();

	int getBrightness() const;
	int getContrast() const;
	int getIntensity() const;

public slots:
	virtual void reject();

private slots:
	void brightnessSlider_valueChanged(int value);
	void contrastSlider_valueChanged(int value);
	void intensitySlider_valueChanged(int value);

private:
	Ui::BrightnessContrastDialog ui;	///< Ui класс

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
	int m_startContrast;	///< Начальная контрастность
	int m_startBrightness;	///< Нвчальная яркость
	int m_startIntensity;	///< Начальная интенсивность
};

} // namespace ICEGUI_NS

#endif // BRIGHTNESSCONTRASTDIALOG_H
