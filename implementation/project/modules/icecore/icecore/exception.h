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
* Исключение при работе с базой данных
*/
class ICECORE_EXPORT DataBaseException : public std::exception
{
public:
	/**
	* Конструктор
	* @param errorDescription описание ошибки
	*/
	DataBaseException(const std::string& errorDescription);
	
	/**
	* Деструктор
	*/
	virtual ~DataBaseException();

	/**
	* Получить текстовое описание ошибки
	* @return текстовое описание ошибки
	*/
	const std::string& getErrorDescription() const;
private:
	std::string m_errorDescription;	///< Текстовое описание ошибки
};

} //namespace ICECORE_NS

#endif // ICECORE_EXCEPTION_H
