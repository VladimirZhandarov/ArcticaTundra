#include "editwidget.h"

#include <iceimage/imagebuilder.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include <QtGui/QColorDialog>

#include "findobjectprocplugin.h"

namespace ICEFINDOBJECTPROC_NS
{

EditWidget::EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
	QWidget* parent) : ICEPROC_NS::EditWidget(proc, imageWindowInterface, 2, false, parent)
{
	ui.setupUi(this);

	ui.item1Widget->init(0, Qt::yellow, Qt::red);
	ui.item1Widget->setTitle(tr("Объект 1"));

	ui.item2Widget->init(1, Qt::white, Qt::blue);
	ui.item2Widget->setTitle(tr("Объект 2"));
}

EditWidget::~EditWidget()
{

}

bool EditWidget::isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder) const
{
	if(!imageBuilder.isNull())
	{
		return (imageBuilder->getMaskListSize() - imageBuilder->getNotNullContourItemModelListSize() > 0);
	}
	else
	{
		return false;
	}
}

void EditWidget::setFilterParamId(unsigned int filterParamId)
{
	if(filterParamId == 0)
	{
		ui.item1Widget->init(0, Qt::yellow, Qt::red);
		ui.item1Widget->setTitle(tr("Объект 1"));
		ui.item1Widget->clear();

		ui.item2Widget->init(1, Qt::white, Qt::blue);
		ui.item2Widget->setTitle(tr("Объект 2"));
		ui.item2Widget->clear();
	}
	ICEPROC_NS::EditWidget::setFilterParamId(filterParamId);
}

void EditWidget::setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb)
{
	if(index == 0)
	{
		ui.item1Widget->setParamObject(param, fromDb);
	}
	else if(index == 1)
	{
		ui.item2Widget->setParamObject(param, fromDb);
	}
}

QSharedPointer<QObject> EditWidget::getParamObject(int index) const
{
	if(index == 0)
	{
		return ui.item1Widget->getParamObject();
	}
	else if(index == 1)
	{
		return ui.item2Widget->getParamObject();
	}
	return QSharedPointer<QObject>();
}

void EditWidget::setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	if(index == 0)
	{
		ui.item1Widget->setMaskView(maskView);
	}
	if(index == 1)
	{
		ui.item2Widget->setMaskView(maskView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> EditWidget::getMaskView(int index) const
{
	if(index == 0)
	{
		return ui.item1Widget->getMaskView();
	}
	if(index == 1)
	{
		return ui.item2Widget->getMaskView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
}

void EditWidget::setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	if(index == 0)
	{
		ui.item1Widget->setLineView(lineView);
	}
	if(index == 1)
	{
		ui.item2Widget->setLineView(lineView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::LineView> EditWidget::getLineView(int index) const
{
	if(index == 0)
	{
		return ui.item1Widget->getLineView();
	}
	if(index == 1)
	{
		return ui.item2Widget->getLineView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::LineView>();
}

} // namespace ICEFINDOBJECTPROC_NS
