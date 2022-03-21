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
* Окно редактирования дерева разделов и выбора раздела
*/
class ICEGUI_EXPORT SectionSelectDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	* @param f флаги окна
	*/
	SectionSelectDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* Деструктор
	*/
	~SectionSelectDialog();

	/**
	* Установить модель дерева разделов и фильтров в окно
	* @param model модель дерева разделов и фильтров в окно
	*/
	void setModel(QAbstractItemModel* model);

	const QModelIndex& getCurrentModelIndex() const;

protected slots:
	void onCurrentFilterChanged(const QModelIndex& currentModelIndex);
	void onCurrentDescriptionChanged(const QString& description);
	void filterTreeView_doubleClicked(const QModelIndex& index);

private:
	Ui::SectionSelectDialog ui;	///< Ui класс
};

} // namespace ICEGUI_NS

#endif // SECTIONSELECTDIALOG_H
