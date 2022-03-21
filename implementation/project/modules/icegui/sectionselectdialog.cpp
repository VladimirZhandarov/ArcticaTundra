#include "sectionselectdialog.h"

#include <QtCore/QAbstractItemModel>

#include <icecore/filteritemmodel.h>

namespace ICEGUI_NS
{

SectionSelectDialog::SectionSelectDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
	ui.filterTreeView->setTreeViewType(FilterTreeView::EDIT_FILTER_SECTION_TYPE);

	connect(ui.filterTreeView, SIGNAL(currentFilterChanged(const QModelIndex&)),
		this, SLOT(onCurrentFilterChanged(const QModelIndex&)));
	connect(ui.filterTreeView, SIGNAL(currentDescriptionChanged(const QString&)),
		this, SLOT(onCurrentDescriptionChanged(const QString&)));
	connect(ui.filterTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(filterTreeView_doubleClicked(const QModelIndex&)));
}

SectionSelectDialog::~SectionSelectDialog()
{

}

void SectionSelectDialog::setModel(QAbstractItemModel* model)
{
	ui.filterTreeView->setModel(model);
}

const QModelIndex& SectionSelectDialog::getCurrentModelIndex() const
{
	return ui.filterTreeView->getCurrentModelIndex();
}

void SectionSelectDialog::onCurrentFilterChanged(const QModelIndex& currentModelIndex)
{
	if(currentModelIndex.isValid())
	{
		ui.okPB->setEnabled(ICECORE_NS::FilterItemModel::getFilterType(currentModelIndex).isEmpty());
	}
	else
	{
		ui.okPB->setEnabled(false);
	}
}

void SectionSelectDialog::onCurrentDescriptionChanged(const QString& description)
{
	ui.descriptionTE->setPlainText(description);
}

void SectionSelectDialog::filterTreeView_doubleClicked(const QModelIndex& index)
{
	if(index.isValid())
	{
		if(ICECORE_NS::FilterItemModel::getFilterType(index).isEmpty())
		{
			QDialog::accept();
		}
	}
}

} // namespace ICEGUI_NS
