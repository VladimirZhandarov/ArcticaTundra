#include "filterbasetype.h"

namespace ICEODB_NS
{

FilterBaseType::FilterBaseType()
{

}

FilterBaseType::~FilterBaseType()
{

}

const QString& FilterBaseType::getTypeName() const
{
	return m_typeName;
}

} // namespace ICEODB_NS
