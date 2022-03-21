#include "filtertreeview.h"

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
#include <QtGui/QMessageBox>

#include <iceodb/odb/filtersection.hxx>
#include <iceodb/odb/filterparam.hxx>

#include <icecore/filteritemmodel.h>

#include "filtersortproxymodel.h"
#include "filtereditdialog.h"
#include "sectioneditdialog.h"

namespace ICEGUI_NS
{

FilterTreeView::FilterTreeView(QWidget* parent) : QTreeView(parent), m_parent(0)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setSortingEnabled(true);
	setAlternatingRowColors(true);
	sortByColumn(0, Qt::AscendingOrder);

	m_contextMenu = new QMenu(this);

	m_addSectionAction = m_contextMenu->addAction(tr("Добавить раздел"));
	connect(m_addSectionAction, SIGNAL(triggered()), this, SLOT(onAddSectionAction()));

	m_editSectionAction = m_contextMenu->addAction(tr("Изменить раздел"));
	connect(m_editSectionAction, SIGNAL(triggered()), this, SLOT(onEditSectionAction()));

	m_removeSectionAction = m_contextMenu->addAction(tr("Удалить раздел"));
	connect(m_removeSectionAction, SIGNAL(triggered()), this, SLOT(onRemoveSectionAction()));

	m_contextMenu->addSeparator();

	m_editParamAction = m_contextMenu->addAction(tr("Редактировать фильтр"));
	connect(m_editParamAction, SIGNAL(triggered()), this, SLOT(onEditParamAction()));

	m_removeParamAction = m_contextMenu->addAction(tr("Удалить фильтр"));
	connect(m_removeParamAction, SIGNAL(triggered()), this, SLOT(onRemoveParamAction()));

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(onCustomContextMenuRequested(const QPoint&)));
}

FilterTreeView::~FilterTreeView()
{

}

void FilterTreeView::setParentDialog(QWidget* parent)
{
	m_parent = parent;
}

void FilterTreeView::setFilterText(const QString& text)
{
	FilterSortProxyModel* proxyModel = dynamic_cast<FilterSortProxyModel*>(model());
	if(proxyModel)
	{
		proxyModel->setFilterText(text);
	}
}

void FilterTreeView::setModel(QAbstractItemModel* model, const QStringList& dllNameList)
{
	FilterSortProxyModel* proxyModel = new FilterSortProxyModel(this);
	proxyModel->setSourceModel(model);
	QTreeView::setModel(proxyModel);
	if(proxyModel)
	{
		expand(proxyModel->index(0, 0));
		setRootIndex(proxyModel->index(0, 0));
		proxyModel->setAcceptedFilterTypeList(dllNameList);
	}
}

void FilterTreeView::setTreeViewType(TreeViewType treeViewType)
{
	if(treeViewType == EDIT_FILTER_PARAM_TYPE)
	{
		m_editParamAction->setVisible(true);
		m_removeParamAction->setVisible(true);
	}
	else if(treeViewType == EDIT_FILTER_SECTION_TYPE)
	{
		m_editParamAction->setVisible(false);
		m_removeParamAction->setVisible(false);
	}
}

const QModelIndex& FilterTreeView::getCurrentModelIndex() const
{
	return m_currentModelIndex;
}

void FilterTreeView::mousePressEvent(QMouseEvent* event)
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model());
	if(!sortModel)
	{
		return;
	}
	m_currentModelIndex = sortModel->mapToSource(indexAt(event->pos()));
	setCurrentIndex(m_currentModelIndex);
	emit currentFilterChanged(m_currentModelIndex);
	QTreeView::mousePressEvent(event);
}

void FilterTreeView::onCustomContextMenuRequested(const QPoint& pos)
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model());
	if(!sortModel)
	{
		return;
	}
	m_currentModelIndex = sortModel->mapToSource(indexAt(pos));

	bool sectionFlag = ICECORE_NS::FilterItemModel::isFilterSection(m_currentModelIndex);
	m_editSectionAction->setEnabled(sectionFlag);
	m_removeSectionAction->setEnabled(sectionFlag);
	bool paramFlag = ICECORE_NS::FilterItemModel::isFilterParam(m_currentModelIndex);
	m_editParamAction->setEnabled(paramFlag);
	m_removeParamAction->setEnabled(paramFlag);
	m_contextMenu->popup(mapToGlobal(pos));
}

