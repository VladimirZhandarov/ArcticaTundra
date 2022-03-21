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
* ��� �������
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterType : public FilterBaseType
{
public:
	/**
	* �����������
	*/
	FilterType();

	/**
	* �����������
	* @param dllName ��� dll-����� � ���������� ��������� �����������
	*/
	FilterType(const QString& dllName);

	/**
	* ����������
	*/
	virtual ~FilterType();

	QString getDllName() const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;		///< ������������� ��

	std::string dllName_;	///< ��� dll-����� � ���������� ��������� �����������
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // FILTERTYPE_HXX
