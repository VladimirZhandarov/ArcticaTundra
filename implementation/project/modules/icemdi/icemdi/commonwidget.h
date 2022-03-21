//==============================================================================
// Название:        ---
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
// Имя файла: commonwidget.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef COMMONWIDGET_H
#define COMMONWIDGET_H

#include "icemdidef.h"

#include <QtGui/QWidget>

#include "ui_commonwidget.h"

namespace ICEGUI_NS
{
	class ImageEditor;
}

namespace ICEMDI_NS
{

class CommonWidget: public QWidget
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	* @param f флаги окна
	*/
	CommonWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~CommonWidget();

	ICEQWT_NS::ImageEditor* getImageEditor() const;

signals:
	void errorProcessed(QString errorMsg);

private:
	Ui::CommonWidget ui;	///< Ui класс
};

} // namespace ICEMDI_NS

#endif // COMMONWIDGET_H
