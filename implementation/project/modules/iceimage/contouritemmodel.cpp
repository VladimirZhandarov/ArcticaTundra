#include "contouritemmodel.h"

#include "global.h"

#include <iceodb/odb/maskview.hxx>
#include <iceodb/odb/lineview.hxx>

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

ContourItemInfo::ContourItemInfo(int index, double arcLength, double contourArea,
	double objectArea, const QString& comment) : m_index(index), m_arcLength(arcLength),
	m_contourArea(contourArea), m_objectArea(objectArea), m_comment(comment),
	m_checked(true), m_enabled(true)
{

}


ContourItemInfo::~ContourItemInfo()
{

}

int ContourItemInfo::getIndex() const
{
	return m_index;
}

double ContourItemInfo::getArcLength() const
{
	return m_arcLength;
}

double ContourItemInfo::getContourArea() const
{
	return m_contourArea;
}

double ContourItemInfo::getObjectArea() const
{
	return m_objectArea;
}

void ContourItemInfo::setChecked(bool value)
{
	m_checked = value;
}

bool ContourItemInfo::isChecked() const
{
	return m_checked;
}

void ContourItemInfo::setEnabled(bool value)
{
	m_enabled = value;
}

bool ContourItemInfo::isEnabled() const
{
	return m_enabled;
}

void ContourItemInfo::setComment(const QString& comment)
{
	m_comment = comment;
}

const QString& ContourItemInfo::getComment() const
{
	return m_comment;
}

ContourItemModel::ContourItemModel(int index, int matImageCols, int matImageRows,
	QObject* parent) : QAbstractItemModel(parent), m_index(index),
	m_matImageCols(matImageCols), m_matImageRows(matImageRows), m_minSizeValue(0),
	m_minPerimeterValue(0), m_minKFormValue(0.0), m_visible(false), m_divK(1.0),
	m_unit(0), m_outsideSelectedIndex(-1), m_objectCount(0), m_arcLengthSum(0.0),
	m_contourAreaSum(0.0), m_objectAreaSum(0.0), m_addedImagePointCount(0)
{

}

ContourItemModel::ContourItemModel(bool useGpu, int index, const std::vector<std::vector<cv::Point> >& contours,
	const cv::Mat& matImage, const QString& filterName, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView,
	const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView, int minSizeValue,
	int minPerimeterValue, double minKFormValue, double divK, int unit, bool visible,
	QObject* parent) : QAbstractItemModel(parent), m_index(index),
	m_matImage(matImage.clone()), m_filterName(filterName), m_minSizeValue(minSizeValue),
	m_minPerimeterValue(minPerimeterValue), m_minKFormValue(minKFormValue), m_visible(visible),
	m_divK(divK), m_unit(unit), m_outsideSelectedIndex(-1), m_objectCount(0),
	m_arcLengthSum(0.0), m_contourAreaSum(0.0), m_objectAreaSum(0.0),
	m_addedImagePointCount(0), m_matImageCols(0), m_matImageRows(0)
{
	if(!m_matImage.empty())
	{
		m_matImageCols = m_matImage.cols;
		m_matImageRows = m_matImage.rows;
	}
	if(maskView.get())
	{
		//TODO refac
		m_maskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(new ICEODB_NS::MaskView(maskView->getName(),
			maskView->getColor(), maskView->isUseImage(), maskView->getAlphaImageChannel(),
			maskView->getFilterParamNameList(), maskView->getSkippedPointCount()));
	}
	if(lineView.get())
	{
		m_lineView = std::tr1::shared_ptr<ICEODB_NS::LineView>(new ICEODB_NS::LineView(lineView->getName(),
			lineView->getColor(), lineView->isUseImage(), lineView->getAlphaImageChannel(),
			lineView->getFilterParamNameList(), lineView->getLineWidth()));
	}
	cv::Mat matWorkImage(matImage.rows, matImage.cols, matImage.type(), cv::Scalar(0));
	if(useGpu)
	{
		cv::gpu::GpuMat gpuMatImage;
		cv::gpu::GpuMat gpuMatWorkImage;
		gpuMatImage.upload(matImage);
		double minKFormSqValue = minKFormValue * minKFormValue;
		int itemIndex = 1;
		for(int i=0; i<contours.size(); i++)
		{
			double arcLength = cv::arcLength(contours[i], true);

			//TODO cv::contourArea(contours[i]); Проверить как работает
			cv::drawContours(matWorkImage, contours, i, cv::Scalar(255), CV_FILLED);
			gpuMatWorkImage.upload(matWorkImage);
			double contourArea = cv::gpu::countNonZero(gpuMatWorkImage);
			cv::gpu::bitwise_and(gpuMatWorkImage, gpuMatImage, gpuMatWorkImage);
			double objectArea = cv::gpu::countNonZero(gpuMatWorkImage);
			matWorkImage = cv::Scalar(0);

			double kFormSq = arcLength * arcLength / contourArea; 

			if(minSizeValue <= objectArea && minPerimeterValue <= arcLength &&
				minKFormSqValue <= kFormSq)
			{
				m_arcLengthSum += arcLength;
				m_contourAreaSum += contourArea;
				m_objectAreaSum += objectArea;
				m_matContours.push_back(contours[i]);

				m_itemInfoList.append(new ContourItemInfo(itemIndex, arcLength,
					contourArea, objectArea, ""));
				itemIndex++;
			}
		}
	}
	else
	{
		double minKFormSqValue = minKFormValue * minKFormValue;
		int itemIndex = 1;
		for(int i=0; i<contours.size(); i++)
		{
			double arcLength = cv::arcLength(contours[i], true);

			//TODO cv::contourArea(contours[i]); Проверить как работает
			cv::drawContours(matWorkImage, contours, i, cv::Scalar(255), CV_FILLED);
			double contourArea = cv::countNonZero(matWorkImage);
			cv::bitwise_and(matWorkImage, matImage, matWorkImage);
			double objectArea = cv::countNonZero(matWorkImage);
			matWorkImage = cv::Scalar(0);

			double kFormSq = arcLength * arcLength / contourArea;

			if(minSizeValue <= objectArea && minPerimeterValue <= arcLength &&
				minKFormSqValue <= kFormSq)
			{
				m_arcLengthSum += arcLength;
				m_contourAreaSum += contourArea;
				m_objectAreaSum += objectArea;
				m_matContours.push_back(contours[i]);
				m_itemInfoList.append(new ContourItemInfo(itemIndex, arcLength,
					contourArea, objectArea, ""));
				itemIndex++;
			}
		}
	}
}

