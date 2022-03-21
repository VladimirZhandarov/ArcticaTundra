#include "contourtreeview.h"

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>

#include <QtCore/QTextCodec>
#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include <iceimage/contouritemmodel.h>

#include "contoursortproxymodel.h"
#include "lineeditdelegate.h"

namespace ICEGUI_NS
{

ContourTreeView::ContourTreeView(QWidget* parent) : QTreeView(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSortingEnabled(true);
	setAlternatingRowColors(true);
	sortByColumn(0, Qt::AscendingOrder);

	m_contextMenu = new QMenu(this);

	m_disabledAction = m_contextMenu->addAction(tr("Скрыть"));
	connect(m_disabledAction, SIGNAL(triggered()), this, SLOT(onDisabledAction()));
	m_enabledAction = m_contextMenu->addAction(tr("Отобразить"));
	connect(m_enabledAction, SIGNAL(triggered()), this, SLOT(onEnabledAction()));
	m_enabledAllAction = m_contextMenu->addAction(tr("Отобразить все"));
	connect(m_enabledAllAction, SIGNAL(triggered()), this, SLOT(onEnabledAllAction()));
	m_contextMenu->addSeparator();
	m_selectAction = m_contextMenu->addAction(tr("Выбрать"));
	connect(m_selectAction, SIGNAL(triggered()), this, SLOT(onSelectAction()));
	m_unselectAction = m_contextMenu->addAction(tr("Снять выбор"));
	connect(m_unselectAction, SIGNAL(triggered()), this, SLOT(onUnselectAction()));
	m_contextMenu->addSeparator();
	m_removeAction = m_contextMenu->addAction(tr("Удалить"));
	connect(m_removeAction, SIGNAL(triggered()), this, SLOT(onRemoveAction()));

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(onCustomContextMenuRequested(const QPoint&)));
}

ContourTreeView::~ContourTreeView()
{

}

void ContourTreeView::setModel(QAbstractItemModel* model)
{
	ContourSortProxyModel* proxyModel = new ContourSortProxyModel(this);
	proxyModel->setSourceModel(model);
	QTreeView::setModel(proxyModel);
	setEditTriggers(QAbstractItemView::DoubleClicked);
	setItemDelegateForColumn(ICEIMAGE_NS::ContourItemModel::COMMENT_COL,
		new ICEGUI_NS::LineEditDelegate(this));
}

void ContourTreeView::selectAll()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(sortModel && contourItemModel)
	{
		contourItemModel->setCheckedAll();
		sortModel->invalidate();
	}
}

void ContourTreeView::unselectAll()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(sortModel && contourItemModel)
	{
		contourItemModel->setUncheckedAll();
		sortModel->invalidate();
	}
}

void ContourTreeView::onCustomContextMenuRequested(const QPoint& pos)
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model());
	if(!sortModel)
	{
		return;
	}
	bool flag = false;
	QItemSelectionModel* itemSelectionModel = selectionModel();
	if(itemSelectionModel && !itemSelectionModel->selectedRows().isEmpty())
	{
		flag = true;
	}
	m_enabledAction->setEnabled(flag);
	m_disabledAction->setEnabled(flag);
	m_enabledAllAction->setEnabled(sortModel->rowCount() > 0);
	m_selectAction->setEnabled(flag);
	m_unselectAction->setEnabled(flag);
	m_removeAction->setEnabled(flag);
	m_contextMenu->popup(mapToGlobal(pos));
}

void ContourTreeView::onEnabledAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	QItemSelectionModel* itemSelectionModel = selectionModel();
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(itemSelectionModel && contourItemModel && sortModel)
	{
		QModelIndexList rows = itemSelectionModel->selectedRows();
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			contourItemModel->setEnabled(sortModel->mapToSource(*iter));
		}
	}
}

void ContourTreeView::onDisabledAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	QItemSelectionModel* itemSelectionModel = selectionModel();
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(itemSelectionModel && contourItemModel && sortModel)
	{
		QModelIndexList rows = itemSelectionModel->selectedRows();
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			contourItemModel->setDisabled(sortModel->mapToSource(*iter));
		}
	}
}

void ContourTreeView::onEnabledAllAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model());
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(sortModel && contourItemModel)
	{
		contourItemModel->setEnabledAll();
		sortModel->invalidate();
	}
}

void ContourTreeView::onSelectAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	QItemSelectionModel* itemSelectionModel = selectionModel();
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(itemSelectionModel && contourItemModel && sortModel)
	{
		QModelIndexList rows = itemSelectionModel->selectedRows();
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			contourItemModel->setChecked(sortModel->mapToSource(*iter), true);
		}
	}
}

void ContourTreeView::onUnselectAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	QItemSelectionModel* itemSelectionModel = selectionModel();
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(itemSelectionModel && contourItemModel && sortModel)
	{
		QModelIndexList rows = itemSelectionModel->selectedRows();
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			contourItemModel->setChecked(sortModel->mapToSource(*iter), false);
		}
	}
}

void ContourTreeView::onRemoveAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	QItemSelectionModel* itemSelectionModel = selectionModel();
	ICEIMAGE_NS::ContourItemModel* contourItemModel = getContourItemModel();
	if(itemSelectionModel && contourItemModel && sortModel)
	{
		QModelIndexList rows = itemSelectionModel->selectedRows();
		if(rows.isEmpty())
		{
			return;
		}
		int ret = QMessageBox::information(this, tr("ПО ЛЕД"), tr("Удалить выбранные объекты?"),
			tr("Да"), tr("Нет"));
		if(ret == 1)
		{
			return;
		}
		QModelIndexList sourceRows;
		QModelIndexList::const_iterator iterEnd = rows.constEnd();
		for(QModelIndexList::const_iterator iter = rows.constBegin(); iter != iterEnd;
			iter++)
		{
			sourceRows.append(sortModel->mapToSource(*iter));
		}
		qSort(sourceRows.begin(), sourceRows.end(), qGreater<QModelIndex>());
		contourItemModel->removeItems(sourceRows);
	}
	sortModel->invalidate();
}

ICEIMAGE_NS::ContourItemModel* ContourTreeView::getContourItemModel() const
{
	QSortFilterProxyModel* itemModel = (QSortFilterProxyModel*)model();
	if(!itemModel)
	{
		return 0;
	}
	return dynamic_cast<ICEIMAGE_NS::ContourItemModel*>(itemModel->sourceModel());
}

} // namespace ICEGUI_NS
