//==============================================================================
// Название:        Диалоговое окно "Произвольный поворот"
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
// Имя файла: turndialog.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef TURNDIALOG_H
#define TURNDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_turndialog.h"

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT TurnDialog : public QDialog
{
	Q_OBJECT
public:
	TurnDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder, 
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~TurnDialog();

	int getSettedAngle() const;

public slots:
	virtual void reject();

private slots:
	void turnSlider_valueChanged(int value);
	void keepSizeChB_stateChanged(int state);

private:
	Ui::TurnDialog ui;	///< Ui класс

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
	int m_startAngle; ///< Начальный угол
	bool m_startKeepSizeOnTurn;
};

} // namespace ICEGUI_NS

#endif // TURNDIALOG_H
