#ifndef DOUBLESPINBOXDELEGATE_H
#define DOUBLESPINBOXDELEGATE_H

#include "iceprocdef.h"

#include <QtGui/QItemDelegate>

namespace ICEPROC_NS
{

/**
* @class DoubleSpinBoxDelegate
* ������� ��� ������ ��� �������� � ������������ QDoubleSpinBox
*/
class ICEPROC_EXPORT DoubleSpinBoxDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	/**
	* �����������
	* @param parent ������������ ������
	* @param min ����������� ��������
	* @param max ������������ ��������
	*/
	DoubleSpinBoxDelegate(QObject* parent = 0, double min = -99999999.0, double max = 99999999.0);

	/**
	* ����������
	*/
	~DoubleSpinBoxDelegate();

	/**
	* ������� �������� ��������
	* @param parent ������������ ������ ��� ��������� (QDoubleSpinBox)
	* @param option ����� ����� ������������ ���������
	* @param modelIndex ������ � ������� �� ������� ��� ������
	*/
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& modelIndex) const;

	/**
	* ���������� ������ � �������� (QDoubleSpinBox)
	* @param editor �������� (QDoubleSpinBox)
	* @param modelIndex ������ � ������� �� ������� ��� ������, �������
	*                   ��������������� � ��������
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& modelIndex) const;

	/**
	* ���������� ������ ������ �� ��������� (QDoubleSpinBox)
	* @param editor �������� (QDoubleSpinBox)
	* @param model ������ ������� ��� ������
	* @param modelIndex ������ ���� ��������������� ������
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& modelIndex) const;

	/**
	* �������� ��������� ��������� (QDoubleSpinBox)
	* @param editor �������� (QDoubleSpinBox)
	* @param option ����� ����� ���������
	* @param modelIndex ������ � ������� �� ������� ��� ������
	*/
	virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
		const QModelIndex& modelIndex) const;

protected:
	double m_min;	///< ����������� ��������
	double m_max;	///< ������������ ��������
};

} // namespace ICEPROC_NS

#endif // DOUBLESPINBOXDELEGATE_H
