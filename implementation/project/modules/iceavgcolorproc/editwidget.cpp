#include "editwidget.h"

#include <QtGui/QMessageBox>

#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include "avgcolorprocplugin.h"

namespace ICEAVGCOLORPROC_NS
{

EditWidget::EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
	QWidget* parent) : ICEPROC_NS::EditWidget(proc, imageWindowInterface, 2, false, parent)
{
	ui.setupUi(this);
	ui.item1Widget->init(0, m_proc, m_imageWindowInterface, Qt::red);
	ui.item1Widget->setTitle(tr("Эталон 1"));
	connect(ui.item1Widget, SIGNAL(maskCreated(int, const QString&)),
		this, SIGNAL(maskCreated(int, const QString&)));
	connect(ui.item1Widget, SIGNAL(maskRemoved(int)), this, SIGNAL(maskRemoved(int)));

	ui.item2Widget->init(1, m_proc, m_imageWindowInterface, Qt::blue);
	ui.item2Widget->setTitle(tr("Эталон 2"));
	connect(ui.item2Widget, SIGNAL(maskCreated(int, const QString&)),
		this, SIGNAL(maskCreated(int, const QString&)));
	connect(ui.item2Widget, SIGNAL(maskRemoved(int)), this, SIGNAL(maskRemoved(int)));
}

EditWidget::~EditWidget()
{

}

void EditWidget::setFilterParamId(unsigned int filterParamId)
{
	if(filterParamId == 0)
	{
		//TODO refac
		ui.item1Widget->init(0, m_proc, m_imageWindowInterface, Qt::red);
		ui.item1Widget->setTitle(tr("Эталон 1"));
		ui.item1Widget->clear();

		ui.item2Widget->init(1, m_proc, m_imageWindowInterface, Qt::blue);
		ui.item2Widget->setTitle(tr("Эталон 2"));
		ui.item2Widget->clear();
	}
	ICEPROC_NS::EditWidget::setFilterParamId(filterParamId);
}

void EditWidget::setParamObject(int index, const QSharedPointer<QObject>& param,
	bool fromDb)
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

QString EditWidget::getMaskName(int index) const
{
	if(index == 0)
	{
		return ui.item1Widget->getMaskName();
	}
	if(index == 1)
	{
		return ui.item2Widget->getMaskName();	
	}
	return "";
}

bool EditWidget::isWithImages() const
{
	return true;
}

} // namespace ICEAVGCOLORPROC_NS