void FilterTreeView::onAddSectionAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	ICECORE_NS::FilterItemModel* filterModel = getFilterItemModel();
	if(!sortModel || !filterModel)
	{
		return;
	}
	SectionEditDialog dialog(filterModel, m_currentModelIndex,
		std::tr1::shared_ptr<ICEODB_NS::FilterSection>(), this);
	if(dialog.exec() == QDialog::Accepted)
	{
		try
		{
			filterModel->addSection(dialog.getResultSection(), m_currentModelIndex);
		}
		catch(const ICECORE_NS::DataBaseException& exc)
		{
			QMessageBox::warning(this, tr("Ошибка работы с базой данных"),
				QString::fromStdString(exc.getErrorDescription()));
		}
		sortModel->invalidate();
		setRootIndex(sortModel->index(0, 0));
	}
}

void FilterTreeView::onEditSectionAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	ICECORE_NS::FilterItemModel* filterModel = getFilterItemModel();
	if(!sortModel || !filterModel || !m_currentModelIndex.isValid())
	{
		return;
	}

	//TODO: refac
	ICEODB_NS::FilterSection* section =
		ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
		m_currentModelIndex.internalPointer());
	if(!section)
	{
		return;
	}
	SectionEditDialog dialog(filterModel, m_currentModelIndex.parent(), section->shared_from_this(),
		this);
	if(dialog.exec() == QDialog::Accepted)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterSection> resultSection = dialog.getResultSection();
		Q_ASSERT(resultSection.get());
		try
		{
			filterModel->editSection(resultSection);
		}
		catch(const ICECORE_NS::DataBaseException& exc)
		{
			QMessageBox::warning(this, tr("Ошибка работы с базой данных"),
				QString::fromStdString(exc.getErrorDescription()));
		}
		sortModel->invalidate();
		setRootIndex(sortModel->index(0, 0));
		emit currentDescriptionChanged(resultSection->getDescription());
	}
}

void FilterTreeView::onRemoveSectionAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	ICECORE_NS::FilterItemModel* filterModel = getFilterItemModel();
	if(!sortModel || !filterModel || !m_currentModelIndex.isValid())
	{
		return;
	}
	try
	{
		filterModel->removeSection(m_currentModelIndex);
	}
	catch(const ICECORE_NS::DataBaseException& exc)
	{
		QMessageBox::warning(this, tr("Ошибка работы с базой данных"),
			QString::fromStdString(exc.getErrorDescription()));
	}
	sortModel->invalidate();
	setRootIndex(sortModel->index(0, 0));
	emit currentDescriptionChanged("");
}

void FilterTreeView::onEditParamAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	ICECORE_NS::FilterItemModel* filterModel = getFilterItemModel();
	if(!sortModel || !filterModel || !m_currentModelIndex.isValid())
	{
		return;
	}
	if(!ICECORE_NS::FilterItemModel::isFilterParam(m_currentModelIndex))
	{
		return;
	}
	FilterEditDialog* dialog = FilterEditDialog::createInstance(m_currentModelIndex,
		m_parent, Qt::Dialog | Qt::WindowMinimizeButtonHint);
	if(dialog)
	{
		dialog->show(std::tr1::shared_ptr<ICEODB_NS::FilterParam>(), false);
	}
}

void FilterTreeView::onRemoveParamAction()
{
	QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(model()); 
	ICECORE_NS::FilterItemModel* filterModel = getFilterItemModel();
	if(!sortModel || !filterModel || !m_currentModelIndex.isValid())
	{
		return;
	}
	try
	{
		filterModel->removeParam(m_currentModelIndex);
	}
	catch(const ICECORE_NS::DataBaseException& exc)
	{
		QMessageBox::warning(this, tr("Ошибка работы с базой данных"),
			QString::fromStdString(exc.getErrorDescription()));
	}
	sortModel->invalidate();
	setRootIndex(sortModel->index(0, 0));
	emit currentDescriptionChanged("");
}

ICECORE_NS::FilterItemModel* FilterTreeView::getFilterItemModel() const
{
	QSortFilterProxyModel* itemModel = (QSortFilterProxyModel*)model();
	if(!itemModel)
	{
		return 0;
	}
	return dynamic_cast<ICECORE_NS::FilterItemModel*>(itemModel->sourceModel());
}

void FilterTreeView::setCurrentIndex(const QModelIndex& current)
{
	if(current.isValid())
	{
		ICECORE_NS::FilterItemModel* filterModel = getFilterItemModel();
		if(filterModel)
		{
			//TODO: refac
			ICEODB_NS::FilterSection* section =
				ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterSection>(
				current.internalPointer());
			if(section)
			{
				emit currentDescriptionChanged(section->getDescription());
				return;
			}
			//TODO: refac
			ICEODB_NS::FilterParam* param =
				ICECORE_NS::FilterItemModel::castFilterToType<ICEODB_NS::FilterParam>(
				current.internalPointer());
			if(param)
			{
				emit currentDescriptionChanged(param->getDescription());
				return;
			}
		}
	}
	emit currentDescriptionChanged("");
}

} // namespace ICEGUI_NS
