#include "filteritemmodel.h"

#include <QtCore/QStringList>

#include <QtGui/QIcon>

#include <assert.h>

#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>

#include <iceodb/filtersection-odb.hxx>
#include <iceodb/filterparam-odb.hxx>
#include <iceodb/filtercount-odb.hxx>

namespace ICECORE_NS
{

FilterItemModel::FilterItemModel(QObject* parent) : QAbstractItemModel(parent)
{

}

FilterItemModel::~FilterItemModel()
{

}

void FilterItemModel::initDb(const QString& user, const QString& password, const QString& dbName,
	const QString& host, unsigned short int port) throw(ICECORE_NS::DataBaseException)
{
	try
	{
		m_db = std::auto_ptr<odb::database>(new odb::pgsql::database(user.toStdString(),
			password.toStdString(), dbName.toStdString(), host.toStdString(), port));
		checkAndCreateFilterRoot();
		fillFilterTypeMap();
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::setLoadedFilterType(const QStringList& filterTypeList)
	throw(ICECORE_NS::DataBaseException)
{
	QStringList existTypeList;
	QMap<QString, std::tr1::shared_ptr<ICEODB_NS::FilterType> >::const_iterator
		iterEnd = m_filterTypeMap.constEnd();
	for(QMap<QString, std::tr1::shared_ptr<ICEODB_NS::FilterType> >::const_iterator
		iter = m_filterTypeMap.constBegin(); iter != iterEnd; iter++)
	{
		existTypeList.append((*iter)->getDllName());
	}
	try
	{
		QStringList::const_iterator dllIterEnd = filterTypeList.constEnd();
		for(QStringList::const_iterator dllIter = filterTypeList.constBegin();
			dllIter != dllIterEnd; dllIter++)
		{
			if(!existTypeList.contains(*dllIter))
			{
				addFilterType(*dllIter);
			}
		}
		fillFilterTypeMap();
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::setIcon(const QString& filterType, const QIcon& icon)
{
	m_filterIconMap.insert(filterType, icon);
}

void FilterItemModel::addSection(const std::tr1::shared_ptr<ICEODB_NS::FilterSection>& section,
	const QModelIndex& parent)
{
	try
	{
		if(section.get())
		{
			QModelIndex usedParent = parent;
			ICEODB_NS::FilterSection* parentSection =
				ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
				usedParent.internalPointer());
			if(!usedParent.isValid() || !parentSection)
			{
				usedParent = index(0, 0, QModelIndex());
				parentSection = static_cast<ICEODB_NS::FilterSection*>(usedParent.internalPointer());
			}
			Q_ASSERT(parentSection);
			section->setParentSection(parentSection);
			beginInsertRows(usedParent, 0, 0);
			odb::transaction transaction(m_db->begin());
			m_db->persist<ICEODB_NS::FilterSection>(*section);

			QMap<QString, std::tr1::shared_ptr<ICEODB_NS::FilterType> >::const_iterator
				typeIterEnd = m_filterTypeMap.constEnd();
			for(QMap<QString, std::tr1::shared_ptr<ICEODB_NS::FilterType> >::const_iterator
				typeIter = m_filterTypeMap.constBegin(); typeIter != typeIterEnd; typeIter++)
			{
				ICEODB_NS::FilterCount filterCount(*typeIter, section);
				m_db->persist<ICEODB_NS::FilterCount>(filterCount);
			}
			m_db->reload<ICEODB_NS::FilterSection>(*section);
			parentSection->addChildSection(section.get());
			transaction.commit();
			endInsertRows();
		}
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::editSection(const std::tr1::shared_ptr<ICEODB_NS::FilterSection>& section)
{
	try
	{
		if(section.get())
		{
			odb::transaction transaction(m_db->begin());
			m_db->update<ICEODB_NS::FilterSection>(*section);
			transaction.commit();
		}
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::removeSection(const QModelIndex& modelIndex)
{
	try
	{
		if(!modelIndex.isValid())
		{
			return;
		}
		ICEODB_NS::FilterSection* section =
			ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
			modelIndex.internalPointer());
		if(!section)
		{
			return;
		}
		QModelIndex usedParent = parent(modelIndex);
		ICEODB_NS::FilterSection* parentSection =
			ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
			usedParent.internalPointer());
		if(!usedParent.isValid() || !parentSection)
		{
			usedParent = index(0, 0, QModelIndex());
			parentSection = static_cast<ICEODB_NS::FilterSection*>(usedParent.internalPointer());
		}
		Q_ASSERT(parentSection);
		beginRemoveRows(usedParent, modelIndex.row(), modelIndex.row());
		odb::transaction transaction(m_db->begin());
		eraseSection(section);
		parentSection->removeChildSection(section);
		transaction.commit();
		endRemoveRows();
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

QModelIndex FilterItemModel::addParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param,
	const QModelIndex& parent) throw(ICECORE_NS::DataBaseException)
{
	Q_ASSERT(param.get());
	try
	{
		QModelIndex usedParent = parent;
		ICEODB_NS::FilterSection* parentSection = 0;
		if(usedParent.isValid())
		{
			parentSection = static_cast<ICEODB_NS::FilterSection*>(usedParent.internalPointer());
			Q_ASSERT(parentSection);
			param->setParentSection(parentSection);
			beginInsertRows(usedParent, 0, 0);
		}
		odb::transaction transaction(m_db->begin());
		std::tr1::shared_ptr<ICEODB_NS::MaskView> backgroundMaskView = param->getBackgroundView();
		if(backgroundMaskView.get())
		{
			m_db->persist<ICEODB_NS::MaskView>(*backgroundMaskView);
		}
		m_db->persist<ICEODB_NS::FilterParam>(*param);

		for(int i=0; i<param->getMaskParamCount(); i++)
		{
			std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = param->getMaskParam(i);
			if(maskParam.get())
			{
				maskParam->setParentFilterParam(param.get());
				std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage = maskParam->getFilterImage();
				if(filterImage.get())
				{
					m_db->persist<ICEODB_NS::FilterImage>(*filterImage);
				}
				std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
				if(maskView.get())
				{
					m_db->persist<ICEODB_NS::MaskView>(*maskView);
				}
				std::tr1::shared_ptr<ICEODB_NS::LineView> lineView = maskParam->getLineView();
				if(lineView.get())
				{
					m_db->persist<ICEODB_NS::LineView>(*lineView);
				}
				m_db->persist<ICEODB_NS::MaskParam>(*maskParam);
			}
		}
		for(int i=0; i<param->getChildParamCount(); i++)
		{
			std::tr1::shared_ptr<ICEODB_NS::FilterParam> childFilterParam = param->getChildParam(i);
			if(childFilterParam.get())
			{
				addChildParam(childFilterParam);
			}
		}
		if(parentSection)
		{
			ICEODB_NS::FilterSection* parentSection2 = parentSection->getParentSection();
			m_db->reload<ICEODB_NS::FilterSection>(*parentSection);
			parentSection->setParentSection(parentSection2);
		}
		transaction.commit();
		if(usedParent.isValid())
		{
			endInsertRows();
			return index(parentSection->getChildSectionCount() + parentSection->getChildParamCount() - 1,
				0, usedParent);
		}
		else
		{
			return QModelIndex();
		}
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::addChildParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param) throw(ICECORE_NS::DataBaseException)
{
	Q_ASSERT(param.get());
	std::tr1::shared_ptr<ICEODB_NS::MaskView> backgroundMaskView = param->getBackgroundView();
	if(backgroundMaskView.get())
	{
		m_db->persist<ICEODB_NS::MaskView>(*backgroundMaskView);
	}
	m_db->persist<ICEODB_NS::FilterParam>(*param);

	for(int i=0; i<param->getMaskParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = param->getMaskParam(i);
		if(maskParam.get())
		{
			maskParam->setParentFilterParam(param.get());
			std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage = maskParam->getFilterImage();
			if(filterImage.get())
			{
				m_db->persist<ICEODB_NS::FilterImage>(*filterImage);
			}
			std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
			if(maskView.get())
			{
				m_db->persist<ICEODB_NS::MaskView>(*maskView);
			}
			std::tr1::shared_ptr<ICEODB_NS::LineView> lineView = maskParam->getLineView();
			if(lineView.get())
			{
				m_db->persist<ICEODB_NS::LineView>(*lineView);
			}
			m_db->persist<ICEODB_NS::MaskParam>(*maskParam);
		}
	}
	for(int i=0; i<param->getChildParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> childFilterParam = param->getChildParam(i);
		if(childFilterParam.get())
		{
			addChildParam(childFilterParam);
		}
	}
}

void FilterItemModel::editParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param)
{
	try
	{
		if(param.get())
		{
			odb::transaction transaction(m_db->begin());
			std::tr1::shared_ptr<ICEODB_NS::MaskView> backgroundMaskView = param->getBackgroundView();
			if(backgroundMaskView.get())
			{
				m_db->update<ICEODB_NS::MaskView>(*backgroundMaskView);
			}
			for(int i=0; i<param->getMaskParamCount(); i++)
			{
				std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = param->getMaskParam(i);
				if(maskParam.get())
				{
					std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
					if(maskView.get())
					{
						m_db->update<ICEODB_NS::MaskView>(*maskView);
					}
					m_db->update<ICEODB_NS::MaskParam>(*maskParam);
				}
			}
			m_db->update<ICEODB_NS::FilterParam>(*param);
			transaction.commit();
		}
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::removeParam(const QModelIndex& modelIndex)
{
	try
	{
		if(!modelIndex.isValid())
		{
			return;
		}
		ICEODB_NS::FilterParam* param =
			ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterParam>(
			modelIndex.internalPointer());
		if(!param)
		{
			return;
		}
		m_filterParamMap.remove(param->getId());
		QModelIndex usedParent = parent(modelIndex);
		ICEODB_NS::FilterSection* parentSection =
			ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
			usedParent.internalPointer());
		if(!usedParent.isValid() || !parentSection)
		{
			usedParent = index(0, 0, QModelIndex());
			parentSection = static_cast<ICEODB_NS::FilterSection*>(usedParent.internalPointer());
		}
		Q_ASSERT(parentSection);
		beginRemoveRows(usedParent, modelIndex.row(), modelIndex.row());
		odb::transaction transaction(m_db->begin());
		m_db->erase<ICEODB_NS::FilterParam>(param->getId());
		parentSection->removeChildParam(param);
		transaction.commit();
		endRemoveRows();
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

void FilterItemModel::setParentSection(const QModelIndex& modelIndex,
	const QModelIndex& parentModelIndex)
{
	if(!modelIndex.isValid() || !parentModelIndex.isValid())
	{
		return;
	}
	ICEODB_NS::FilterParam* param =
		ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterParam>(
		modelIndex.internalPointer());
	ICEODB_NS::FilterSection* section =
		ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
		parentModelIndex.internalPointer());
	if(section && param)
	{
		param->setParentSection(section);
	}
	//TODO vvv
}

QModelIndex FilterItemModel::getModelIndexOfDuplicateFilterName(const QString& name,
	unsigned int id, const QString& dllName, const QModelIndex& parent) const
{
	QModelIndex currentModelIndex = parent;
	if(!currentModelIndex.isValid())
	{
		currentModelIndex = index(0, 0, QModelIndex());
	}
	int rows = rowCount(currentModelIndex);
	for(int i=0; i<rows; i++)
	{
		QModelIndex childModelIndex = index(i, 0, currentModelIndex);
		if(childModelIndex.isValid())
		{
			if(childModelIndex.data().toString() == name &&
				childModelIndex.data(ICECORE_NS::FilterItemModel::FilterTypeRole).toString() == dllName &&
				childModelIndex.data(ICECORE_NS::FilterItemModel::FilterIdRole).toUInt() != id)
			{
				return childModelIndex;
			}
		}
	}
	return QModelIndex();
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterItemModel::getFilterParam(const QModelIndex& modelIndex) const
{
	return getFilterParam(getFilterId(modelIndex));
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterItemModel::getFilterParam(unsigned int id) const
{
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> result = m_filterParamMap.value(id);
	if(result.get())
	{
		return result;
	}
	try
	{
		odb::transaction transaction(m_db->begin());
		odb::query<ICEODB_NS::FilterParam> queryParam = odb::query<ICEODB_NS::FilterParam>(
			odb::query<ICEODB_NS::FilterParam>::id == id);
		odb::result<ICEODB_NS::FilterParam> resultParam(
			m_db->query<ICEODB_NS::FilterParam>(queryParam));
		if(!resultParam.empty())
		{
			odb::result<ICEODB_NS::FilterParam>::iterator iter = resultParam.begin();
			result = std::tr1::shared_ptr<ICEODB_NS::FilterParam>(iter.load());
			m_filterParamMap.insert(result->getId(), result);
		}
		transaction.commit();
		return result;
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
}

std::tr1::shared_ptr<ICEODB_NS::FilterType> FilterItemModel::getFilterType(const QString& key) const
{
	return m_filterTypeMap.value(key);
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterItemModel::getChildFilterParam(
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam, int index) const
{
	try
	{
		Q_ASSERT(filterParam.get());
		odb::transaction transaction(m_db->begin());
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> result = filterParam->getChildParam(index);
		transaction.commit();
		return result;
	}
	catch(const odb::exception& /*exc*/)
	{
		//TODO refac
		return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskParam> FilterItemModel::getMaskParam(
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam, int index) const
{
	try
	{
		Q_ASSERT(filterParam.get());
		odb::transaction transaction(m_db->begin());
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> result = filterParam->getMaskParam(index);
		transaction.commit();
		return result;
	}
	catch(const odb::exception& /*exc*/)
	{
		//TODO refac
		return std::tr1::shared_ptr<ICEODB_NS::MaskParam>();
	}
}

Qt::ItemFlags FilterItemModel::flags(const QModelIndex& modelIndex) const
{
	if(!modelIndex.isValid())
	{
		return 0;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int FilterItemModel::columnCount(const QModelIndex& /*parent*/) const
{
	return NUM_COLS;
}

QVariant FilterItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if(orientation == Qt::Vertical)
	{
		return QVariant();
	}
	else
	{
		if(section == NAME_COLNUM)
		{
			return tr("Название");
		}
	}
	return QVariant();
}

QVariant FilterItemModel::data(const QModelIndex& modelIndex, int role) const
{
	if(!modelIndex.isValid())
	{
		return QVariant();
	}
	ICEODB_NS::FilterSection* itemSection = castFilterToType<ICEODB_NS::FilterSection>(
		modelIndex.internalPointer());
	if(itemSection)
	{
		if(role == Qt::DisplayRole)
		{
			return itemSection->getName();
		}
		else if(role == Qt::DecorationRole)
		{
			return QIcon(":/dvg/icecore/images/filtersection.png");
		}
		else if(role == TypeRole)
		{
			return SECTION_TYPE;
		}
		else if(role == FilterTypeRole)
		{
			return "";
		}
		else if(role == FilterIdRole)
		{
			return itemSection->getId();
		}
	}
	ICEODB_NS::FilterParam* itemParam = castFilterToType<ICEODB_NS::FilterParam>(
		modelIndex.internalPointer());
	if(itemParam)
	{
		if(role == Qt::DisplayRole)
		{
			return itemParam->getName();
		}
		else if(role == Qt::DecorationRole)
		{
			return m_filterIconMap.value(itemParam->getFilterType()->getDllName(),
				QIcon(":/dvg/icecore/images/filterparam.png"));
		}
		else if(role == TypeRole)
		{
			return PARAM_TYPE;
		}
		else if(role == FilterTypeRole)
		{
			return itemParam->getFilterType()->getDllName();
		}
		else if(role == FilterIdRole)
		{
			return itemParam->getId();
		}
		else if(role == FilterDescriptionRole)
		{
			return itemParam->getDescription();
		}
	}
	return QVariant();
}

QModelIndex FilterItemModel::index(int row, int column, const QModelIndex& parent) const
	throw(ICECORE_NS::DataBaseException)
{
	try
	{
		if(!hasIndex(row, column, parent))
		{
			return QModelIndex();
		}
		if(!parent.isValid())
		{
			return createIndex(row, column, m_rootSection.get());
		}
		ICEODB_NS::FilterSection* parentItem = castFilterToType<ICEODB_NS::FilterSection>(
			parent.internalPointer());
		if(parentItem)
		{
			int sectionCount = parentItem->getChildSectionCount();
			if(row < sectionCount)
			{
				odb::transaction transaction(m_db->begin());
				ICEODB_NS::FilterSection* childItem = parentItem->getChildSection(row);
				transaction.commit();
				if(childItem)
				{
					return createIndex(row, column, childItem);
				}
				else
				{
					return QModelIndex();
				}
			}
			if(row - sectionCount < parentItem->getChildParamCount())
			{
				odb::transaction transaction(m_db->begin());
				ICEODB_NS::FilterParam* childItem = parentItem->getChildParam(row - sectionCount);
				transaction.commit();
				if(childItem)
				{
					m_filterParamMap.insert(childItem->getId(), childItem->shared_from_this());
					return createIndex(row, column, childItem);
				}
				else
				{
					return QModelIndex();
				}
			}
		}
	}
	catch (const odb::exception& exc)
	{
		throw ICECORE_NS::DataBaseException(exc.what());
	}
	return QModelIndex();
}

QModelIndex FilterItemModel::parent(const QModelIndex& child) const
{
	if(!child.isValid())
	{
		return QModelIndex();
	}
	ICEODB_NS::FilterSection* childSectionItem = castFilterToType<ICEODB_NS::FilterSection>(
		child.internalPointer());
	if(childSectionItem)
	{
		if(childSectionItem == m_rootSection.get())
		{
			return QModelIndex();
		}
		odb::transaction transaction(m_db->begin());
		ICEODB_NS::FilterSection* parentItem = childSectionItem->getParentSection();
		transaction.commit();
		if(parentItem == 0)
		{
			return QModelIndex();
		}
		else
		{
			return createIndex(parentItem->getIndex(childSectionItem), 0, parentItem);
		}
	}
	ICEODB_NS::FilterParam* childParamItem = castFilterToType<ICEODB_NS::FilterParam>(
		child.internalPointer());
	if(childParamItem)
	{
		odb::transaction transaction(m_db->begin());
		ICEODB_NS::FilterSection* parentItem = childParamItem->getParentSection();
		transaction.commit();
		if(parentItem == 0)
		{
			return QModelIndex();
		}
		else
		{
			return createIndex(parentItem->getIndex(childParamItem), 0, parentItem);
		}
	}
	return QModelIndex();
}

int FilterItemModel::rowCount(const QModelIndex& parent) const
{
	if(!parent.isValid())
	{
		return 1;
	}
	ICEODB_NS::FilterSection* parentItem = castFilterToType<ICEODB_NS::FilterSection>(
		parent.internalPointer());
	if(parentItem)
	{
		return parentItem->getChildSectionCount() + parentItem->getChildParamCount();
	}
	else
	{
		return 0;
	}
}

bool FilterItemModel::isFilterSection(const QModelIndex& modelIndex)
{
	return (getType(modelIndex) == SECTION_TYPE);
}

bool FilterItemModel::isFilterParam(const QModelIndex& modelIndex)
{
	return (getType(modelIndex) == PARAM_TYPE);
}

int FilterItemModel::getType(const QModelIndex& modelIndex)
{
	return modelIndex.data(TypeRole).toInt();
}

QString FilterItemModel::getFilterType(const QModelIndex& modelIndex)
{
	if(modelIndex.isValid())
	{
		return modelIndex.data(FilterTypeRole).toString();
	}
	else
	{
		return QString();
	}
}

unsigned int FilterItemModel::getFilterId(const QModelIndex& modelIndex)
{
	if(modelIndex.isValid())
	{
		return modelIndex.data(FilterIdRole).toUInt();
	}
	else
	{
		return 0;
	}
}

QString FilterItemModel::getFilterDescription(const QModelIndex& modelIndex)
{
	if(modelIndex.isValid())
	{
		return modelIndex.data(FilterDescriptionRole).toString();
	}
	else
	{
		return QString();
	}
}

void FilterItemModel::eraseSection(ICEODB_NS::FilterSection* section)
{
	if(!section)
	{
		return;
	}
	for(int i=0; i<section->getFilterCountCount(); i++)
	{
		ICEODB_NS::FilterCount* filterCount = section->getFilterCount(i);
		if(filterCount)
		{
			m_db->erase<ICEODB_NS::FilterCount>(filterCount->getId());
		}
	}
	for(int i=0; i<section->getChildParamCount(); i++)
	{
		ICEODB_NS::FilterParam* filterParam = section->getChildParam(i);
		if(filterParam)
		{
			m_db->erase<ICEODB_NS::FilterParam>(filterParam->getId());
		}
	}
	for(int i=0; i<section->getChildSectionCount(); i++)
	{
		eraseSection(section->getChildSection(i));
	}
	m_db->erase<ICEODB_NS::FilterSection>(section->getId());
}

std::tr1::shared_ptr<ICEODB_NS::FilterSection> FilterItemModel::getRootFilterSection()
{
	std::tr1::shared_ptr<ICEODB_NS::FilterSection> result;
	odb::transaction transaction(m_db->begin());
	odb::query<ICEODB_NS::FilterSection> querySection = odb::query<ICEODB_NS::FilterSection>("level = 0");
	odb::result<ICEODB_NS::FilterSection> resultSection(m_db->query<ICEODB_NS::FilterSection>(querySection));
	if(!resultSection.empty())
	{
		odb::result<ICEODB_NS::FilterSection>::iterator iter = resultSection.begin();
		result = std::tr1::shared_ptr<ICEODB_NS::FilterSection>(iter.load());
	}
	transaction.commit();
	return result;
}

std::tr1::shared_ptr<ICEODB_NS::FilterSection> FilterItemModel::createRootFilterSection()
{
	std::tr1::shared_ptr<ICEODB_NS::FilterSection> result(new ICEODB_NS::FilterSection("root", "root"));
	odb::transaction transaction(m_db->begin());
	m_db->persist<ICEODB_NS::FilterSection>(*result);
	transaction.commit();
	return result;
}

void FilterItemModel::checkAndCreateFilterRoot()
{
	m_rootSection = getRootFilterSection();
	if(m_rootSection.get() == 0)
	{
		m_rootSection = createRootFilterSection();
	}
}

void FilterItemModel::addFilterType(const QString& dllName)
{
	std::tr1::shared_ptr<ICEODB_NS::FilterType> result(new ICEODB_NS::FilterType(dllName));
	odb::transaction transaction(m_db->begin());
	m_db->persist<ICEODB_NS::FilterType>(*result);
	transaction.commit();
}

void FilterItemModel::fillFilterTypeMap()
{
	m_filterTypeMap.clear();
	std::tr1::shared_ptr<ICEODB_NS::FilterType> result;
	odb::transaction transaction(m_db->begin());
	odb::query<ICEODB_NS::FilterType> queryType = odb::query<ICEODB_NS::FilterType>();
	odb::result<ICEODB_NS::FilterType> resultType(m_db->query<ICEODB_NS::FilterType>(queryType));
	for(odb::result<ICEODB_NS::FilterType>::iterator iter(resultType.begin()); iter != resultType.end();
		iter++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterType> item(iter.load());
		m_filterTypeMap.insert(item->getDllName(), item);
	}
	transaction.commit();
}

} // namespace ICECORE_NS