ContourItemModel::~ContourItemModel()
{

}

void ContourItemModel::addContourItemInfo(double arcLength, double contourArea,
	double objectArea, const QString& comment)
{
	if(arcLength > 0.0 && contourArea > 0.0 && objectArea > 0.0)
	{
		m_arcLengthSum += arcLength;
		m_contourAreaSum += contourArea;
		m_objectAreaSum += objectArea;
		m_itemInfoList.append(new ContourItemInfo(m_itemInfoList.size() + 1, arcLength,
			contourArea, objectArea, comment));
	}
}

QString ContourItemModel::getName() const
{
	if(m_maskView.get())
	{
		return m_maskView->getName();
	}
	else
	{
		return tr("Контур ") + QString::number(m_index);
	}
}

const QString& ContourItemModel::getFilterName() const
{
	return m_filterName;
}

const std::vector<std::vector<cv::Point> >& ContourItemModel::getMatContours() const
{
	return m_matContours;
}

const cv::Mat& ContourItemModel::getMatImage() const
{
	return m_matImage;
}

void ContourItemModel::setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	m_maskView = maskView;
}

const std::tr1::shared_ptr<ICEODB_NS::MaskView>& ContourItemModel::getMaskView() const
{
	return m_maskView;
}

void ContourItemModel::setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	m_lineView = lineView;
}

const std::tr1::shared_ptr<ICEODB_NS::LineView>& ContourItemModel::getLineView() const
{
	return m_lineView;
}

int ContourItemModel::getMinSizeValue() const
{
	return m_minSizeValue;
}

int ContourItemModel::getMinPerimeterValue() const
{
	return m_minPerimeterValue;
}

double ContourItemModel::getMinKFormValue() const
{
	return m_minKFormValue;
}

void ContourItemModel::addSumData(int objectCount, double objectSumArea,
	double objectSumArcLength, double skippedPointCount, int addedImagePointCount)
{
	if(m_itemInfoList.empty())
	{
		m_objectCount += objectCount;
		m_objectAreaSum += objectSumArea;
		m_arcLengthSum += objectSumArcLength;
		m_addedImagePointCount += addedImagePointCount;
	}
	if(m_maskView.get())
	{
		int skipped = m_maskView->getSkippedPointCount() + skippedPointCount;
		m_maskView->setSkippedPointCount(skipped);
	}
}

int ContourItemModel::getObjectCount() const
{
	int result = rowCount();
	if(result == 0)
	{
		result = m_objectCount;
	}
	return result;
}

double ContourItemModel::getArcLengthSum(const QModelIndexList& rows) const
{
	if(rows.isEmpty())
	{
		return m_arcLengthSum / m_divK;
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
			ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>((*iter).internalPointer());
			if(itemInfo)
			{
				result += itemInfo->getArcLength() / m_divK;
			}
		}
		return result;
	}
}

