#include "editwidget.h"

#include <QtGui/QMessageBox>

#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include "avgcolorfindobjectprocplugin.h"

namespace ICEAVGCOLORFINDOBJECTPROC_NS
{

EditWidget::EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
	QWidget* parent) : ICEPROC_NS::EditWidget(proc, imageWindowInterface, 2, false, parent)
{
	ui.setupUi(this);

	ui.avgColorItemWidget->init(0, proc, imageWindowInterface, Qt::red);
	ui.avgColorItemWidget->setTitle(tr("Эталон"));
	ui.avgColorItemWidget->setCheckable(false);
	connect(ui.avgColorItemWidget, SIGNAL(maskCreated(int, const QString&)),
		this, SIGNAL(maskCreated(int, const QString&)));
	connect(ui.avgColorItemWidget, SIGNAL(maskRemoved(int)), this, SIGNAL(maskRemoved(int)));

	ui.findObjectItemWidget->init(1, Qt::yellow, Qt::blue);
	ui.findObjectItemWidget->setTitle(tr("Объект"));
}

EditWidget::~EditWidget()
{

}

void EditWidget::setFilterParamId(unsigned int filterParamId)
{
	if(filterParamId == 0)
	{
		//TOD refac
		ui.avgColorItemWidget->init(0, m_proc, m_imageWindowInterface, Qt::red);
		ui.avgColorItemWidget->setTitle(tr("Эталон"));
		ui.avgColorItemWidget->clear();

		ui.findObjectItemWidget->init(1, Qt::yellow, Qt::blue);
		ui.findObjectItemWidget->setTitle(tr("Объект"));
		ui.findObjectItemWidget->clear();
	}
	ICEPROC_NS::EditWidget::setFilterParamId(filterParamId);
}

void EditWidget::setParamObject(int index, const QSharedPointer<QObject>& param,
	bool fromDb)
{
	if(index == 0)
	{
		ui.avgColorItemWidget->setParamObject(param, fromDb);
	}
	else if(index == 1)
	{
		ui.findObjectItemWidget->setParamObject(param, fromDb);
	}
}

QSharedPointer<QObject> EditWidget::getParamObject(int index) const
{
	if(index == 0)
	{
		return ui.avgColorItemWidget->getParamObject();
	}
	else if(index == 1)
	{
		return ui.findObjectItemWidget->getParamObject();
	}
	return QSharedPointer<QObject>();
}

void EditWidget::setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	if(index == 0)
	{
		ui.avgColorItemWidget->setMaskView(maskView);
	}
	if(index == 1)
	{
		ui.findObjectItemWidget->setMaskView(maskView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> EditWidget::getMaskView(int index) const
{
	if(index == 0)
	{
		return ui.avgColorItemWidget->getMaskView();
	}
	if(index == 1)
	{
		return ui.findObjectItemWidget->getMaskView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
}

void EditWidget::setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	if(index == 1)
	{
		ui.findObjectItemWidget->setLineView(lineView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::LineView> EditWidget::getLineView(int index) const
{
	if(index == 1)
	{
		return ui.findObjectItemWidget->getLineView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::LineView>();
}

QString EditWidget::getMaskName(int index) const
{
	if(index == 0)
	{
		return ui.avgColorItemWidget->getMaskName();
	}
	if(index == 1)
	{
		return ui.findObjectItemWidget->getMaskName();
	}
	return "";
}

bool EditWidget::isWithImages() const
{
	return true;
}

} // namespace ICEAVGCOLORFINDOBJECTPROC_NS
