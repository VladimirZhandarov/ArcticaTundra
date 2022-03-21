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
// Имя файла: commonwidget.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "commonwidget.h"

#include <iceqwt/imageeditor.h>

namespace ICEMDI_NS
{

CommonWidget::CommonWidget(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	ui.setupUi(this);
	connect(ui.frame, SIGNAL(errorProcessed(QString)), this, SIGNAL(errorProcessed(QString)));
}

CommonWidget::~CommonWidget()
{

}

ICEQWT_NS::ImageEditor* CommonWidget::getImageEditor() const
{
	return ui.frame;
}

} // namespace ICEMDI_NS
