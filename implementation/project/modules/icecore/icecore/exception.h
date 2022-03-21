#ifndef ICECORE_EXCEPTION_H
#define ICECORE_EXCEPTION_H

#include "icecoredef.h"

#include <windows.h>
#include <stdio.h>
#include <exception>
#include <string>

namespace ICECORE_NS
{

/**
* @class DataBaseException
* ���������� ��� ������ � ����� ������
*/
class ICECORE_EXPORT DataBaseException : public std::exception
{
public:
	/**
	* �����������
	* @param errorDescription �������� ������
	*/
	DataBaseException(const std::string& errorDescription);
	
	/**
	* ����������
	*/
	virtual ~DataBaseException();

	/**
	* �������� ��������� �������� ������
	* @return ��������� �������� ������
	*/
	const std::string& getErrorDescription() const;
private:
	std::string m_errorDescription;	///< ��������� �������� ������
};

} //namespace ICECORE_NS

#endif // ICECORE_EXCEPTION_H
