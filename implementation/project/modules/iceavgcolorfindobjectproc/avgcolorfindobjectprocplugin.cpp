#include "avgcolorfindobjectprocplugin.h"

#include <QtCore/QtPlugin>

#include <iceimage/imagebuilder.h>

#include "editwidget.h"

#include <iceavgcolorproc/param.h>
#include <icefindobjectproc/param.h>

#include <iceavgcolorproc/avgcolorprocplugin.h>
#include <icefindobjectproc/findobjectprocplugin.h>

#include <iceodb/odb/filterparam.hxx>

#include <opencv/cv.h>

namespace ICEAVGCOLORFINDOBJECTPROC_NS
{

AvgColorFindObjectProcPlugin::AvgColorFindObjectProcPlugin() : ICEPROC_NS::ImageProcInterface()
{
	m_subPluginList.append(QSharedPointer<ICEPROC_NS::ImageProcInterface>(
		new ICEAVGCOLORPROC_NS::AvgColorProcPlugin()));
	m_subPluginList.append(QSharedPointer<ICEPROC_NS::ImageProcInterface>(
		new ICEFINDOBJECTPROC_NS::FindObjectProcPlugin()));
}

AvgColorFindObjectProcPlugin::~AvgColorFindObjectProcPlugin()
{

}

QSharedPointer<QObject> AvgColorFindObjectProcPlugin::createParamObject(int index) const
{
	if(index >= 0 && index < m_subPluginList.size())
	{
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[index];
		if(!item.isNull())
		{
			return item->createParamObject(0);
		}
	}
	return QSharedPointer<QObject>();
}

QString AvgColorFindObjectProcPlugin::getShortName() const
{
	return tr("ФСК");
}

QVector<int> AvgColorFindObjectProcPlugin::calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
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
		QString subPluginShortName = "";
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[i];
		if(!item.isNull())
		{
			//TODO refac
			std::tr1::shared_ptr<ICEODB_NS::FilterParam> subFilterParam(
				new ICEODB_NS::FilterParam(filterParam->getName(),
				filterParam->getDescription(), filterParam->getFilterType()));
			if(paramIndex < paramVector.size())
			{
				subFilterParam->addMaskParam(createMaskParam(paramVector[paramIndex],
					maskViewVector[paramIndex], lineViewVector[paramIndex]));
				result = item->calcMaskVector(imageBuilder, subFilterParam, result,
					divK, unit);
				paramIndex++;
			}
			subPluginShortName = item->getShortName();
			countSteps++;
			emitMaskProgressValueChanged(countSteps);
			emitMaskProgressTextChanged(subPluginShortName);
		}
	}
	return result;
}

QSharedPointer<QObject> AvgColorFindObjectProcPlugin::calcParamObject(int index,
	QSharedPointer<QObject> param, const QImage& image, const QRect& rect) const
{
	if(index >= 0 && index < m_subPluginList.size())
	{
		QSharedPointer<ICEPROC_NS::ImageProcInterface> item = m_subPluginList[index];
		if(!item.isNull())
		{
			return item->calcParamObject(index, param, image, rect);
		}
	}
	return QSharedPointer<QObject>();
}

QString AvgColorFindObjectProcPlugin::getViewName(bool severalItems) const
{
	if(severalItems)
	{
		return tr("Комбинированные спектральные фильтры");
	}
	else
	{
		return tr("Комбинированный спектральный фильтр");
	}
}

QIcon AvgColorFindObjectProcPlugin::getViewIcon() const
{
	return QIcon(":/dvg/iceavgcolorfindobjectproc/images/filtercolorfindobject.png");
}

ICEPROC_NS::EditWidget* AvgColorFindObjectProcPlugin::createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface);
}

} // namespace ICEAVGCOLORFINDOBJECTPROC_NS

Q_EXPORT_PLUGIN2(avgcolorfindobjectprocplugin, ICEAVGCOLORFINDOBJECTPROC_NS::AvgColorFindObjectProcPlugin);
