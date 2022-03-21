#include "lineitemmodel.h"

#include <QtCore/qmath.h>

#include <iceodb/odb/maskview.hxx>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning(disable: 4985)
#endif
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include <QtGui/QApplication>
#include <QtGui/QPalette>

namespace ICEIMAGE_NS
{

LineItemInfo::LineItemInfo(int index, const QPoint& point1, const QPoint& point2,
	double length) : m_index(index), m_point1(point1), m_point2(point2), m_length(length),
	m_checked(true), m_enabled(true)
{

}

LineItemInfo::~LineItemInfo()
{

}

int LineItemInfo::getIndex() const
{
	return m_index;
}

const QPoint& LineItemInfo::getPoint1() const
{
	return m_point1;
}

const QPoint& LineItemInfo::getPoint2() const
{
	return m_point2;
}

double LineItemInfo::getLength() const
{
	return m_length;
}

void LineItemInfo::setChecked(bool value)
{
	m_checked = value;
}

bool LineItemInfo::isChecked() const
{
	return m_checked;
}

void LineItemInfo::setEnabled(bool value)
{
	m_enabled = value;
}

bool LineItemInfo::isEnabled() const
{
	return m_enabled;
}

void LineItemInfo::setComment(const QString& comment)
{
	m_comment = comment;
}

const QString& LineItemInfo::getComment() const
{
	return m_comment;
}

LineItemModel::LineItemModel(int index, const std::vector<cv::Vec4i>& lines,
	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView, bool visible, QObject* parent) :
	QAbstractItemModel(parent), m_index(index), m_maskView(maskView), m_visible(visible)
{
	m_lineLengthSum = 0.0;

	int itemIndex = 1;
	for(int i=0; i<lines.size(); i++)
	{
		cv::Vec4i line = lines[i];
		double length = qSqrt((line[0] - line[2]) * (line[0] - line[2]) +
			(line[1] - line[3]) * (line[1] - line[3]));
		m_itemInfoList.append(new LineItemInfo(itemIndex, QPoint(line[0], line[1]),
			QPoint(line[2], line[3]), length));
		m_matLines.push_back(line);
		itemIndex++;
	}
}

LineItemModel::~LineItemModel()
{

}

QString LineItemModel::getName() const
{
	if(m_maskView.get())
	{
		return m_maskView->getName();
	}
	else
	{
		return tr("Линии ") + QString::number(m_index);
	}
}

const std::vector<cv::Vec4i>& LineItemModel::getMatLines() const
{
	return m_matLines;
}

double LineItemModel::getLineLengthSum(const QModelIndexList& rows) const
{
	if(rows.isEmpty())
	{
		return m_lineLengthSum;
	}
	else
	{
		double result = 0.0;
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			if(!(*iter).isValid())
			{
				continue;
			}
			LineItemInfo* itemInfo = static_cast<LineItemInfo*>((*iter).internalPointer());
			if(itemInfo)
			{
				result += itemInfo->getLength();
			}
		}
		return result;
	}
}

bool LineItemModel::isVisibleItem(int index) const
{
	if(index >=0 && index < m_itemInfoList.size())
	{
		if(m_itemInfoList[index])
		{
			return m_itemInfoList[index]->isEnabled();
		}
	}
	return false;
}

bool LineItemModel::isCheckedItem(int index) const
{
	if(index >=0 && index < m_itemInfoList.size())
	{
		if(m_itemInfoList[index])
		{
			return m_itemInfoList[index]->isChecked();
		}
	}
	return false;
}

void LineItemModel::setCheckedAll()
{
	for(int i=0; i<m_itemInfoList.count(); i++)
	{
		if(m_itemInfoList[i])
		{
			m_itemInfoList[i]->setChecked(true);
		}
	}
	emit dataChanged(QModelIndex(), QModelIndex());
}

void LineItemModel::setUncheckedAll()
{
	for(int i=0; i<m_itemInfoList.count(); i++)
	{
		if(m_itemInfoList[i])
		{
			m_itemInfoList[i]->setChecked(false);
		}
	}
	emit dataChanged(QModelIndex(), QModelIndex());
}

void LineItemModel::setEnabledAll()
{
	for(int i=0; i<m_itemInfoList.count(); i++)
	{
		if(m_itemInfoList[i])
		{
			m_itemInfoList[i]->setEnabled(true);
		}
	}
	emit dataChanged(QModelIndex(), QModelIndex());
}

void LineItemModel::setVisible(bool visible)
{
	m_visible = visible;
}

bool LineItemModel::isVisible() const
{
	return m_visible;
}

QColor LineItemModel::getColor() const
{
	Q_ASSERT(m_maskView);
	return m_maskView->getColor();
}

Qt::ItemFlags LineItemModel::flags(const QModelIndex& modelIndex) const
{
	if(!modelIndex.isValid())
	{
		return 0;
	}
	Qt::ItemFlags result = Qt::ItemIsSelectable  | Qt::ItemIsEnabled;
	if(modelIndex.column() == COMMENT_COL)
	{
		result = result | Qt::ItemIsEditable;
	}
	return result;
}

int LineItemModel::columnCount(const QModelIndex& /*parent*/) const
{
	return NUM_COLS;
}

