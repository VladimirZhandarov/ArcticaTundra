#include "findobjectprocplugin.h"

#include <QtCore/QtPlugin>

#include <QtGui/QProgressDialog>

#include <iceimage/imagebuilder.h>
#include <iceimage/matfunction.h>

#include <iceodb/odb/filterparam.hxx>

#include "editwidget.h"
#include "param.h"

#include <opencv/cv.h>

namespace ICEFINDOBJECTPROC_NS
{

FindObjectProcPlugin::FindObjectProcPlugin() : ICEPROC_NS::ImageProcInterface()
{

}

FindObjectProcPlugin::~FindObjectProcPlugin()
{

}

QSharedPointer<QObject> FindObjectProcPlugin::createParamObject(int /*index*/) const
{
	return QSharedPointer<Param>(new Param());
}

QString FindObjectProcPlugin::getShortName() const
{
	return tr("ФО");
}

QVector<int> FindObjectProcPlugin::calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, const QVector<int>& indexVector,
	double divK, int unit)
{
	Q_ASSERT(!imageBuilder.isNull() && filterParam.get());
	emitFilterCalculated(0, getShortName());
	QVector<QSharedPointer<QObject> > paramVector = getParamVector(filterParam);
	QString filterName = filterParam->getName();
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > maskViewVector = getMaskViewVector(
		filterParam);
	QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > lineViewVector = getLineViewVector(
		filterParam);
	Q_ASSERT(paramVector.size() == maskViewVector.size() && paramVector.size() == lineViewVector.size());
	int countSteps = 0;
	//TOO refac
	for(int i=0; i<paramVector.size(); i++)
	{
		Param* param = qobject_cast<Param*>(paramVector[i].data());
		Q_ASSERT(param);
		if(param->isMaskUsed())
		{
			countSteps++;
		}
	}
	emitMaskProgressRangeChanged(0, countSteps);
	countSteps = 0;
	QVector<int> result;
	QVector<int> indexVectorWork;
	if(indexVector.isEmpty())
	{
		for(int i=0; i<imageBuilder->getContourItemModelListSize(); i++)
		{
			if(!imageBuilder->getContourItemModel(i))
			{
				indexVectorWork.append(i);
			}
		}
	}
	else
	{
		indexVectorWork = indexVector;
	}
	for(int i=0; i<paramVector.size(); i++)
	{
		Param* param = qobject_cast<Param*>(paramVector[i].data());
		Q_ASSERT(param);
		if(!param->isMaskUsed())
		{
			continue;
		}
		int kErodeValue = param->getKErode();
		int kDilateValue = param->getKDilate();
		bool erodeFirstValue = param->isErodeFirst();

		if(i < indexVectorWork.size())
		{
			int maskIndex = indexVectorWork[i];
			cv::Mat srcImage = imageBuilder->getMatMaskImage(maskIndex).clone();
			cv::Mat workImage = srcImage.clone();
			if(!workImage.empty())
			{
				cv::Mat matErodeKernel = getStructuringElement(CV_SHAPE_ELLIPSE,
					cv::Size(kErodeValue, kErodeValue), cv::Point(-1, -1));
				if(erodeFirstValue)
				{
					cv::erode(workImage, workImage, matErodeKernel);
				}
				cv::Mat matDilateKernel = getStructuringElement(CV_SHAPE_ELLIPSE,
					cv::Size(kDilateValue, kDilateValue), cv::Point(-1, -1));
				cv::dilate(workImage, workImage, matDilateKernel);
				if(!erodeFirstValue)
				{
					cv::erode(workImage, workImage, matErodeKernel);
				}

				std::vector<std::vector<cv::Point> > contours;
				std::vector<cv::Vec4i> hierarchy;
				cv::findContours(workImage, contours, hierarchy, CV_RETR_CCOMP,
					CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
				int minArea = param->getMinArea();
				if(!param->isCheckArea())
				{
					minArea = 0;
				}
				int minPerimeter = param->getMinPerimeter();
				if(!param->isCheckPerimeter())
				{
					minPerimeter = 0;
				}
				double minKForm = param->getMinKForm();
				if(!param->isCheckKForm())
				{
					minKForm = 0.0;
				}
				else
				{
					minPerimeter = 0;
				}
				imageBuilder->setContours(maskIndex, contours, srcImage, filterName,
					maskViewVector[i], lineViewVector[i], minArea, minPerimeter,
					minKForm, divK, unit);
			}
		}
		countSteps++;
		emitMaskProgressValueChanged(countSteps);
		emitMaskProgressTextChanged(getShortName());
	}
	imageBuilder->startCalc();
	return result;
}

QString FindObjectProcPlugin::getViewName(bool severalItems) const
{
	if(severalItems)
	{
		return tr("Фильтры объектов");
	}
	else
	{
		return tr("Фильтр объектов");
	}
}

QIcon FindObjectProcPlugin::getViewIcon() const
{
	return QIcon(":/dvg/icefindobjectproc/images/filterobject.png");
}

ICEPROC_NS::EditWidget* FindObjectProcPlugin::createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface);
}

} // namespace ICEFINDOBJECTPROC_NS

Q_EXPORT_PLUGIN2(findobjectprocplugin, ICEFINDOBJECTPROC_NS::FindObjectProcPlugin);
