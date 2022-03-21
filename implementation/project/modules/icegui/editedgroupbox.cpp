#include "editedgroupbox.h"

#include <QtGui/QStyleOption>
#include <QtGui/QMouseEvent>
#include <QtGui/QLineEdit>

namespace ICEGUI_NS
{

EditedGroupBox::EditedGroupBox(QWidget* parent) : QGroupBox(parent)
{
	init();
}

EditedGroupBox::EditedGroupBox(const QString& title, QWidget* parent) :
	QGroupBox(title, parent)
{
	init();
}

EditedGroupBox::~EditedGroupBox()
{

}

void EditedGroupBox::onEditingFinished()
{
	Q_ASSERT(m_editor);
	setTitle(m_editor->text());
	setFocusProxy(0);
	setFocus();
	m_editor->hide();
	update();
}

void EditedGroupBox::init()
{
	m_editor = new QLineEdit(this);
	connect(m_editor, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
	m_editor->hide();
}

void EditedGroupBox::mousePressEvent(QMouseEvent* event)
{
	QRect rect = getTextRect();
	if(rect.contains(event->pos()))
	{
		if(!isCheckable() || (isCheckable() && isChecked()))
		{
			Q_ASSERT(m_editor);
			m_editor->setText(title());
			rect.adjust(0, 0, 0, 2);
			m_editor->setGeometry(rect);
			m_editor->show();
			setFocusProxy(m_editor);
			setFocus();
		}
	}
	else
	{
		QGroupBox::mousePressEvent(event);
	}
}

void EditedGroupBox::mouseReleaseEvent(QMouseEvent* event)
{
	QRect rect = getTextRect();
	if(rect.contains(event->pos()))
	{
		if(!isCheckable() || (isCheckable() && isChecked()))
		{
			Q_ASSERT(m_editor);
			m_editor->setText(title());
			rect.adjust(0, 0, 0, 2);
			m_editor->setGeometry(rect);
			m_editor->show();
			setFocusProxy(m_editor);
			setFocus();
		}
	}
	else
	{
		QGroupBox::mouseReleaseEvent(event);
	}
}

QRect EditedGroupBox::getTextRect() const
{
	QStyleOptionGroupBox option;
	initStyleOption(&option);
	QRect result = style()->subControlRect(QStyle::CC_GroupBox, &option,
		QStyle::SC_GroupBoxLabel, this);
	QPoint point = result.bottomRight();
	point.setX(width());
	result.setBottomRight(point);
	return result;
}

} // namespace ICEGUI_NS
