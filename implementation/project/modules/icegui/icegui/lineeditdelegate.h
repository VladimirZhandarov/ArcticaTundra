#ifndef LINEEDITDELEGATE_H
#define LINEEDITDELEGATE_H

#include <QtGui/QItemDelegate>

namespace ICEGUI_NS
{

class LineEditDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	enum ItemDataRole
	{
		ReadOnlyRole = Qt::UserRole + 1
	};

	/**
	* Конструктор
	* @param parent родительский объект
	* @param readonly =TRUE - LineEdit только для чтения
	*/
	LineEditDelegate(QObject* parent = 0, bool readonly = false);

	/**
	* Деструктор
	*/
	~LineEditDelegate();

	void setCustomReadOnly(bool flag);

	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

private:
	bool m_readonly;
	bool m_customReadOnly;
};

} // namespace ICEGUI_NS

#endif // LINEEDITDELEGATE_H