double ContourItemModel::getContourAreaSum(const QModelIndexList& rows) const
{
	if(rows.isEmpty())
	{
		return m_contourAreaSum / (m_divK * m_divK);
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
			ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>((*iter).internalPointer());
			if(itemInfo)
			{
				result += itemInfo->getContourArea() / (m_divK * m_divK);
			}
		}
		return result;
	}
}

double ContourItemModel::getObjectAreaSum(const QModelIndexList& rows) const
{
	if(rows.isEmpty())
	{
		return m_objectAreaSum / (m_divK * m_divK);
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
			ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>((*iter).internalPointer());
			if(itemInfo)
			{
				result += itemInfo->getObjectArea() / (m_divK * m_divK);
			}
		}
		return result;
	}
}

double ContourItemModel::getSkippedAreaSum() const
{
	if(m_maskView.get())
	{
		return m_maskView->getSkippedPointCount() / (m_divK * m_divK);
	}
	else
	{
		return 0.0;
	}
}

int ContourItemModel::getImagePointCount() const
{
	if(m_matImageCols > 0 && m_matImageRows > 0)
	{
		return (m_matImageCols * m_matImageRows);
	}
	else
	{
		return m_addedImagePointCount;
	}
}

double ContourItemModel::getObjectAreaPercent(const QModelIndexList& rows) const
{
	int imagePointCount = getImagePointCount();
	if(imagePointCount > 0)
	{
		double sumArea = imagePointCount - getSkippedAreaSum() * m_divK * m_divK;
		if(sumArea > 0.0)
		{
			double result = 100 * getObjectAreaSum(rows) / sumArea * m_divK * m_divK;
			if(result > 100.0)
			{
				result = 100.0;
			}
			return result;
		}
	}
	return 0.0;
}

bool ContourItemModel::isVisibleItem(int index) const
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

bool ContourItemModel::isCheckedItem(int index) const
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

void ContourItemModel::setCheckedAll()
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

void ContourItemModel::setUncheckedAll()
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

void ContourItemModel::setEnabledAll()
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

void ContourItemModel::setVisible(bool visible)
{
	m_visible = visible;
}

bool ContourItemModel::isVisible() const
{
	return m_visible;
}

QColor ContourItemModel::getFillColor() const
{
	Q_ASSERT(m_maskView);
	return m_maskView->getColor();
}

QColor ContourItemModel::getLineColor() const
{
	Q_ASSERT(m_lineView);
	return m_lineView->getColor();
}

int ContourItemModel::getLineWidth() const
{
	Q_ASSERT(m_lineView);
	return m_lineView->getLineWidth();
}

bool ContourItemModel::isDrawLine() const
{
	Q_ASSERT(m_lineView);
	return !m_lineView->isUseImage();
}

Qt::ItemFlags ContourItemModel::flags(const QModelIndex& modelIndex) const
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

int ContourItemModel::columnCount(const QModelIndex& /*parent*/) const
{
	return NUM_COLS;
}

QVariant ContourItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
			return tr("№ объекта");
		}
		else if(section == OBJECT_AREA_COL)
		{
			return tr("Площадь объекта");
		}
		else if(section == ARC_LEN_COL)
		{
			return tr("Периметр объекта");
		}
		else if(section == CONTOUR_AREA_COL)
		{
			return tr("Площадь контура");
		}
		else if(section == COMMENT_COL)
		{
			return tr("Комментарии");
		}
	}
	return QVariant();
}

QVariant ContourItemModel::data(const QModelIndex& modelIndex, int role) const
{
	if(!modelIndex.isValid())
	{
		return QVariant();
	}
	ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		if(role == Qt::BackgroundRole)
		{
			ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
			if(itemInfo && !itemInfo->isEnabled())
			{
				return QVariant(QApplication::palette().color(QPalette::Disabled,
					QPalette::Window));
			}
		}
		else if(role == Qt::ForegroundRole)
		{
			ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
			if(itemInfo && !itemInfo->isEnabled())
			{
				return QVariant(QApplication::palette().color(QPalette::Disabled, QPalette::WindowText));
			}
		}
		else if(role == Qt::CheckStateRole && modelIndex.column() == NUMBER_COL)
		{
			return static_cast<int>(itemInfo->isChecked() ? Qt::Checked : Qt::Unchecked);
		}
		//TODO vvzh нужна оптимизация
		else if(role == Qt::DisplayRole || role == DataRole)
		{
			if(modelIndex.column() == NUMBER_COL)
			{
				return itemInfo->getIndex();
			}
			else if(modelIndex.column() == OBJECT_AREA_COL)
			{
				return itemInfo->getObjectArea() / (m_divK * m_divK);
			}
			if(modelIndex.column() == ARC_LEN_COL)
			{
				return itemInfo->getArcLength() / m_divK;
			}
			else if(modelIndex.column() == CONTOUR_AREA_COL)
			{
				return itemInfo->getContourArea() / (m_divK * m_divK);
			}
			else if(modelIndex.column() == COMMENT_COL)
			{
				return itemInfo->getComment();
			}
		}
	}
	return QVariant();
}

