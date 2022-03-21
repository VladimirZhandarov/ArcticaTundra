#include "filtersection.hxx"

#include "filtersection-odb.hxx"
#include "filterparam-odb.hxx"

namespace ICEODB_NS
{

FilterSection::FilterSection() : FilterBaseType()
{
	m_typeName = typeid(FilterSection).name();
}

FilterSection::FilterSection(const QString& name, const QString& description) :
	FilterBaseType(), name_(name.toStdString()), description_(description.toStdString()),
	level_(0)
{
	m_typeName = typeid(FilterSection).name();
}

FilterSection::~FilterSection()
{

}

unsigned int FilterSection::getId() const
{
	return id_;
}

QString FilterSection::getName() const
{
	return QString::fromStdString(name_);
}

void FilterSection::setName(const QString& name)
{
	name_ = name.toStdString();
}

QString FilterSection::getDescription() const
{
	return QString::fromStdString(description_);
}

void FilterSection::setDescription(const QString& description)
{
	description_ = description.toStdString();
}

int FilterSection::getLevel() const
{
	return level_;
}

ICEODB_NS::FilterSection* FilterSection::getParentSection() const
{
	return parentSection_.get();
}

void FilterSection::setParentSection(ICEODB_NS::FilterSection* parentSection)
{
	if(parentSection)
	{
		parentSection_ = parentSection->shared_from_this();
		if(parentSection_.get())
		{
			level_ = parentSection_->getLevel()+1;
		}
	}
	else
	{
		parentSection_ = std::tr1::shared_ptr<ICEODB_NS::FilterSection>();
		level_ = 0;
	}
}

void FilterSection::addChildSection(ICEODB_NS::FilterSection* section)
{
	Q_ASSERT(section);
	std::tr1::shared_ptr<ICEODB_NS::FilterSection> sectionPtr = section->shared_from_this();
	childSectionImplVector_.push_back(sectionPtr);
	childSectionVector_.push_back(odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterSection>(sectionPtr));
}

void FilterSection::removeChildSection(ICEODB_NS::FilterSection* section)
{
	Q_ASSERT(section);
	int index = getIndex(section);
	childSectionImplVector_.erase(childSectionImplVector_.begin() + index);
	childSectionVector_.erase(childSectionVector_.begin() + index);
}

int FilterSection::getChildSectionCount() const
{
	return (int)childSectionVector_.size();
}

ICEODB_NS::FilterSection* FilterSection::getChildSection(int index)
{
	if((index < 0) || (index >= childSectionVector_.size()))
	{
		return 0;
	}
	if(childSectionImplVector_.size() != childSectionVector_.size())
	{
		childSectionImplVector_.resize(childSectionVector_.size());
	}
	childSectionImplVector_[index] = childSectionVector_[index].load();
	childSectionImplVector_[index]->setParentSection(this);
	return childSectionImplVector_[index].get();
}

void FilterSection::removeChildParam(ICEODB_NS::FilterParam* param)
{
	Q_ASSERT(param);
	int index = getIndex(param);
	childParamImplVector_.erase(childParamImplVector_.begin() + index - getChildSectionCount());
	childParamVector_.erase(childParamVector_.begin() + index - getChildSectionCount());
}

int FilterSection::getChildParamCount() const
{
	return (int)childParamVector_.size();
}

ICEODB_NS::FilterParam* FilterSection::getChildParam(int index)
{
	if((index < 0) || (index >= childParamVector_.size()))
	{
		return 0;
	}
	if(childParamImplVector_.size() != childParamVector_.size())
	{
		childParamImplVector_.resize(childParamVector_.size());
	}
	childParamImplVector_[index] = childParamVector_[index].load();
	childParamImplVector_[index]->setParentSection(this);
	return childParamImplVector_[index].get();
}

int FilterSection::getFilterCountCount() const
{
	return (int)filterCountVector_.size();
}

ICEODB_NS::FilterCount* FilterSection::getFilterCount(int index)
{
	if((index < 0) || (index >= filterCountVector_.size()))
	{
		return 0;
	}
	if(filterCountImplVector_.size() != filterCountVector_.size())
	{
		filterCountImplVector_.resize(filterCountVector_.size());
	}
	filterCountImplVector_[index] = filterCountVector_[index].load();
	return filterCountImplVector_[index].get();
}

int FilterSection::getIndex(ICEODB_NS::FilterSection* filterSection) const
{
	if(!filterSection)
	{
		return -1;
	}
	for(int i=0; i<childSectionImplVector_.size(); i++)
	{
		if(filterSection == childSectionImplVector_[i].get())
		{
			return i;
		}
	}
	return -1;
}

int FilterSection::getIndex(ICEODB_NS::FilterParam* filterParam) const
{
	if(!filterParam)
	{
		return -1;
	}
	for(int i=0; i<childParamImplVector_.size(); i++)
	{
		if(filterParam == childParamImplVector_[i].get())
		{
			return i + getChildSectionCount();
		}
	}
	return -1;
}

} // namespace ICEODB_NS
