#include "exception.h"

namespace ICECORE_NS
{

DataBaseException::DataBaseException(const std::string& errorDescription) :
	m_errorDescription(errorDescription)
{
	
}

DataBaseException::~DataBaseException()
{

}

const std::string& DataBaseException::getErrorDescription() const
{
	return m_errorDescription;
}

} //namespace ICECORE_NS
