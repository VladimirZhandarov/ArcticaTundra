#include "filtertype.hxx"

namespace ICEODB_NS
{

FilterType::FilterType() : FilterBaseType()
{
	m_typeName = typeid(FilterType).name();
}

FilterType::FilterType(const QString& dllName) : FilterBaseType(), dllName_(dllName.toStdString())
{
	m_typeName = typeid(FilterType).name();
}

FilterType::~FilterType()
{

}

QString FilterType::getDllName() const
{
	return QString::fromStdString(dllName_);
}

} // namespace ICEODB_NS
