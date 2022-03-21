#ifndef DOUBLESPINBOXDELEGATE_H
#define DOUBLESPINBOXDELEGATE_H

#include "iceprocdef.h"

#include <QtGui/QItemDelegate>

namespace ICEPROC_NS
{

/**
* @class DoubleSpinBoxDelegate
* ƒегегат дл€ таблиц или деревьев с отображением QDoubleSpinBox
*/
class ICEPROC_EXPORT DoubleSpinBoxDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	/**
	*  онструктор
	* @param parent родительский объект
	* @param min минимальное значение
	* @param max максимальное значение
	*/
	DoubleSpinBoxDelegate(QObject* parent = 0, double min = -99999999.0, double max = 99999999.0);

	/**
	* ƒеструктор
	*/
	~DoubleSpinBoxDelegate();

	/**
	* —оздать редактор значени€
	* @param parent родительский объект дл€ радактора (QDoubleSpinBox)
	* @param option опции стил€ создаваемого редактора
	* @param modelIndex индекс с данными из таблицы или дерева
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& modelIndex) const;

	/**
	* ”становить данные в радактор (QDoubleSpinBox)
	* @param editor радектор (QDoubleSpinBox)
	* @param modelIndex индекс с данными из таблицы или дерева, которые
	*                   устанавливаютс€ в редактор
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& modelIndex) const;

	/**
	* ”становить данные модели из редактора (QDoubleSpinBox)
	* @param editor радектор (QDoubleSpinBox)
	* @param model модель таблицы или дерева
	* @param modelIndex индекс куда устанавливаютс€ данные
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& modelIndex) const;

	/**
	* »зменить геометрию редактора (QDoubleSpinBox)
	* @param editor радектор (QDoubleSpinBox)
	* @param option опции стил€ редактора
	* @param modelIndex индекс с данными из таблицы или дерева
	*/
	virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
		const QModelIndex& modelIndex) const;

protected:
	double m_min;	///< ћинимальное значение
	double m_max;	///< ћаксимальное значение
};

} // namespace ICEPROC_NS

#endif // DOUBLESPINBOXDELEGATE_H
