#include "filtercount.hxx"

namespace ICEODB_NS
{

FilterCount::FilterCount() : FilterBaseType()
{
	m_typeName = typeid(FilterCount).name();
}

FilterCount::FilterCount(const std::tr1::shared_ptr<ICEODB_NS::FilterType>& filterType,
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterSection> parentSection) : FilterBaseType(),
	filterType_(filterType), parentSection_(parentSection), count_(0)
{
	m_typeName = typeid(FilterCount).name();
}

FilterCount::~FilterCount()
{

}

unsigned int FilterCount::getId() const
{
	return id_;
}

void FilterCount::increase()
{
	count_++;
}

void FilterCount::decrease(unsigned int num)
{
	count_ -= num;
}

} // namespace ICEODB_NS
