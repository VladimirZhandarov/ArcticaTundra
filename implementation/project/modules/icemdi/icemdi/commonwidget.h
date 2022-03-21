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
// ��� �����: commonwidget.h
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
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
	* �����������
	* @param parent ������������ ������
	* @param f ����� ����
	*/
	CommonWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~CommonWidget();

	ICEQWT_NS::ImageEditor* getImageEditor() const;

signals:
	void errorProcessed(QString errorMsg);

private:
	Ui::CommonWidget ui;	///< Ui �����
};

} // namespace ICEMDI_NS

#endif // COMMONWIDGET_H
