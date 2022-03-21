#include "editwidget.h"

#include <QtGui/QMessageBox>

//TODO refactor
//#include <icecore/core.h>

//#include <icegui/filtermanagedialog.h>
//#include <icegui/filtereditdialog.h>

//#include <iceproc/imagewindowinterface.h>

//#include <iceodb/odb/maskparam.hxx>
//#include <iceodb/odb/maskview.hxx>
//#include <iceodb/odb/filterparam.hxx>

#include "groupprocplugin.h"

namespace ICEGROUPPROC_NS
{

EditWidget::EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
	QWidget* parent) : ICEPROC_NS::EditWidget(proc, imageWindowInterface, 0, false,
	parent)
{
	ui.setupUi(this);
	int count = ui.filterTreeWidget->resizeHeaderToContent(30);
	ui.filterTreeWidget->setColumnWidth(0, width() - count + ui.filterTreeWidget->columnWidth(0) - 5);
}

EditWidget::~EditWidget()
{

}

int EditWidget::setChildFilterParam(int index,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam)
{
	return ui.filterTreeWidget->setChildFilterParam(index, filterParam);
}

int EditWidget::getChildFilterParamCount() const
{
	return ui.filterTreeWidget->getChildFilterParamCount();
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> EditWidget::getChildFilterParam(int index) const
{
	return ui.filterTreeWidget->getChildFilterParam(index);
}

bool EditWidget::isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& /*imageBuilder*/) const
{
	return true;
}

void EditWidget::setFilterParamId(unsigned int filterParamId)
{
	if(filterParamId == 0)
	{
		ui.filterTreeWidget->clear();
	}
	ICEPROC_NS::EditWidget::setFilterParamId(filterParamId);
}

void EditWidget::setParamObject(int /*index*/, const QSharedPointer<QObject>& /*param*/,
	bool /*fromDb*/)
{
// 	if(index == ICEAVGLUMPROC_NS::Param::FILTER_BASE_MASK_TYPE ||
// 		index == ICEAVGLUMPROC_NS::Param::IMAGE_BASE_MASK_TYPE)
// 	{
// 		ui.topItemWidget->setParamObject(index, param, fromDb);
// 	}
// 	else
// 	{
// 		if(index == ICEAVGLUMPROC_NS::Param::MASK_TYPE)
// 		{
// 			ui.avgLumItemWidget->setParamObject(param, fromDb);
// 		}
// 		else if (index == ICEAVGLUMPROC_NS::Param::MASK_TYPE + 1)
// 		{
// 			ui.findObjectItemWidget->setParamObject(param, fromDb);
// 		}
// 	}
}

QSharedPointer<QObject> EditWidget::getParamObject(int /*index*/) const
{
// 	if(index == ICEAVGLUMPROC_NS::Param::FILTER_BASE_MASK_TYPE ||
// 		index == ICEAVGLUMPROC_NS::Param::IMAGE_BASE_MASK_TYPE)
// 	{
// 		return ui.topItemWidget->getParamObject(index);
// 	}
// 	else
// 	{
// 		if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
// 		{
// 			return ui.avgLumItemWidget->getParamObject();
// 		}
// 		if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
// 		{
// 			return ui.findObjectItemWidget->getParamObject();
// 		}
// 	}
	return QSharedPointer<QObject>();
}

void EditWidget::setMaskView(int /*index*/, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& /*maskView*/)
{
// 	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
// 	{
// 		ui.avgLumItemWidget->setMaskView(maskView);
// 	}
// 	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
// 	{
// 		ui.findObjectItemWidget->setMaskView(maskView);
// 	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> EditWidget::getMaskView(int /*index*/) const
{
// 	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
// 	{
// 		return ui.avgLumItemWidget->getMaskView();
// 	}
// 	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
// 	{
// 		return ui.findObjectItemWidget->getMaskView();
// 	}
	return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
}

void EditWidget::setLineView(int /*index*/, const std::tr1::shared_ptr<ICEODB_NS::LineView>& /*lineView*/)
{
// 	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
// 	{
// 		ui.findObjectItemWidget->setLineView(lineView);
// 	}
}

std::tr1::shared_ptr<ICEODB_NS::LineView> EditWidget::getLineView(int /*index*/) const
{
// 	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
// 	{
// 		return ui.findObjectItemWidget->getLineView();
// 	}
	return std::tr1::shared_ptr<ICEODB_NS::LineView>();
}

QString EditWidget::getMaskName(int /*index*/) const
{
	return tr("");
}

bool EditWidget::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& /*imageBuilder*/)
{
	//TODO vvv
	return true;//ui.topItemWidget->prepareFilter(imageBuilder);
}

} // namespace ICEGROUPPROC_NS
