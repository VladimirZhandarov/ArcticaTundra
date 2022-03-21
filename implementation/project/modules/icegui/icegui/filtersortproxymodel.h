#ifndef FILTERSORTPROXYMODEL_H
#define FILTERSORTPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

#include <QtCore/QStringList>

namespace ICEGUI_NS
{

/**
* @class FilterSortProxyModel
* Модель с сортировкой дерева разделов и фильтров
*/
class FilterSortProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	*/
	FilterSortProxyModel(QObject* parent = 0);

	/**
	* Деструктор
	*/
	~FilterSortProxyModel();

	void setFilterText(const QString& text);
	void setAcceptedFilterTypeList(const QStringList& dllNameList);

protected:
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

private:
	QString m_filterText;
	QStringList m_dllNameList;
};

} // namespace ICEGUI_NS

#endif // FILTERSORTPROXYMODEL_H
