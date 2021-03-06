#include "avglumfindobjectprocplugin.h"

#include <QtCore/QtPlugin>

#include <QtGui/QProgressDialog>

#include <iceimage/imagebuilder.h>

#include "editwidget.h"

#include <iceavglumproc/param.h>
#include <icefindobjectproc/param.h>

#include <iceavglumproc/avglumprocplugin.h>
#include <icefindobjectproc/findobjectprocplugin.h>

#include <iceodb/odb/filterparam.hxx>

#include <opencv/cv.h>

namespace ICEAVGLUMFINDOBJECTPROC_NS
{

AvgLumFindObjectProcPlugin::AvgLumFindObjectProcPlugin() : ICEPROC_NS::ImageProcInterface()
{
	m_subPluginList.append(QSharedPointer<ICEPROC_NS::ImageProcInterface>(
		new ICEAVGLUMPROC_NS::AvgLumProcPlugin()));
	m_subPluginList.append(QSharedPointer<ICEPROC_NS::ImageProcInterface>(
		new ICEFINDOBJECTPROC_NS::FindObjectProcPlugin()));
}

AvgLumFindObjectProcPlugin::~AvgLumFindObjectProcPlugin()
{

}

QSharedPointer<QObject> AvgLumFindObjectProcPlugin::createParamObject(int index) const
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

QString AvgLumFindObjectProcPlugin::getShortName() const
{
	return tr("ФИК");
}

QVector<int> AvgLumFindObjectProcPlugin::calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, const QVector<int>& /*indexVector*/,
	double divK, int unit)
{
	Q_ASSERT(!imageBuilder.isNull() && filterParam.get());
	emitFilterCalculated(0, getShortName());
	QVector<QSharedPointer<QObject> > paramVector = getParamVector(filterParam);
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > maskViewVector = getMaskViewVector(
		filterParam);
	QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > lineViewVector = getLineViewVector(
		filterParam);
	Q_ASSERT(paramVector.size() == maskViewVector.size() && paramVector.size() == lineViewVector.size());
	int countSteps = 0;
	for(int i=0; i<m_subPluginList.size(); i++)
	{
		QString subPluginShortName = "";
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[i];
		if(!item.isNull())
		{
			countSteps++;
		}
	}
	emitMaskProgressRangeChanged(0, countSteps);
	countSteps = 0;
	QVector<int> result;
	int paramIndex = 0;
	for(int i=0; i<m_subPluginList.size(); i++)
	{
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[i];
		QString subPluginShortName = "";
		if(!item.isNull())
		{
			//TODO refac
			int delta = 3;
			if(i == 1)
			{
				delta = 1;
			}
			if(paramIndex + delta - 1 < paramVector.size())
			{
				std::tr1::shared_ptr<ICEODB_NS::FilterParam> subFilterParam(
					new ICEODB_NS::FilterParam(filterParam->getName(),
					filterParam->getDescription(), filterParam->getFilterType()));
				for(int j=0; j<delta; j++)
				{
					subFilterParam->addMaskParam(createMaskParam(paramVector[paramIndex],
						maskViewVector[paramIndex], lineViewVector[paramIndex]));
					paramIndex++;
				}
				result = item->calcMaskVector(imageBuilder, subFilterParam, result,
					divK, unit);
			}
			subPluginShortName = item->getShortName();
			countSteps++;
			emitMaskProgressValueChanged(countSteps);
			emitMaskProgressTextChanged(subPluginShortName);
		}
	}
	return result;
}

QSharedPointer<QObject> AvgLumFindObjectProcPlugin::calcParamObject(int index,
	QSharedPointer<QObject> param, const QImage& image, const QRect& rect) const
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

QString AvgLumFindObjectProcPlugin::getViewName(bool severalItems) const
{
	if(severalItems)
	{
		return tr("Комбинированные фильтры интенсивности");
	}
	else
	{
		return tr("Комбинированный фильтр интенсивности");
	}
}

QIcon AvgLumFindObjectProcPlugin::getViewIcon() const
{
	return QIcon(":/dvg/iceavglumfindobjectproc/images/filterintensityfindobject.png");
}

bool AvgLumFindObjectProcPlugin::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam)
{
	//TODO refac
	QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[0];
	if(!item.isNull())
	{
		return item->prepareFilter(imageBuilder, filterParam);
	}
	return false;
}

ICEPROC_NS::EditWidget* AvgLumFindObjectProcPlugin::createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface);
}

} // namespace ICEAVGLUMFINDOBJECTPROC_NS

Q_EXPORT_PLUGIN2(avglumfindobjectprocplugin, ICEAVGLUMFINDOBJECTPROC_NS::AvgLumFindObjectProcPlugin);
