#include "filtersortproxymodel.h"

#include <icecore/filteritemmodel.h>

namespace ICEGUI_NS
{

FilterSortProxyModel::FilterSortProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
}

FilterSortProxyModel::~FilterSortProxyModel()
{

}

void FilterSortProxyModel::setFilterText(const QString& text)
{
	m_filterText = text;
	invalidateFilter();
}

void FilterSortProxyModel::setAcceptedFilterTypeList(const QStringList& dllNameList)
{
	m_dllNameList = dllNameList;
	invalidateFilter();
}

bool FilterSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	int leftType = ICECORE_NS::FilterItemModel::getType(left);
	int rightType = ICECORE_NS::FilterItemModel::getType(right);
	if(leftType == rightType)
	{
		return QSortFilterProxyModel::lessThan(left, right);
	}
	if(sortOrder() == Qt::AscendingOrder)
	{
		return (leftType < rightType);
	}
	else
	{
		return (leftType > rightType);
	}
}

bool FilterSortProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	QModelIndex index = sourceModel()->index(sourceRow, ICECORE_NS::FilterItemModel::NAME_COLNUM,
		sourceParent);
	if(!index.isValid())
	{
		return false;
	}
	if(ICECORE_NS::FilterItemModel::isFilterSection(index))
	{
		return true;
	}
	if(m_dllNameList.isEmpty() ||
		m_dllNameList.contains(ICECORE_NS::FilterItemModel::getFilterType(index)))
	{
		if(m_filterText.isEmpty())
		{
			return true;
		}
		return (index.data().toString().contains(m_filterText) ||
			ICECORE_NS::FilterItemModel::getFilterDescription(index).contains(m_filterText));
	}
	return false;
}

} // namespace ICEGUI_NS
