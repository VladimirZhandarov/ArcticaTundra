#ifndef FILTERPARAM_HXX
#define FILTERPARAM_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>
#include <odb/tr1/lazy-ptr.hxx>

#include <QtCore/QString>

#include "maskparam.hxx"
#include "filtersection.hxx"
#include "filtertype.hxx"

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{

/**
* @class FilterParam
* ������
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterParam : public FilterBaseType, public std::tr1::enable_shared_from_this<FilterParam>
{
public:
	/**
	* �����������
	*/
	FilterParam();

	/**
	* �����������
	* @param name ��������
	* @param description ��������
	* @param filterType ��� �������
	*/
	FilterParam(const QString& name, const QString& description,
		const std::tr1::shared_ptr<ICEODB_NS::FilterType>& filterType);

	/**
	* ����������
	*/
	virtual ~FilterParam();

	unsigned int getId() const;
	
	void setName(const QString& name);

	/**
	* �������� ��������
	* @return ��������
	*/
	QString getName() const;

	void setDescription(const QString& description);
	/**
	* �������� ��������
	* @return ��������
	*/
	QString getDescription() const;

	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& getBackgroundView() const;
	void setBackgroundView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundView);

	ICEODB_NS::FilterSection* getParentSection() const;
	void setParentSection(ICEODB_NS::FilterSection* parentSection);

	ICEODB_NS::FilterParam* getParentParam() const;
	void setParentParam(ICEODB_NS::FilterParam* parentParam);

	void addChildParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param);
	int getChildParamCount() const;
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getChildParam(int index);

	void addMaskParam(const std::tr1::shared_ptr<ICEODB_NS::MaskParam>& maskParam);
	int getMaskParamCount() const;
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> getMaskParam(int index);
	std::tr1::shared_ptr<ICEODB_NS::FilterType> getFilterType() const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;			///< ������������� ��

	std::string name_;			///< ��������
	std::string description_;	///< ��������

	#pragma db null column("idBackgroundView")
	std::tr1::shared_ptr<ICEODB_NS::MaskView> backgroundView_;

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterParam> > childParamImplVector_;

	#pragma db inverse(parentParam_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterParam> > childParamVector_;

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::MaskParam> > maskParamImplVector_;

	#pragma db inverse(parentParam_)
	std::vector<odb::tr1::lazy_shared_ptr<ICEODB_NS::MaskParam> > maskParamVector_;

	#pragma db column("idFilterType")
	std::tr1::shared_ptr<ICEODB_NS::FilterType> filterType_; ///< ��� �������

	#pragma db column("idParentSection")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterSection> parentSection_;
		///< ������������ ������

	#pragma db column("idParentParam")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterParam> parentParam_;
		///< ������������ ������
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // FILTERIMAGE_HXX