QVariant LineItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
		if(section == NUMBER_COL)
		{
			return tr("№ линии");
		}
		else if(section == POINT1_COL)
		{
			return tr("Точка 1");
		}
		else if(section == POINT1_COL)
		{
			return tr("Точка 2");
		}
		else if(section == LEN_COL)
		{
			return tr("Длина");
		}
		else if(section == COMMENT_COL)
		{
			return tr("Комментарии");
		}
	}
	return QVariant();
}

QVariant LineItemModel::data(const QModelIndex& modelIndex, int role) const
{
	if(!modelIndex.isValid())
	{
		return QVariant();
	}
	LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		if(role == Qt::BackgroundRole)
		{
			LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
			if(itemInfo && !itemInfo->isEnabled())
			{
				return QVariant(QApplication::palette().color(QPalette::Disabled, QPalette::Window));
			}
		}
		else if(role == Qt::ForegroundRole)
		{
			LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
			if(itemInfo && !itemInfo->isEnabled())
			{
				return QVariant(QApplication::palette().color(QPalette::Disabled, QPalette::WindowText));
			}
		}
		else if(role == Qt::CheckStateRole && modelIndex.column() == NUMBER_COL)
		{
			return static_cast<int>(itemInfo->isChecked() ? Qt::Checked : Qt::Unchecked);
		}
		else if(role == Qt::DisplayRole || role == DataRole)
		{
			if(modelIndex.column() == NUMBER_COL)
			{
				return itemInfo->getIndex();
			}
			else if(modelIndex.column() == POINT1_COL)
			{
				return itemInfo->getPoint1();
			}
			else if(modelIndex.column() == POINT2_COL)
			{
				return itemInfo->getPoint1();
			}
			if(modelIndex.column() == LEN_COL)
			{
				return itemInfo->getLength();
			}
			else if(modelIndex.column() == COMMENT_COL)
			{
				return itemInfo->getComment();
			}
		}
	}
	return QVariant();
}

bool LineItemModel::setData(const QModelIndex& modelIndex, const QVariant& value, int role)
{
	if(!modelIndex.isValid())
	{
		return false;
	}
	if(modelIndex.column() == NUMBER_COL && role == Qt::CheckStateRole)
	{
		LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
		if(itemInfo)
		{
			itemInfo->setChecked(value.toBool());
			emit dataChanged(modelIndex, modelIndex);
			return true;
		}
	}
	if(modelIndex.column() == COMMENT_COL && role == Qt::EditRole)
	{
		LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
		if(itemInfo)
		{
			itemInfo->setComment(value.toString());
			emit dataChanged(modelIndex, modelIndex);
			return true;
		}
	}
	return false;
}

QModelIndex LineItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if(!hasIndex(row, column, parent))
	{
		return QModelIndex();
	}
	if(!parent.isValid() && row >= 0 && row < m_itemInfoList.size())
	{
		return createIndex(row, column, m_itemInfoList[row]);
	}
	return QModelIndex();
}

QModelIndex LineItemModel::parent(const QModelIndex& /*child*/) const
{
	return QModelIndex();
}

int LineItemModel::rowCount(const QModelIndex& parent) const
{
	if(parent.column() > 0)
	{
		return 0;
	}
	if(!parent.isValid())
	{
		return m_itemInfoList.size();
	}
	else
	{
		return 0;
	}
}

void LineItemModel::setEnabled(const QModelIndex& modelIndex)
{
	if(!modelIndex.isValid())
	{
		return;
	}
	LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		itemInfo->setEnabled(true);
		emit dataChanged(modelIndex, modelIndex);
	}
}

void LineItemModel::setDisabled(const QModelIndex& modelIndex)
{
	if(!modelIndex.isValid())
	{
		return;
	}
	LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		itemInfo->setEnabled(false);
		emit dataChanged(modelIndex, modelIndex);
	}
}

void LineItemModel::setChecked(const QModelIndex& modelIndex, bool value)
{
	if(!modelIndex.isValid())
	{
		return;
	}
	LineItemInfo* itemInfo = static_cast<LineItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		itemInfo->setChecked(value);
		emit dataChanged(modelIndex, modelIndex);
	}
}

void LineItemModel::removeItems(const QModelIndexList& rows)
{
	QList<LineItemInfo*> itemForRemove;
	QModelIndexList::const_iterator iterEnd = rows.constEnd();
	for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
		iter++)
	{
		if(!(*iter).isValid())
		{
			continue;
		}
		LineItemInfo* itemInfo = static_cast<LineItemInfo*>((*iter).internalPointer());
		int index = m_itemInfoList.indexOf(itemInfo);
		if(itemInfo && index >= 0 && index < m_itemInfoList.size())
		{
			beginRemoveRows(parent(*iter), index, index);
			m_lineLengthSum -= itemInfo->getLength();
			m_itemInfoList.removeOne(itemInfo);
			m_matLines.erase(m_matLines.begin() + index);
			delete itemInfo;
			endRemoveRows();
		}
	}
	emit dataChanged(QModelIndex(), QModelIndex());
}

} // namespace ICEIMAGE_NS
