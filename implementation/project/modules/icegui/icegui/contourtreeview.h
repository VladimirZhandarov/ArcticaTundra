#ifndef CONTOURTREEVIEW_H
#define CONTOURTREEVIEW_H

#include "iceguidef.h"

#include <QtGui/QTreeView>

class QMenu;
class QAction;

namespace ICEIMAGE_NS
{
	class ContourItemModel;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ContourTreeView : public QTreeView
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param parent родительский объект
	*/
	ContourTreeView(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~ContourTreeView();

	virtual void setModel(QAbstractItemModel* model);
	void selectAll();
	void unselectAll();

protected slots:
	void onCustomContextMenuRequested(const QPoint& pos);
	void onEnabledAction();
	void onDisabledAction();
	void onEnabledAllAction();
	void onSelectAction();
	void onUnselectAction();
	void onRemoveAction();

private:
	ICEIMAGE_NS::ContourItemModel* getContourItemModel() const;

	QMenu* m_contextMenu;
	QAction* m_enabledAction;
	QAction* m_disabledAction;
	QAction* m_enabledAllAction;
	QAction* m_selectAction;
	QAction* m_unselectAction;
	QAction* m_removeAction;
};

} // namespace ICEGUI_NS

#endif  // CONTOURTREEVIEW_H