bool ContourItemModel::setData(const QModelIndex& modelIndex, const QVariant& value, int role)
{
	if(!modelIndex.isValid())
	{
		return false;
	}
	if(modelIndex.column() == NUMBER_COL && role == Qt::CheckStateRole)
	{
		ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
		if(itemInfo)
		{
			itemInfo->setChecked(value.toBool());
			emit dataChanged(modelIndex, modelIndex);
			return true;
		}
	}
	if(modelIndex.column() == COMMENT_COL && role == Qt::EditRole)
	{
		ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
		if(itemInfo)
		{
			itemInfo->setComment(value.toString());
			emit dataChanged(modelIndex, modelIndex);
			return true;
		}
	}
	return false;
}

QModelIndex ContourItemModel::index(int row, int column, const QModelIndex& parent) const
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

QModelIndex ContourItemModel::parent(const QModelIndex& /*child*/) const
{
	return QModelIndex();
}

int ContourItemModel::rowCount(const QModelIndex& parent) const
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

void ContourItemModel::setEnabled(const QModelIndex& modelIndex)
{
	if(!modelIndex.isValid())
	{
		return;
	}
	ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		itemInfo->setEnabled(true);
		emit dataChanged(modelIndex, modelIndex);
	}
}

void ContourItemModel::setDisabled(const QModelIndex& modelIndex)
{
	if(!modelIndex.isValid())
	{
		return;
	}
	ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		itemInfo->setEnabled(false);
		emit dataChanged(modelIndex, modelIndex);
	}
}

void ContourItemModel::setChecked(const QModelIndex& modelIndex, bool value)
{
	if(!modelIndex.isValid())
	{
		return;
	}
	ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>(modelIndex.internalPointer());
	if(itemInfo)
	{
		itemInfo->setChecked(value);
		emit dataChanged(modelIndex, modelIndex);
	}
}

void ContourItemModel::removeItems(const QModelIndexList& rows)
{
	QList<ContourItemInfo*> itemForRemove;
	QModelIndexList::const_iterator iterEnd = rows.constEnd();
	for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
		iter++)
	{
		if(!(*iter).isValid())
		{
			continue;
		}
		ContourItemInfo* itemInfo = static_cast<ContourItemInfo*>((*iter).internalPointer());
		int index = m_itemInfoList.indexOf(itemInfo);
		if(itemInfo && index >= 0 && index < m_itemInfoList.size())
		{
			beginRemoveRows(parent(*iter), index, index);
			m_arcLengthSum -= itemInfo->getArcLength();
			m_contourAreaSum -= itemInfo->getContourArea();
			m_objectAreaSum -= itemInfo->getObjectArea();
			m_itemInfoList.removeOne(itemInfo);
			if(!m_matContours.empty())
			{
				m_matContours.erase(m_matContours.begin() + index);
			}
			delete itemInfo;
			endRemoveRows();
		}
	}
	emit dataChanged(QModelIndex(), QModelIndex());
}

void ContourItemModel::setTransformation(double divK, int unit)
{
	m_divK = divK;
	m_unit = unit;
	emit dataChanged(QModelIndex(), QModelIndex());
}

int ContourItemModel::getUnit() const
{
	return m_unit;
}

QString ContourItemModel::getUnitName() const
{
	switch(m_unit)
	{
		case KM:
		{
			return tr("км");
		}
		case M:
		{
			return tr("м");
		}
		case SM:
		{
			return tr("см");
		}
		case MM:
		{
			return tr("мм");
		}
		default:
		case NONE:
		{
			return tr("пикс.");
		}
	};
}

QString ContourItemModel::getSqUnitName() const
{
	QString result = getUnitName();
	if(m_unit != 0)
	{
		result += tr(" кв.");
	}
	return result;
}

double ContourItemModel::getDivK() const
{
	return m_divK;
}

void ContourItemModel::setOutsideSelectedIndex(int index)
{
	m_outsideSelectedIndex = index;
}

int ContourItemModel::getOutsideSelectedIndex() const
{
	return m_outsideSelectedIndex;
}

} // namespace ICEIMAGE_NS
