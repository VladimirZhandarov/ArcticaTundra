#ifndef FILTERMANAGEDIALOG_H
#define FILTERMANAGEDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_filtermanagedialog.h"

#include <boost/tr1/memory.hpp>

class QAbstractItemModel;

namespace ICEODB_NS
{
	class FilterParam;
}

namespace ICECORE_NS
{
	class FilterItemModel;
}

namespace ICEGUI_NS
{

/**
* @class FilterManageDialog
* Окно редактирования дерева разделов и фильтров в базе данных
*/
class ICEGUI_EXPORT FilterManageDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param dllNameList
	* @param parent родительский объект
	* @param f флаги окна
	*/
	FilterManageDialog(const QStringList& dllNameList, QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* Деструктор
	*/
	~FilterManageDialog();

	/**
	* Установить модель дерева разделов и фильтров в окно
	* @param filterModel модель дерева разделов и фильтров в окно
	*/
	void setModel(ICECORE_NS::FilterItemModel* filterModel);

	void setSelectEnabled(bool flag);

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getCurrentFilterParam() const;
	const QModelIndex& getCurrentFilterParamModelIndex() const;

protected slots:
	void openFilterPB_clicked();
	void findLE_textChanged(const QString& text);
	void showImagePB_clicked(bool checked);
	void onCurrentFilterChanged(const QModelIndex& currentModelIndex);
	void onCurrentDescriptionChanged(const QString& description);
	void filterTreeView_doubleClicked(const QModelIndex& index);

private:
	Ui::FilterManageDialog ui;	///< Ui класс

	QStringList m_dllNameList;
	ICECORE_NS::FilterItemModel* m_filterModel;
};

} // namespace ICEGUI_NS

#endif // FILTERMANAGEDIALOG_H
