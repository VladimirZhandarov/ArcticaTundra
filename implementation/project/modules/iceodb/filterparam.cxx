#include "filterparam.hxx"

#include "filterparam-odb.hxx"
#include "maskparam-odb.hxx"

namespace ICEODB_NS
{

FilterParam::FilterParam() : FilterBaseType()
{
	m_typeName = typeid(FilterParam).name();
}

FilterParam::FilterParam(const QString& name, const QString& description,
	const std::tr1::shared_ptr<ICEODB_NS::FilterType>& filterType) :
	FilterBaseType(), name_(name.toStdString()), description_(description.toStdString()),
	filterType_(filterType)
{
	m_typeName = typeid(FilterParam).name();
}

FilterParam::~FilterParam()
{

}

unsigned int FilterParam::getId() const
{
	return id_;
}

void FilterParam::setName(const QString& name)
{
	name_ = name.toStdString();
}

QString FilterParam::getName() const
{
	return QString::fromStdString(name_);
}

void FilterParam::setDescription(const QString& description)
{
	description_ = description.toStdString();
}

QString FilterParam::getDescription() const
{
	return QString::fromStdString(description_);
}

const std::tr1::shared_ptr<ICEODB_NS::MaskView>& FilterParam::getBackgroundView() const
{
	return backgroundView_;
}

void FilterParam::setBackgroundView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundView)
{
	backgroundView_ = backgroundView;
}

ICEODB_NS::FilterSection* FilterParam::getParentSection() const
{
	return parentSection_.get();
}

void FilterParam::setParentSection(ICEODB_NS::FilterSection* parentSection)
{
	if(parentSection)
	{
		parentSection_ = parentSection->shared_from_this();
	}
	else
	{
		parentSection_ = std::tr1::shared_ptr<ICEODB_NS::FilterSection>();
	}
}

ICEODB_NS::FilterParam* FilterParam::getParentParam() const
{
	return parentParam_.get();
}

void FilterParam::setParentParam(ICEODB_NS::FilterParam* parentParam)
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

void FilterParam::addChildParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param)
{
	param->setParentSection(0);
	childParamImplVector_.push_back(param);
	childParamVector_.push_back(param);
}

int FilterParam::getChildParamCount() const
{
	return (int)childParamVector_.size();
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterParam::getChildParam(int index)
{
	if((index < 0) || (index >= childParamVector_.size()))
	{
		return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
	if(childParamImplVector_.size() != childParamVector_.size())
	{
		childParamImplVector_.resize(childParamVector_.size());
	}
	childParamImplVector_[index] = childParamVector_[index].load();
	childParamImplVector_[index]->setParentParam(this);
	return childParamImplVector_[index];
}

void FilterParam::addMaskParam(const std::tr1::shared_ptr<ICEODB_NS::MaskParam>& maskParam)
{
	maskParamImplVector_.push_back(maskParam);
	maskParamVector_.push_back(maskParam);
}

int FilterParam::getMaskParamCount() const
{
	return (int)maskParamVector_.size();
}

std::tr1::shared_ptr<ICEODB_NS::MaskParam> FilterParam::getMaskParam(int index)
{
	if((index < 0) || (index >= maskParamVector_.size()))
	{
		return std::tr1::shared_ptr<ICEODB_NS::MaskParam>();
	}
	if(maskParamImplVector_.size() != maskParamVector_.size())
	{
		maskParamImplVector_.resize(maskParamVector_.size());
	}
	maskParamImplVector_[index] = maskParamVector_[index].load();
	return maskParamImplVector_[index];
}

std::tr1::shared_ptr<ICEODB_NS::FilterType> FilterParam::getFilterType() const
{
	return filterType_;
}

} // namespace ICEODB_NS
