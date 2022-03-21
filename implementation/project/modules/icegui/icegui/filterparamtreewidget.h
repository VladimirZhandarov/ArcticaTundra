#ifndef FILTERPARAMTREEWIDGET_H
#define FILTERPARAMTREEWIDGET_H

#include "iceguidef.h"

#include <QtGui/QTreeWidget>

#include <boost/tr1/memory.hpp>

namespace ICEODB_NS
{
	class FilterParam;
	class MaskParam;
	class MaskView;
	class LineView;
}

namespace ICECORE_NS
{
	class FilterItemModel;
}

namespace ICEGUI_NS
{

enum
{
	NAME_COL = 0,
	MASK_NAME_COL,
	MASK_COLOR_COL,
	LINE_NAME_COL,
	OBJECT_COLOR_COL,
	LINE_COLOR_COL,
	NUM_COLS
};

class ICEGUI_EXPORT FilterParamTreeWidgetItem : public QTreeWidgetItem
{
public:
	FilterParamTreeWidgetItem(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
		int emptyMaskNumber, int index, QTreeWidget* parent);
	virtual ~FilterParamTreeWidgetItem();
	
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getFilterParam() const;
	int getIndex() const;
	bool isUsedMask() const;
	bool isUsedLine() const;

	void setColor(int columnNumber, const QColor& color);
	QColor getColor(int columnNumber) const;
	void setUseImage(int columnNumber, bool useImage);
	bool isUseImage(int columnNumber) const;

private:
	ICEODB_NS::MaskView* getMaskViewByColumn(int columnNumber) const;
	void setContent(int columnNumber, ICEODB_NS::MaskView* view);

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> m_filterParam;
	int m_index;
	QBrush m_defaultBackgroundBrush;
	std::tr1::shared_ptr<ICEODB_NS::MaskView> m_maskView;
	std::tr1::shared_ptr<ICEODB_NS::MaskView> m_objectView;
	std::tr1::shared_ptr<ICEODB_NS::LineView> m_lineView;
};

class ICEGUI_EXPORT FilterParamTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	*/
	FilterParamTreeWidget(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~FilterParamTreeWidget();

	int resizeHeaderToContent(int addedWidth = 0);
	int setChildFilterParam(int index, const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);
	int getChildFilterParamCount() const;
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getChildFilterParam(int index) const;

	void setContextMenuEnabled(bool enabled);

protected slots:
	void onCustomContextMenuRequested(const QPoint& pos);
	void addAction_triggered();
	void editAction_triggered();
	void removeAction_triggered();

	void editMaskColorAction_triggered();
	void disableMaskColorAction_triggered(bool checked);
	void editObjectColorAction_triggered();
	void disableObjectColorAction_triggered(bool checked);
	void editLineColorAction_triggered();
	void disableLineColorAction_triggered(bool checked);

private:
	void showColorDialog(int colunmNumber);
	void setUseImage(int colunmNumber, bool checked);

	QMenu* m_contextMenu;
	QAction* m_addAction;
	QAction* m_editAction;
	QAction* m_removeAction;

	QAction* m_editMaskColorAction;
	QAction* m_disableMaskColorAction;
	QAction* m_editObjectColorAction;
	QAction* m_disableObjectColorAction;
	QAction* m_editLineColorAction;
	QAction* m_disableLineColorAction;

	QTreeWidgetItem* m_currentItem;
};

} // namespace ICEGUI_NS

#endif  // FILTERPARAMTREEWIDGET_H
