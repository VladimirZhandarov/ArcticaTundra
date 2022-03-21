#include "lineeditdelegate.h"

#include <QtGui/QLineEdit>

#include <QtGui/QStyleOptionViewItem>

namespace ICEGUI_NS
{

LineEditDelegate::LineEditDelegate(QObject* parent, bool readonly) : QItemDelegate(parent),
	m_readonly(readonly), m_customReadOnly(false)
{

}

LineEditDelegate::~LineEditDelegate()
{

}

void LineEditDelegate::setCustomReadOnly(bool flag)
{
	m_customReadOnly = flag;
}

QWidget* LineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/,
	const QModelIndex& /*index*/) const
{
	QLineEdit *editor = new QLineEdit(parent);
	editor->setReadOnly(m_readonly);
	editor->setText("");
	return editor;
}

void LineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();
	QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
	if(m_customReadOnly)
	{
		bool cRO = index.model()->data(index, ReadOnlyRole).toBool();
		lineEdit->setReadOnly(cRO);
	}
	lineEdit->setText(value);
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
	model->setData(index, lineEdit->text(), Qt::EditRole);
}

void LineEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
	const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}

} // namespace ICEGUI_NS
