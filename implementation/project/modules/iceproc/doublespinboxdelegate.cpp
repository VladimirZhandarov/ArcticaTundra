#include "doublespinboxdelegate.h"

#include <QtGui/QDoubleSpinBox>
#include <QtGui/QStyleOptionViewItem>

namespace ICEPROC_NS
{

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject* parent, double min, double max) :
	QItemDelegate(parent), m_min(min), m_max(max)
{

}

DoubleSpinBoxDelegate::~DoubleSpinBoxDelegate()
{

}

QWidget* DoubleSpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
	const QModelIndex& /*modelIndex*/) const
{
	QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
	editor->setDecimals(6);
	editor->setValue(0.0);
	editor->setMinimum(m_min);
	editor->setMaximum(m_max);
	return editor;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& modelIndex) const
{
	float value = modelIndex.model()->data(modelIndex, Qt::EditRole).toFloat();
	QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
	if(spinBox)
	{
		spinBox->setValue(value);
	}
}

void DoubleSpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
	const QModelIndex& modelIndex) const
{
	QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
	if(spinBox)
	{
		model->setData(modelIndex, spinBox->value(), Qt::EditRole);
	}
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
	const QModelIndex& /*modelIndex*/) const
{
	editor->setGeometry(option.rect);
}

} // namespace ICEPROC_NS
