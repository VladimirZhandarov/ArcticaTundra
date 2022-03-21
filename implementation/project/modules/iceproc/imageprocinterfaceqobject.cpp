#include "imageprocinterfaceqobject.h"

namespace ICEPROC_NS
{

ImageProcInterfaceQObject::ImageProcInterfaceQObject(QObject* parent) : QObject(parent)
{

}

ImageProcInterfaceQObject::~ImageProcInterfaceQObject()
{

}

void ImageProcInterfaceQObject::emitMaskProgressRangeChanged(int minimum, int maximum)
{
	emit maskProgressRangeChanged(minimum, maximum);
}

void ImageProcInterfaceQObject::emitMaskProgressValueChanged(int progressValue)
{
	emit maskProgressValueChanged(progressValue);
}

void ImageProcInterfaceQObject::emitMaskProgressTextChanged(const QString& progressText)
{
	emit maskProgressTextChanged(progressText);
}

void ImageProcInterfaceQObject::emitFilterCalculated(int filterNum, const QString& filterType)
{
	emit filterCalculated(filterNum, filterType);
}

} // namespace ICEPROC_NS
