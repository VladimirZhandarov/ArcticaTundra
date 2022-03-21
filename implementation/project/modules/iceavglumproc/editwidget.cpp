#include "editwidget.h"

#include <QtGui/QMessageBox>

#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include "avglumprocplugin.h"

namespace ICEAVGLUMPROC_NS
{

EditWidget::EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
	QWidget* parent) : ICEPROC_NS::EditWidget(proc, imageWindowInterface, 4, false,
	parent)
{
	ui.setupUi(this);

	ui.topItemWidget->init(m_proc, m_imageWindowInterface);

	ui.item1Widget->init(0, m_proc, m_imageWindowInterface, Qt::red, ui.topItemWidget->getRedColor(),
		ui.topItemWidget->getGreenColor(), ui.topItemWidget->getBlueColor());
	ui.item1Widget->setTitle(tr("Эталон 1"));
	connect(ui.item1Widget, SIGNAL(maskCreated(int, const QString&)),
		this, SIGNAL(maskCreated(int, const QString&)));
	connect(ui.item1Widget, SIGNAL(maskRemoved(int)), this, SIGNAL(maskRemoved(int)));
	connect(ui.item1Widget, SIGNAL(maskChanged()), this, SLOT(onMaskChanged()));
	connect(ui.topItemWidget, SIGNAL(imageBaseValueChanged(int)),
		ui.item1Widget, SLOT(onImageBaseValueChanged(int)));
	connect(ui.topItemWidget, SIGNAL(maskPBEnabled(bool)),
		ui.item1Widget, SLOT(onMaskPBEnabled(bool)));
	connect(ui.topItemWidget, SIGNAL(channelTypeChanged(int)),
		ui.item1Widget, SLOT(onChannelTypeChanged(int)));

	ui.item2Widget->init(1, m_proc, m_imageWindowInterface, Qt::blue, ui.topItemWidget->getRedColor(),
		ui.topItemWidget->getGreenColor(), ui.topItemWidget->getBlueColor());
	ui.item2Widget->setTitle(tr("Эталон 2"));
	connect(ui.item2Widget, SIGNAL(maskCreated(int, const QString&)),
		this, SIGNAL(maskCreated(int, const QString&)));
	connect(ui.item2Widget, SIGNAL(maskRemoved(int)), this, SIGNAL(maskRemoved(int)));
	connect(ui.item2Widget, SIGNAL(maskChanged()), this, SLOT(onMaskChanged()));
	connect(ui.topItemWidget, SIGNAL(imageBaseValueChanged(int)),
		ui.item2Widget, SLOT(onImageBaseValueChanged(int)));
	connect(ui.topItemWidget, SIGNAL(maskPBEnabled(bool)),
		ui.item2Widget, SLOT(onMaskPBEnabled(bool)));
	connect(ui.topItemWidget, SIGNAL(channelTypeChanged(int)),
		ui.item2Widget, SLOT(onChannelTypeChanged(int)));

	ui.item1Widget->setMaskPBEnabled(true);
	ui.item2Widget->setMaskPBEnabled(true);

	connect(ui.topItemWidget, SIGNAL(baseChanged()), this, SLOT(onBaseChanged()));
	connect(ui.topItemWidget, SIGNAL(paramFilterBaseChanged(QSharedPointer<QObject>)),
		ui.item1Widget, SLOT(onParamFilterBaseChanged(QSharedPointer<QObject>)));
	connect(ui.topItemWidget, SIGNAL(paramFilterBaseChanged(QSharedPointer<QObject>)),
		ui.item2Widget, SLOT(onParamFilterBaseChanged(QSharedPointer<QObject>)));
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
			return ui.topItemWidget->isCanUsed() &&
				ui.item1Widget->isCanUsed() && ui.item2Widget->isCanUsed();
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

	ui.item1Widget->init(0, m_proc, m_imageWindowInterface, Qt::red, ui.topItemWidget->getRedColor(),
		ui.topItemWidget->getGreenColor(), ui.topItemWidget->getBlueColor());
	ui.item1Widget->setTitle(tr("Эталон 1"));
	ui.item1Widget->clear();

	ui.item2Widget->init(1, m_proc, m_imageWindowInterface, Qt::blue, ui.topItemWidget->getRedColor(),
		ui.topItemWidget->getGreenColor(), ui.topItemWidget->getBlueColor());
	ui.item2Widget->setTitle(tr("Эталон 2"));
	ui.item2Widget->clear();

	ui.topItemWidget->setFilterParamId(filterParamId);
	ICEPROC_NS::EditWidget::setFilterParamId(filterParamId);
}

void EditWidget::setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb)
{
	if(index == Param::FILTER_BASE_MASK_TYPE || index == Param::IMAGE_BASE_MASK_TYPE)
	{
		ui.topItemWidget->setParamObject(index, param, fromDb);
	}
	else
	{
		if(index == Param::MASK_TYPE)
		{
			ui.item1Widget->setParamObject(param, fromDb);
		}
		else if (index == Param::MASK_TYPE + 1)
		{
			ui.item2Widget->setParamObject(param, fromDb);
		}
	}
}

QSharedPointer<QObject> EditWidget::getParamObject(int index) const
{
	if(index == Param::FILTER_BASE_MASK_TYPE || index == Param::IMAGE_BASE_MASK_TYPE)
	{
		return ui.topItemWidget->getParamObject(index);
	}
	else
	{
		if(index - Param::MASK_TYPE == 0)
		{
			return ui.item1Widget->getParamObject();
		}
		if(index - Param::MASK_TYPE == 1)
		{
			return ui.item2Widget->getParamObject();
		}
	}
	return QSharedPointer<QObject>();
}

void EditWidget::setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	if(index - Param::MASK_TYPE == 0)
	{
		ui.item1Widget->setMaskView(maskView);
	}
	if(index - Param::MASK_TYPE == 1)
	{
		ui.item2Widget->setMaskView(maskView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> EditWidget::getMaskView(int index) const
{
	if(index - Param::MASK_TYPE == 0)
	{
		return ui.item1Widget->getMaskView();
	}
	if(index - Param::MASK_TYPE == 1)
	{
		return ui.item2Widget->getMaskView();
	}
	return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
}

QString EditWidget::getMaskName(int index) const
{
	if(index - Param::MASK_TYPE == 0)
	{
		return ui.item1Widget->getMaskName();
	}
	if(index - Param::MASK_TYPE == 1)
	{
		return ui.item2Widget->getMaskName();
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
	if(m_imageWindowInterface)
	{
		emit currentImageBuilderChanged(m_imageWindowInterface->getCurrentImageBuilder());
	}
}

} // namespace ICEAVGLUMPROC_NS
