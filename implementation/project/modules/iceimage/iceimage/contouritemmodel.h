#ifndef CONTOURITEMMODEL_H
#define CONTOURITEMMODEL_H

#include "iceimagedef.h"

#include <QtCore/QAbstractItemModel>

#include <QtGui/QColor>
#include <QtCore/QVector>

#include <vector>
#include <boost/tr1/memory.hpp>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning(disable: 4985)
#endif
#include <opencv2/core/core.hpp>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

namespace ICEODB_NS
{
	class MaskView;
	class LineView;
}

namespace ICEIMAGE_NS
{

class ContourItemInfo
{
public:
	ContourItemInfo(int index, double arcLength, double contourArea, double objectArea,
		const QString& comment);
	~ContourItemInfo();

	int getIndex() const;
	double getArcLength() const;
	double getContourArea() const;
	double getObjectArea() const;

	void setChecked(bool value);
	bool isChecked() const;

	void setEnabled(bool value);
	bool isEnabled() const;

	void setComment(const QString& comment);
	const QString& getComment() const;

private:
	int m_index;
	double m_arcLength;
	double m_contourArea;
	double m_objectArea;
	bool m_checked;
	bool m_enabled;
	QString m_comment;
};

class ICEIMAGE_EXPORT ContourItemModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	enum
	{
		DataRole = Qt::UserRole
	};

	enum
	{
		NUMBER_COL = 0,
		OBJECT_AREA_COL,
		ARC_LEN_COL,
		CONTOUR_AREA_COL,
		COMMENT_COL,
		NUM_COLS
	};

	ContourItemModel(int index = 0, int matImageCols = 0, int matImageRows = 0,
		QObject* parent = 0);

	/**
	* Конструктор
	* @param parent родительский объект
	*/
	ContourItemModel(bool useGpu, int index, const std::vector<std::vector<cv::Point> >& contours,
		const cv::Mat& matImage, const QString& filterName, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView,
		const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView, int minSizeValue,
		int minPerimeterValue, double minKFormValue, double divK, int unit,
		bool visible = true, QObject* parent = 0);

	/**
	* Деструктор
	*/
	~ContourItemModel();

	void addContourItemInfo(double arcLength, double contourArea, double objectArea,
		const QString& comment);

	QString getName() const;
	const QString& getFilterName() const;

	const std::vector<std::vector<cv::Point> >& getMatContours() const;
	const cv::Mat& getMatImage() const;
	void setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& getMaskView() const;
	void setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);
	const std::tr1::shared_ptr<ICEODB_NS::LineView>& getLineView() const;
	int getMinSizeValue() const;
	int getMinPerimeterValue() const;
	double getMinKFormValue() const;

	void addSumData(int objectCount, double objectSumArea, double objectSumArcLength,
		double skippedPointCount, int addedImagePointCount = 0);

	int getObjectCount() const;
	double getArcLengthSum(const QModelIndexList& rows = QModelIndexList()) const;
	double getContourAreaSum(const QModelIndexList& rows = QModelIndexList()) const;
	double getObjectAreaSum(const QModelIndexList& rows = QModelIndexList()) const;
	double getSkippedAreaSum() const;
	int getImagePointCount() const;
	double getObjectAreaPercent(const QModelIndexList& rows = QModelIndexList()) const;

	bool isVisibleItem(int index) const;
	bool isCheckedItem(int index) const;

	void setCheckedAll();
	void setUncheckedAll();
	void setEnabledAll();

	void setVisible(bool visible);
	bool isVisible() const;

	QColor getFillColor() const;
	QColor getLineColor() const;
	int getLineWidth() const;
	bool isDrawLine() const;

	virtual Qt::ItemFlags flags(const QModelIndex& modelIndex) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex& modelIndex, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& modelIndex, const QVariant& value, int role = Qt::EditRole);
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex& child) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	void setEnabled(const QModelIndex& modelIndex);
	void setDisabled(const QModelIndex& modelIndex);

	void setChecked(const QModelIndex& modelIndex, bool value);
	void removeItems(const QModelIndexList& rows);

	//TODO: vvzh подумать куда это
	void setTransformation(double divK, int unit);

	int getUnit() const;
	QString getUnitName() const;
	QString getSqUnitName() const;
	double getDivK() const;
	void setOutsideSelectedIndex(int index);
	int getOutsideSelectedIndex() const;

private:
	int m_index;
	std::vector<std::vector<cv::Point> > m_matContours;
	cv::Mat m_matImage;
	int m_matImageCols;
	int m_matImageRows;
	QList<ContourItemInfo*> m_itemInfoList;
	int m_objectCount; //TODO refac
	double m_arcLengthSum;
	double m_contourAreaSum;
	double m_objectAreaSum;
	int m_addedImagePointCount;
	QString m_filterName;
	std::tr1::shared_ptr<ICEODB_NS::MaskView> m_maskView;
	std::tr1::shared_ptr<ICEODB_NS::LineView> m_lineView;
	int m_minSizeValue;
	int m_minPerimeterValue;
	double m_minKFormValue;
	bool m_visible;
	double m_divK;
	int m_unit;
	int m_outsideSelectedIndex;
};

} // namespace ICEIMAGE_NS

#endif // CONTOURITEMMODEL_H
