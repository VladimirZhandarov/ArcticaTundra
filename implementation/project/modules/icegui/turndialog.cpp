//==============================================================================
// ��������:        ���������� ���� "������������ �������"
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
// ��� �����: turndialog.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "turndialog.h"

#include <iceimage/imagebuilder.h>

#include <QtGui/QCloseEvent>

namespace ICEGUI_NS
{

TurnDialog::TurnDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder, 
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_imageBuilder(imageBuilder)
{
	ui.setupUi(this);

	Q_ASSERT(!m_imageBuilder.isNull());
	m_startAngle = m_imageBuilder->getRotatedAngleDegree();
	ui.turnSlider->setValue(m_startAngle);

	m_startKeepSizeOnTurn = m_imageBuilder->isKeepSizeOnRotated();
	ui.keepSizeChB->setChecked(m_startKeepSizeOnTurn);
}

TurnDialog::~TurnDialog()
{

}

void TurnDialog::turnSlider_valueChanged(int value)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setRotatedAngleDegree(value, ui.keepSizeChB->isChecked());
}

void TurnDialog::keepSizeChB_stateChanged(int state)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setRotatedAngleDegree(ui.turnSlider->value(), state != Qt::Unchecked);
}

int TurnDialog::getSettedAngle() const
{
	return ui.turnSlider->value();
}

void TurnDialog::reject()
{
	turnSlider_valueChanged(m_startAngle);
	keepSizeChB_stateChanged(m_startKeepSizeOnTurn);
	QDialog::reject();
}

} // namespace ICEGUI_NS
