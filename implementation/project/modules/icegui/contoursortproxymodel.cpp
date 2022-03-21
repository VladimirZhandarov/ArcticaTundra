#include "contoursortproxymodel.h"

#include <iceimage/contouritemmodel.h>

namespace ICEGUI_NS
{

ContourSortProxyModel::ContourSortProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
}

ContourSortProxyModel::~ContourSortProxyModel()
{

}

bool ContourSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	int leftType = left.data(ICEIMAGE_NS::ContourItemModel::DataRole).toInt();
	int rightType = right.data(ICEIMAGE_NS::ContourItemModel::DataRole).toInt();
	if(leftType == rightType)
	{
		return QSortFilterProxyModel::lessThan(left, right);
	}
	return (leftType < rightType);
}

} // namespace ICEGUI_NS
