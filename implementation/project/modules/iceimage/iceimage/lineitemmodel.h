#ifndef LINEITEMMODEL_H
#define LINEITEMMODEL_H

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
}

namespace ICEIMAGE_NS
{

class LineItemInfo
{
public:
	LineItemInfo(int index, const QPoint& point1, const QPoint& point2, double length);
	~LineItemInfo();

	int getIndex() const;
	const QPoint& getPoint1() const;
	const QPoint& getPoint2() const;
	double getLength() const;

	void setChecked(bool value);
	bool isChecked() const;

	void setEnabled(bool value);
	bool isEnabled() const;

	void setComment(const QString& comment);
	const QString& getComment() const;

private:
	int m_index;
	QPoint m_point1;
	QPoint m_point2;
	double m_length;
	bool m_checked;
	bool m_enabled;
	QString m_comment;
};

class ICEIMAGE_EXPORT LineItemModel : public QAbstractItemModel
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
		POINT1_COL,
		POINT2_COL,
		LEN_COL,
		COMMENT_COL,
		NUM_COLS
	};

	/**
	* Конструктор
	* @param parent родительский объект
	*/
	LineItemModel(int index, const std::vector<cv::Vec4i>& lines,
		const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView, bool visible = true,
		QObject* parent = 0);

	/**
	* Деструктор
	*/
	~LineItemModel();

	QString getName() const;

	const std::vector<cv::Vec4i>& getMatLines() const;

	double getLineLengthSum(const QModelIndexList& rows = QModelIndexList()) const;

	bool isVisibleItem(int index) const;
	bool isCheckedItem(int index) const;

	void setCheckedAll();
	void setUncheckedAll();
	void setEnabledAll();

	void setVisible(bool visible);
	bool isVisible() const;

	QColor getColor() const;

	virtual Qt::ItemFlags flags(const QModelIndex& modelIndex) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex& modelIndex, int role) const;
	virtual bool setData(const QModelIndex& modelIndex, const QVariant& value, int role = Qt::EditRole);
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex& child) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	void setEnabled(const QModelIndex& modelIndex);
	void setDisabled(const QModelIndex& modelIndex);

	void setChecked(const QModelIndex& modelIndex, bool value);
	void removeItems(const QModelIndexList& rows);

private:
	int m_index;
	std::vector<cv::Vec4i> m_matLines;
	QList<LineItemInfo*> m_itemInfoList;
	double m_lineLengthSum;
	std::tr1::shared_ptr<ICEODB_NS::MaskView> m_maskView;
	bool m_visible;
};

} // namespace ICEIMAGE_NS

#endif // LINEITEMMODEL_H
