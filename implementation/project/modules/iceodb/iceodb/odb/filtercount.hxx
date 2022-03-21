#ifndef FILTERCOUNT_HXX
#define FILTERCOUNT_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>
#include <odb/tr1/lazy-ptr.hxx>

#include <QtCore/QString>

#include "filtertype.hxx"
#include "filtersection.hxx"

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{

/**
* @class FilterCount
* Счетчик фильтров определенного типа в разделе
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterCount : public FilterBaseType
{
public:
	/**
	* Конструктор
	*/
	FilterCount();

	/**
	* Конструктор
	* @param filterType тип фильтра
	*/
	FilterCount(const std::tr1::shared_ptr<ICEODB_NS::FilterType>& filterType,
		odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterSection> parentSection);

	/**
	* Деструктор
	*/
	virtual ~FilterCount();

	unsigned int getId() const;

	void increase();

	void decrease(unsigned int num);

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;	///< Идентификатор БД

	#pragma db column("count")
	unsigned int count_;	///< Счетчик фильтров

	#pragma db column("idFilterType")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterType> filterType_; ///< Тип фильтра

	#pragma db column("idParentSection")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterSection> parentSection_;
		///< Родительский раздел
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // FILTERCOUNT_HXX
