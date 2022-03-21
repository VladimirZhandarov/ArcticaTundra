#include "filtermanagedialog.h"

#include <QtCore/QAbstractItemModel>

#include <icecore/filteritemmodel.h>

#include <iceodb/odb/filtertype.hxx>
#include <iceodb/odb/filterparam.hxx>

namespace ICEGUI_NS
{

FilterManageDialog::FilterManageDialog(const QStringList& dllNameList, QWidget* parent,
	Qt::WindowFlags f) : QDialog(parent, f), m_dllNameList(dllNameList), m_filterModel(0)
{
	ui.setupUi(this);

	ui.filterTreeView->setParentDialog(this);

	//TODO реализовать
	ui.showImagePB->hide();

	ui.okPB->setEnabled(false);
	showImagePB_clicked(false);
	ui.openFilterPB->setEnabled(false);
	ui.openFilterPB->hide();

	connect(ui.filterTreeView, SIGNAL(currentFilterChanged(const QModelIndex&)),
		this, SLOT(onCurrentFilterChanged(const QModelIndex&)));
	connect(ui.filterTreeView, SIGNAL(currentDescriptionChanged(const QString&)),
		this, SLOT(onCurrentDescriptionChanged(const QString&)));
	connect(ui.filterTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(filterTreeView_doubleClicked(const QModelIndex&)));
}

FilterManageDialog::~FilterManageDialog()
{

}

void FilterManageDialog::setModel(ICECORE_NS::FilterItemModel* filterModel)
{
	Q_ASSERT(filterModel);
	m_filterModel = filterModel;
	ui.filterTreeView->setModel(filterModel, m_dllNameList);
}

void FilterManageDialog::setSelectEnabled(bool flag)
{
	ui.okPB->setVisible(flag);
	ui.openFilterPB->setVisible(!flag);
	if(flag)
	{
		ui.cancelPB->setText(tr("Отмена"));
	}
	else
	{
		ui.cancelPB->setText(tr("Закрыть"));
	}
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterManageDialog::getCurrentFilterParam() const
{
	ICEODB_NS::FilterParam* filterParam = static_cast<ICEODB_NS::FilterParam*>(
		ui.filterTreeView->getCurrentModelIndex().internalPointer());
	if(filterParam)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> result = filterParam->shared_from_this();
		for(int i=0; i<result->getMaskParamCount(); i++)
		{
			m_filterModel->getMaskParam(result, i);
		}
		return result;
	}
	else
	{
		return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
}

const QModelIndex& FilterManageDialog::getCurrentFilterParamModelIndex() const
{
	return ui.filterTreeView->getCurrentModelIndex();
}

void FilterManageDialog::openFilterPB_clicked()
{
	ui.filterTreeView->onEditParamAction();
}

void FilterManageDialog::findLE_textChanged(const QString& text)
{
	ui.filterTreeView->setFilterText(text);
}

void FilterManageDialog::showImagePB_clicked(bool checked)
{
	if(checked)
	{
		ui.showImagePB->setText(tr("Скрыть картинки >>"));
		ui.imageScrollArea->show();
	}
	else
	{
		ui.showImagePB->setText(tr("<< Показать картинки"));
		ui.imageScrollArea->hide();
	}
}

void FilterManageDialog::onCurrentFilterChanged(const QModelIndex& currentModelIndex)
{
	QString filterType = "";
	if(currentModelIndex.isValid())
	{
		filterType = ICECORE_NS::FilterItemModel::getFilterType(currentModelIndex);
	}
	if(filterType.isEmpty())
	{
		ui.okPB->setEnabled(false);
	}
	else if(m_dllNameList.isEmpty())
	{
		ui.okPB->setEnabled(true);
	}
	else
	{
		ui.okPB->setEnabled(m_dllNameList.contains(filterType));
	}
	ui.openFilterPB->setEnabled(!filterType.isEmpty());
}

void FilterManageDialog::onCurrentDescriptionChanged(const QString& description)
{
	ui.descriptionTE->setPlainText(description);
}

void FilterManageDialog::filterTreeView_doubleClicked(const QModelIndex& index)
{
	if(index.isValid() && ui.okPB->isEnabled())
	{
		if(ui.okPB->isVisible())
		{
			QDialog::accept();
		}
		else
		{
			ui.filterTreeView->onEditParamAction();
		}
	}
}

} // namespace ICEGUI_NS
