#include "itemwidget.h"

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include <QtGui/QColorDialog>

#include "param.h"

namespace ICEFINDOBJECTPROC_NS
{

ItemWidget::ItemWidget(QWidget* parent) : QWidget(parent), m_index(0)
{
	ui.setupUi(this);
	setColor(Qt::yellow);
	setParamObject(QSharedPointer<Param>(new Param()));
	ui.maskViewWidget->setLabelText(tr("Заливка"));
}

ItemWidget::~ItemWidget()
{

}

void ItemWidget::init(int index, const QColor& lineColor, const QColor& maskColor)
{
	m_index = index;
	setColor(lineColor, true);
	setColor(maskColor);
	ui.maskGB->setTitle(getMaskName());
}

void ItemWidget::setCheckable(bool checkable)
{
	ui.maskGB->setCheckable(checkable);
}

//TODO refac
void ItemWidget::setTitle(const QString& name)
{
	ui.maskGB->setTitle(name);
}

void ItemWidget::clear()
{
	QSharedPointer<QObject> param(new Param());
	setParamObject(param);
}

void ItemWidget::setParamObject(const QSharedPointer<QObject>& param, bool /*fromDb*/)
{
	Param* item = qobject_cast<Param*>(param.data());
	Q_ASSERT(item);
	ui.maskGB->setChecked(item->isMaskUsed());
	ui.erodeFirstMaskChB->setChecked(item->isErodeFirst());
	ui.erodeMaskSB->setValue(item->getKErode());
	ui.dilateMaskSB->setValue(item->getKDilate());
	ui.minAreaChB->setChecked(item->isCheckArea());
	if(item->isCheckArea())
	{
		minAreaChB_stateChanged(Qt::Checked);
	}
	else
	{
		minAreaChB_stateChanged(Qt::Unchecked);	
	}
	ui.minAreaSB->setValue(item->getMinArea());
	ui.minPerimeterChB->setChecked(item->isCheckPerimeter());
	if(item->isCheckPerimeter())
	{
		minPerimeterChB_stateChanged(Qt::Checked);
	}
	else
	{
		minPerimeterChB_stateChanged(Qt::Unchecked);
	}
	ui.minPerimeterSB->setValue(item->getMinPerimeter());
	ui.minKFormChB->setChecked(item->isCheckKForm());
	if(item->isCheckKForm())
	{
		minKFormChB_stateChanged(Qt::Checked);
	}
	else
	{
		minKFormChB_stateChanged(Qt::Unchecked);
	}
	ui.minKFormDSB->setValue(item->getMinKForm());
}

QSharedPointer<QObject> ItemWidget::getParamObject() const
{
	QSharedPointer<Param> result(new Param());
	result->setMaskUsed((!ui.maskGB->isCheckable() || ui.maskGB->isChecked()) && ui.maskGB->isEnabled());
	result->setErodeFirst(ui.erodeFirstMaskChB->isChecked());
	result->setKErode(ui.erodeMaskSB->value());
	result->setKDilate(ui.dilateMaskSB->value());
	result->setCheckArea(ui.minAreaChB->isChecked());
	result->setMinArea(ui.minAreaSB->value());
	result->setCheckPerimeter(ui.minPerimeterChB->isChecked());
	result->setMinPerimeter(ui.minPerimeterSB->value());
	result->setCheckKForm(ui.minKFormChB->isChecked());
	result->setMinKForm(ui.minKFormDSB->value());
	return result;
}

void ItemWidget::setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	Q_ASSERT(maskView.get());
	ui.maskGB->setTitle(maskView->getName());
	ui.maskViewWidget->setMaskView(maskView);
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> ItemWidget::getMaskView() const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskView> result = ui.maskViewWidget->getMaskView();
	Q_ASSERT(result.get());
	result->setName(ui.maskGB->title());
	return result;
}

void ItemWidget::setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	ui.lineViewWidget->setLineView(lineView);
}

std::tr1::shared_ptr<ICEODB_NS::LineView> ItemWidget::getLineView() const
{
	return ui.lineViewWidget->getLineView();
}

QString ItemWidget::getMaskName() const
{
	return ui.maskGB->title();
}

void ItemWidget::minAreaChB_stateChanged(int state)
{
	ui.minAreaL->setEnabled(state != Qt::Unchecked);
	ui.minAreaSB->setEnabled(state != Qt::Unchecked);
}

void ItemWidget::minPerimeterChB_stateChanged(int state)
{
	ui.minPerimeterL->setEnabled(state != Qt::Unchecked);
	ui.minPerimeterSB->setEnabled(state != Qt::Unchecked);
}

void ItemWidget::minKFormChB_stateChanged(int state)
{
	bool flag = (state != Qt::Unchecked);
	ui.minKFormL->setEnabled(flag);
	ui.minKFormDSB->setEnabled(flag);

	ui.minPerimeterChB->setEnabled(!flag);
	ui.minPerimeterL->setEnabled(!flag);
	ui.minPerimeterSB->setEnabled(!flag);
}

void ItemWidget::setColor(const QColor& color, bool forLine)
{
	if(forLine)
	{
		ui.lineViewWidget->setColor(color);
	}
	else
	{
		ui.maskViewWidget->setColor(color);
	}
}

} // namespace ICEFINDOBJECTPROC_NS
