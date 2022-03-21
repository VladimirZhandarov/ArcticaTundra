//==============================================================================
// ��������:        ---
// ��������� �����: ��� "���" DeveloperGroup (�. �����-���������)
//==============================================================================
//
// �����:     $Author: shchav $
// ������:    2011.11.08
// �������:   $Date: 2013-04-19 18:46:52 +0400 (Пт, 19 апр 2013) $
// ������:    icegui
// ���������: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// ��� �����: commonwidget.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
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
