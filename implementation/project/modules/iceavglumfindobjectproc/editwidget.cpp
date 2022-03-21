#include "editwidget.h"

#include <QtGui/QMessageBox>

#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include <iceavglumproc/param.h>

#include "avglumfindobjectprocplugin.h"

namespace ICEAVGLUMFINDOBJECTPROC_NS
{

EditWidget::EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
	QWidget* parent) : ICEPROC_NS::EditWidget(proc, imageWindowInterface, 4, false,
	parent)
{
	ui.setupUi(this);

	ui.topItemWidget->init(m_proc, m_imageWindowInterface);

	ui.avgLumItemWidget->init(0, m_proc, m_imageWindowInterface, Qt::red,
		ui.topItemWidget->getRedColor(),
		ui.topItemWidget->getGreenColor(), ui.topItemWidget->getBlueColor());
	ui.avgLumItemWidget->setTitle(tr("Эталон"));
	ui.avgLumItemWidget->setCheckable(false);
	ui.avgLumItemWidget->setMaskPBEnabled(true);
	connect(ui.avgLumItemWidget, SIGNAL(maskCreated(int, const QString&)),
		this, SIGNAL(maskCreated(int, const QString&)));
	connect(ui.avgLumItemWidget, SIGNAL(maskRemoved(int)), this, SIGNAL(maskRemoved(int)));
	connect(ui.avgLumItemWidget, SIGNAL(maskChanged()), this, SLOT(onMaskChanged()));
	connect(ui.topItemWidget, SIGNAL(imageBaseValueChanged(int)),
		ui.avgLumItemWidget, SLOT(onImageBaseValueChanged(int)));
	connect(ui.topItemWidget, SIGNAL(maskPBEnabled(bool)),
		ui.avgLumItemWidget, SLOT(onMaskPBEnabled(bool)));
	connect(ui.topItemWidget, SIGNAL(channelTypeChanged(int)),
		ui.avgLumItemWidget, SLOT(onChannelTypeChanged(int)));
	connect(ui.topItemWidget, SIGNAL(baseChanged()), this, SLOT(onBaseChanged()));
	connect(ui.topItemWidget, SIGNAL(paramFilterBaseChanged(QSharedPointer<QObject>)),
		ui.avgLumItemWidget, SLOT(onParamFilterBaseChanged(QSharedPointer<QObject>)));

	ui.findObjectItemWidget->init(1, Qt::yellow, Qt::blue);
	ui.findObjectItemWidget->setTitle(tr("Объект"));
}

EditWidget::~EditWidget()
{

}

bool EditWidget::isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder) const
{
	if(ICEPROC_NS::EditWidget::isCanUsed(imageBuilder))
	{
		if(m_filterParamId == 0)
		{
			return ui.topItemWidget->isCanUsed() && ui.avgLumItemWidget->isCanUsed();
		}
		else
		{
			return true;
		}
	}
	return false;
}

void EditWidget::setFilterParamId(unsigned int filterParamId)
{
	ui.topItemWidget->clear();

	ui.avgLumItemWidget->init(0, m_proc, m_imageWindowInterface, Qt::red, 
		ui.topItemWidget->getRedColor(),
		ui.topItemWidget->getGreenColor(), ui.topItemWidget->getBlueColor());
	ui.avgLumItemWidget->setTitle(tr("Эталон"));
	ui.avgLumItemWidget->clear();

	ui.findObjectItemWidget->init(1, Qt::yellow, Qt::blue);
	ui.findObjectItemWidget->setTitle(tr("Объект"));
	ui.findObjectItemWidget->clear();

	ui.topItemWidget->setFilterParamId(filterParamId);
	ICEPROC_NS::EditWidget::setFilterParamId(filterParamId);
}

void EditWidget::setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb)
{
	if(index == ICEAVGLUMPROC_NS::Param::FILTER_BASE_MASK_TYPE ||
		index == ICEAVGLUMPROC_NS::Param::IMAGE_BASE_MASK_TYPE)
	{
		ui.topItemWidget->setParamObject(index, param, fromDb);
	}
	else
	{
		if(index == ICEAVGLUMPROC_NS::Param::MASK_TYPE)
		{
			ui.avgLumItemWidget->setParamObject(param, fromDb);
		}
		else if (index == ICEAVGLUMPROC_NS::Param::MASK_TYPE + 1)
		{
			ui.findObjectItemWidget->setParamObject(param, fromDb);
		}
	}
}

QSharedPointer<QObject> EditWidget::getParamObject(int index) const
{
	if(index == ICEAVGLUMPROC_NS::Param::FILTER_BASE_MASK_TYPE ||
		index == ICEAVGLUMPROC_NS::Param::IMAGE_BASE_MASK_TYPE)
	{
		return ui.topItemWidget->getParamObject(index);
	}
	else
	{
		if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
		{
			return ui.avgLumItemWidget->getParamObject();
		}
		if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
		{
			return ui.findObjectItemWidget->getParamObject();
		}
	}
	return QSharedPointer<QObject>();
}

void EditWidget::setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
	{
		ui.avgLumItemWidget->setMaskView(maskView);
	}
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
	{
		ui.findObjectItemWidget->setMaskView(maskView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> EditWidget::getMaskView(int index) const
{
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
	{
		return ui.avgLumItemWidget->getMaskView();
	}
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
	{
		return ui.findObjectItemWidget->getMaskView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
}

void EditWidget::setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
	{
		ui.findObjectItemWidget->setLineView(lineView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::LineView> EditWidget::getLineView(int index) const
{
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
	{
		return ui.findObjectItemWidget->getLineView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::LineView>();
}

QString EditWidget::getMaskName(int index) const
{
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 0)
	{
		return ui.avgLumItemWidget->getMaskName();
	}
	if(index - ICEAVGLUMPROC_NS::Param::MASK_TYPE == 1)
	{
		return ui.findObjectItemWidget->getMaskName();
	}
	return "";
}

bool EditWidget::isWithImages() const
{
	return true;
}

bool EditWidget::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	return ui.topItemWidget->prepareFilter(imageBuilder);
}

void EditWidget::onBaseChanged()
{
	if(m_imageWindowInterface)
	{
		emit currentImageBuilderChanged(m_imageWindowInterface->getCurrentImageBuilder());
	}
}

void EditWidget::onMaskChanged()
{

}

} // namespace ICEAVGLUMFINDOBJECTPROC_NS
