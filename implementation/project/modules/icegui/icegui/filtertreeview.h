#ifndef FILTERTREEVIEW_H
#define FILTERTREEVIEW_H

#include "iceguidef.h"

#include <QtGui/QTreeView>

class QMenu;
class QAction;

namespace ICEODB_NS
{
	class FilterSection;
}

namespace ICECORE_NS
{
	class FilterItemModel;
}

namespace ICEGUI_NS
{

/**
* @class FilterTreeView
* Дерево разделов и фильтров
*/
class ICEGUI_EXPORT FilterTreeView : public QTreeView
{
	Q_OBJECT
public:
	enum TreeViewType
	{
		EDIT_FILTER_PARAM_TYPE = 0,
		EDIT_FILTER_SECTION_TYPE
	};

	/**
	* Конструктор
	* @param parent родительский объект
	*/
	FilterTreeView(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~FilterTreeView();

	void setParentDialog(QWidget* parent);
	void setFilterText(const QString& text);
	virtual void setModel(QAbstractItemModel* model, const QStringList& dllNameList = QStringList());
	void setTreeViewType(TreeViewType treeViewType);

	const QModelIndex& getCurrentModelIndex() const;

protected:
	virtual void mousePressEvent(QMouseEvent* event);

protected slots:
	void onCustomContextMenuRequested(const QPoint& pos);

	void onAddSectionAction();
	void onEditSectionAction();
	void onRemoveSectionAction();

public slots:
	void onEditParamAction();

protected slots:
	void onRemoveParamAction();

signals:
	void currentFilterChanged(const QModelIndex& currentModelIndex);
	void currentDescriptionChanged(const QString& description);

private:
	ICECORE_NS::FilterItemModel* getFilterItemModel() const;
	void setCurrentIndex(const QModelIndex& current);

	QWidget* m_parent;
	QMenu* m_contextMenu;
	QAction* m_addSectionAction;
	QAction* m_editSectionAction;
	QAction* m_removeSectionAction;
	QAction* m_editParamAction;
	QAction* m_removeParamAction;

	QModelIndex m_currentModelIndex;
};

} // namespace ICEGUI_NS

#endif // FILTERTREEVIEW_H
