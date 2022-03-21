#ifndef FILTERTYPE_HXX
#define FILTERTYPE_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>

#include <QtCore/QString>

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{

/**
* @class FilterType
* Тип фильтра
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterType : public FilterBaseType
{
public:
	/**
	* Конструктор
	*/
	FilterType();

	/**
	* Конструктор
	* @param dllName имя dll-файла с алгоритмом обработки изображения
	*/
	FilterType(const QString& dllName);

	/**
	* Деструктор
	*/
	virtual ~FilterType();

	QString getDllName() const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;		///< Идентификатор БД

	std::string dllName_;	///< Имя dll-файла с алгоритмом обработки изображения
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // FILTERTYPE_HXX
