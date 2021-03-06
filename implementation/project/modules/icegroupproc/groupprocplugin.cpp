#include "groupprocplugin.h"

#include <QtCore/QtPlugin>

#include <QtGui/QProgressDialog>

#include <iceimage/imagebuilder.h>

//TODO refac
#include <icecore/core.h>

#include "editwidget.h"

#include <iceodb/odb/filterparam.hxx>

#include <opencv/cv.h>

namespace ICEGROUPPROC_NS
{

GroupProcPlugin::GroupProcPlugin() : ICEPROC_NS::ImageProcInterface()
{

}

GroupProcPlugin::~GroupProcPlugin()
{

}

//TODO refac
void GroupProcPlugin::init()
{

}

QSharedPointer<QObject> GroupProcPlugin::createParamObject(int index) const
{
	//TODO refac
	if(index >= 0 && index < 4)
	{
		int procIndex = 0;
		int workIndex = index;
		if(index == 3)
		{
			procIndex = 1;
			workIndex = 0;
		}
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[procIndex];
		if(!item.isNull())
		{
			return item->createParamObject(workIndex);
		}
	}
	return QSharedPointer<QObject>();
}

QString GroupProcPlugin::getShortName() const
{
	return tr("ГФ");
}

QVector<int> GroupProcPlugin::calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, const QVector<int>& /*indexVector*/,
	double divK, int unit)
{
	Q_ASSERT(!imageBuilder.isNull() && filterParam.get());
	for(int i=0; i<filterParam->getChildParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> childParam = filterParam->getChildParam(i);
		if(childParam.get())
		{
			ICEPROC_NS::ImageProcInterface* proc = ICECORE_NS::Core::Instance()->getImageProc(
				childParam->getFilterType()->getDllName());
			if(proc)
			{
				emitFilterCalculated(i, getShortName());
				proc->calcMaskVector(imageBuilder, childParam, QVector<int>(),
					divK, unit);
			}
		}
	}
	return QVector<int>();
}

QSharedPointer<QObject> GroupProcPlugin::calcParamObject(int index, QSharedPointer<QObject> param,
	const QImage& image, const QRect& rect) const
{
	//TODO refac
	if(index >= 0 && index < 3 && !m_subPluginList.isEmpty())
	{
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[0];
		if(!item.isNull())
		{
			return item->calcParamObject(index, param, image, rect);
		}
	}
	return QSharedPointer<QObject>();
}

QString GroupProcPlugin::getViewName(bool severalItems) const
{
	if(severalItems)
	{
		return tr("Групповые фильтры");
	}
	else
	{
		return tr("Групповой фильтр");
	}
}

QIcon GroupProcPlugin::getViewIcon() const
{
	return QIcon(":/dvg/icegroupproc/images/filtergroup.png");
}

bool GroupProcPlugin::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam)
{
	bool result = true;
	Q_ASSERT(!imageBuilder.isNull() && filterParam.get());
	for(int i=0; i<filterParam->getChildParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> childParam = filterParam->getChildParam(i);
		if(childParam.get())
		{
			ICEPROC_NS::ImageProcInterface* proc = ICECORE_NS::Core::Instance()->getImageProc(
				childParam->getFilterType()->getDllName());
			if(proc)
			{
				if(!proc->prepareFilter(imageBuilder, childParam))
				{
					result = false;
				}
			}
		}
	}
	return result;
}

ICEPROC_NS::EditWidget* GroupProcPlugin::createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface);
}

} // namespace ICEGROUPPROC_NS

Q_EXPORT_PLUGIN2(groupprocplugin, ICEGROUPPROC_NS::GroupProcPlugin);
