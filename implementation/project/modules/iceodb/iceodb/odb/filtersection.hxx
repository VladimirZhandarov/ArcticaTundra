#ifndef FILTERSECTION_HXX
#define FILTERSECTION_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>
#include <odb/tr1/lazy-ptr.hxx>

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QSharedPointer>

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{
	class FilterParam;
	class FilterCount;

/**
* @class FilterSection
* ������ ��� �������� (������)
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterSection : public FilterBaseType, public std::tr1::enable_shared_from_this<FilterSection>
{
public:
	/**
	* �����������
	*/
	FilterSection();

	/**
	* �����������
	* @param name ��������
	* @param description ��������
	*/
	FilterSection(const QString& name, const QString& description);

	/**
	* ����������
	*/
	virtual ~FilterSection();

	unsigned int getId() const;

	/**
	* �������� ��������
	* @return ��������
	*/
	QString getName() const;

	/**
	* ���������� ��������
	* @param name ��������
	*/
	void setName(const QString& name);

	/**
	* �������� ��������
	* @return ��������
	*/
	QString getDescription() const;

	/**
	* ���������� ��������
	* @param description ��������
	*/
	void setDescription(const QString& description);

	/**
	* �������� ������� �������
	* @return ������� �������
	*/
	int getLevel() const;

	/**
	* TODO: comment
	*/
	ICEODB_NS::FilterSection* getParentSection() const;

	/**
	* TODO: comment
	*/
	void setParentSection(ICEODB_NS::FilterSection* parentSection);

	void addChildSection(ICEODB_NS::FilterSection* section);
	void removeChildSection(ICEODB_NS::FilterSection* section);

	/**
	* �������� ���������� �������� ��������
	* @return ���������� �������� ��������
	*/
	int getChildSectionCount() const;

	/**
	* �������� �������� ������ �� ����������� ������
	* @param index ���������� �����
	* @return �������� ������ (���� =0, �� �� ����� ������� ��� �������)
	*/
	ICEODB_NS::FilterSection* getChildSection(int index);

	void removeChildParam(ICEODB_NS::FilterParam* section);

	/**
	* �������� ���������� �������� ��������
	* @return ���������� �������� ��������
	*/
	int getChildParamCount() const;

	/**
	* �������� �������� ������ �� ����������� ������
	* @param index ���������� �����
	* @return �������� ������ (���� =0, �� �� ����� ������� ��� �������)
	*/
	ICEODB_NS::FilterParam* getChildParam(int index);

	int getFilterCountCount() const;
	ICEODB_NS::FilterCount* getFilterCount(int index);

	int getIndex(ICEODB_NS::FilterSection* filterSection) const;
	int getIndex(ICEODB_NS::FilterParam* filterParam) const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;		///< ������������� ��

	std::string name_;			///< ��������
	std::string description_;	///< ��������
	int level_;					///< ������� �������

	#pragma db null column("idParentSection")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterSection> parentSection_;
		///< ������������ ������

	#pragma db inverse(parentSection_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterSection> > childSectionVector_;
		///< �������� �������

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterSection> > childSectionImplVector_;

	#pragma db inverse(parentSection_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterParam> > childParamVector_;
		///< �������� �������

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterParam> > childParamImplVector_;

	#pragma db inverse(parentSection_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterCount> > filterCountVector_;
		///< ������� ��������

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterCount> > filterCountImplVector_;
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef ODB_COMPILER
#include "filterparam.hxx"
#include "filtercount.hxx"
#endif

#endif //FILTERSECTION_HXX
