#ifndef CONTOURSORTPROXYMODEL_H
#define CONTOURSORTPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace ICEGUI_NS
{

class ContourSortProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	*/
	ContourSortProxyModel(QObject* parent = 0);

	/**
	* Деструктор
	*/
	~ContourSortProxyModel();

protected:
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

} // namespace ICEGUI_NS

#endif // CONTOURSORTPROXYMODEL_H
