#include "imageprocinterface.h"

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning(disable: 4985)
#endif
#include <opencv/cv.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include <iceimage/imagebuilder.h>

#include <iceodb/odb/filterparam.hxx>

#include <cuda_runtime.h>
#include <helper_cuda.h>

#include <QJson/QObjectHelper>
#include <QJson/Serializer>
#include <QJson/Parser>

#include "editwidget.h"
#include "imageprocinterfaceqobject.h"

namespace ICEPROC_NS
{

ImageProcInterface::ImageProcInterface() : m_dllName("")
{
	m_qObject = new ImageProcInterfaceQObject();
}

ImageProcInterface::~ImageProcInterface()
{

}

void ImageProcInterface::init()
{

}

void ImageProcInterface::setDllName(const QString& dllName)
{
	m_dllName = dllName;
}

const QString& ImageProcInterface::getDllName() const
{
	return m_dllName;
}

EditWidget* ImageProcInterface::getEditWidget(ImageWindowInterface* imageWindowInterface)
{
	EditWidget* widget = createEditWidget(imageWindowInterface);
	fillEditWidget(widget);
	return widget;
}

int ImageProcInterface::calcTotalSteps(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& /*imageBuilder*/,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& /*filterParam*/)
{
	return 0;
}

bool ImageProcInterface::prepAndCalcMaskVector(
	const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, double divK,
	int unit)
{
	if(!prepareFilter(imageBuilder, filterParam))
	{
		return false;
	}
	calcMaskVector(imageBuilder, filterParam, QVector<int>(), divK, unit);
	return true;
}

QSharedPointer<QObject> ImageProcInterface::calcParamObject(int /*index*/, QSharedPointer<QObject> /*param*/,
	const QImage& /*image*/, const QRect& /*rect*/) const
{
	return QSharedPointer<QObject>();
}

QString ImageProcInterface::getCudaDeviceProperties(int devId)
{
	cudaDeviceProp deviceProp;
	if(devId >= 0)
	{
		devId = gpuDeviceInit(devId);
	}
	else
	{
		devId = gpuGetMaxGflopsDeviceId();
	}
	QString result = "";
	if(devId >= 0)
	{
		checkCudaErrors(cudaSetDevice(devId));
		checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devId));
		result = QObject::tr("Графический ускоритель %1: \"%2\" имеет мультипроцессоров: %3 , Вычисление %4.%5").arg(devId).
			arg(deviceProp.name).arg(deviceProp.multiProcessorCount).arg(deviceProp.major).arg(deviceProp.minor);
	}
	return result;
}

QSharedPointer<QObject> ImageProcInterface::fromJSonString(int index, const QString& jsonString) const
{
	QSharedPointer<QObject> result;
	QJson::Parser parser;
	bool ok;
	QByteArray jsonArray = QByteArray().append(jsonString);
	QVariantMap paramVariant = parser.parse(jsonArray, &ok).toMap();
	if(!ok)
	{
		return result;
	}
	result = createParamObject(index);
	Q_ASSERT(!result.isNull());
	QJson::QObjectHelper::qvariant2qobject(paramVariant, result.data());
	return result;
}

QString ImageProcInterface::toJSonString(const QSharedPointer<QObject>& param) const
{
	QVariantMap paramVariant = QJson::QObjectHelper::qobject2qvariant(param.data());
	if(!paramVariant.isEmpty())
	{
		QJson::Serializer serializer;
		bool ok;
		QByteArray json = serializer.serialize(paramVariant, &ok);
		if(ok)
		{
			return json;
		}
	}
	return QString();
}

bool ImageProcInterface::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& /*imageBuilder*/,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& /*filterParam*/)
{
	return true;
}

ImageProcInterfaceQObject* ImageProcInterface::getQObject() const
{
	return m_qObject;
}

EditWidget* ImageProcInterface::createEditWidget(ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface, true);
}

void ImageProcInterface::fillEditWidget(EditWidget* /*widget*/) const
{

}

QVector<QSharedPointer<QObject> > ImageProcInterface::getParamVector(
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const
{
	Q_ASSERT(filterParam.get());
	QVector<QSharedPointer<QObject> > result;
	for(int i=0; i<filterParam->getMaskParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterParam->getMaskParam(i);
		result.append(fromJSonString(i, maskParam->getJSonParam()));
	}
	return result;
}

QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > ImageProcInterface::getMaskViewVector(
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const
{
	Q_ASSERT(filterParam.get());
	QStringList filterParamNameList;
	ICEODB_NS::FilterParam* currentFilterParam = filterParam.get();
	do
	{
		filterParamNameList.prepend("(" + getShortName() + ") " + currentFilterParam->getName());
		currentFilterParam = currentFilterParam->getParentParam();
	}
	while(currentFilterParam);
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > result;
	for(int i=0; i<filterParam->getMaskParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterParam->getMaskParam(i);
		std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
		if(maskView.get())
		{
			maskView->setFilterParamNameList(filterParamNameList);
		}
		result.append(maskView);
	}
	return result;
}

QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > ImageProcInterface::getLineViewVector(
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const
{
	Q_ASSERT(filterParam.get());
	QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > result;
	for(int i=0; i<filterParam->getMaskParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterParam->getMaskParam(i);
		result.append(maskParam->getLineView());
	}
	return result;
}

std::tr1::shared_ptr<ICEODB_NS::MaskParam> ImageProcInterface::createMaskParam(
	const QSharedPointer<QObject>& param,
	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView,
	const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> result(new ICEODB_NS::MaskParam());
	if(!param.isNull())
	{
		result->setJSonParam(toJSonString(param));
	}
	if(maskView.get())
	{
		result->setMaskView(maskView);
	}
	if(lineView.get())
	{
		result->setLineView(lineView);
	}
	return result;
}

void ImageProcInterface::emitMaskProgressRangeChanged(int minimum, int maximum)
{
	Q_ASSERT(m_qObject);
	m_qObject->emitMaskProgressRangeChanged(minimum, maximum);
}

void ImageProcInterface::emitMaskProgressValueChanged(int progressValue)
{
	Q_ASSERT(m_qObject);
	m_qObject->emitMaskProgressValueChanged(progressValue);
}

void ImageProcInterface::emitMaskProgressTextChanged(const QString& progressText)
{
	Q_ASSERT(m_qObject);
	m_qObject->emitMaskProgressTextChanged(progressText);
}

void ImageProcInterface::emitFilterCalculated(int filterNum, const QString& filterType)
{
	Q_ASSERT(m_qObject);
	m_qObject->emitFilterCalculated(filterNum, filterType);
}


} // namespace ICEPROC_NS
