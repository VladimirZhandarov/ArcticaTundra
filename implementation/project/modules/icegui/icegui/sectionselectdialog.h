#ifndef SECTIONSELECTDIALOG_H
#define SECTIONSELECTDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_sectionselectdialog.h"

class QAbstractItemModel;

namespace ICEGUI_NS
{

/**
* @class SectionSelectDialog
* ���� �������������� ������ �������� � ������ �������
*/
class ICEGUI_EXPORT SectionSelectDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* �����������
	* @param parent ������������ ������
	* @param f ����� ����
	*/
	SectionSelectDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* ����������
	*/
	~SectionSelectDialog();

	/**
	* ���������� ������ ������ �������� � �������� � ����
	* @param model ������ ������ �������� � �������� � ����
	*/
	void setModel(QAbstractItemModel* model);

	const QModelIndex& getCurrentModelIndex() const;

protected slots:
	void onCurrentFilterChanged(const QModelIndex& currentModelIndex);
	void onCurrentDescriptionChanged(const QString& description);
	void filterTreeView_doubleClicked(const QModelIndex& index);

private:
	Ui::SectionSelectDialog ui;	///< Ui �����
};

} // namespace ICEGUI_NS

#endif // SECTIONSELECTDIALOG_H
