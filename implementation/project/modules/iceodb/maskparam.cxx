#include "maskparam.hxx"

#include "filterparam.hxx"

namespace ICEODB_NS
{

MaskParam::MaskParam() : FilterBaseType()
{

}

MaskParam::MaskParam(const QString& name, const QString& jsonParam, const QColor& color) :
	FilterBaseType(), jsonParam_(jsonParam.toStdString())
{
	maskView_ = std::tr1::shared_ptr<ICEODB_NS::MaskView>(new ICEODB_NS::MaskView(name, color));
}

MaskParam::~MaskParam()
{

}

QString MaskParam::getJSonParam() const
{
	return QString::fromStdString(jsonParam_);
}

void MaskParam::setJSonParam(const QString& jsonParam)
{
	jsonParam_ = jsonParam.toStdString();
}

const std::tr1::shared_ptr<ICEODB_NS::FilterImage>& MaskParam::getFilterImage() const
{
	return filterImage_;
}

void MaskParam::setFilterImage(const std::tr1::shared_ptr<ICEODB_NS::FilterImage>& filterImage)
{
	filterImage_ = filterImage;
}

const std::tr1::shared_ptr<ICEODB_NS::LineView>& MaskParam::getLineView() const
{
	return lineView_;
}

void MaskParam::setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	lineView_ = lineView;
}

const std::tr1::shared_ptr<ICEODB_NS::MaskView>& MaskParam::getMaskView() const
{
	return maskView_;
}

void MaskParam::setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	maskView_ = maskView;
}

ICEODB_NS::FilterParam* MaskParam::getParentFilterParam() const
{
	return parentParam_.get();
}

void MaskParam::setParentFilterParam(ICEODB_NS::FilterParam* parentParam)
{
	if(parentParam)
	{
		parentParam_ = parentParam->shared_from_this();
	}
	else
	{
		parentParam_ = std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
}

} // namespace ICEODB_NS
