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
* Раздел для фильтров (группа)
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterSection : public FilterBaseType, public std::tr1::enable_shared_from_this<FilterSection>
{
public:
	/**
	* Конструктор
	*/
	FilterSection();

	/**
	* Конструктор
	* @param name название
	* @param description описание
	*/
	FilterSection(const QString& name, const QString& description);

	/**
	* Деструктор
	*/
	virtual ~FilterSection();

	unsigned int getId() const;

	/**
	* Получить название
	* @return название
	*/
	QString getName() const;

	/**
	* Установить название
	* @param name название
	*/
	void setName(const QString& name);

	/**
	* Получить описание
	* @return описание
	*/
	QString getDescription() const;

	/**
	* Установить описание
	* @param description описание
	*/
	void setDescription(const QString& description);

	/**
	* Получить уровень раздела
	* @return уровень раздела
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
	* Получить количество дочерних разделов
	* @return количество дочерних разделов
	*/
	int getChildSectionCount() const;

	/**
	* Получить дочерний раздел по порядковому номеру
	* @param index порядковый номер
	* @return дочерний раздел (Если =0, то по этому индексу нет раздела)
	*/
	ICEODB_NS::FilterSection* getChildSection(int index);

	void removeChildParam(ICEODB_NS::FilterParam* section);

	/**
	* Получить количество дочерних фильтров
	* @return количество дочерних фильтров
	*/
	int getChildParamCount() const;

	/**
	* Получить дочерний фильтр по порядковому номеру
	* @param index порядковый номер
	* @return дочерний фильтр (Если =0, то по этому индексу нет фильтра)
	*/
	ICEODB_NS::FilterParam* getChildParam(int index);

	int getFilterCountCount() const;
	ICEODB_NS::FilterCount* getFilterCount(int index);

	int getIndex(ICEODB_NS::FilterSection* filterSection) const;
	int getIndex(ICEODB_NS::FilterParam* filterParam) const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;		///< Идентификатор БД

	std::string name_;			///< Название
	std::string description_;	///< Описание
	int level_;					///< Уровень раздела

	#pragma db null column("idParentSection")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterSection> parentSection_;
		///< Родительский раздел

	#pragma db inverse(parentSection_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterSection> > childSectionVector_;
		///< Дочерние разделы

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterSection> > childSectionImplVector_;

	#pragma db inverse(parentSection_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterParam> > childParamVector_;
		///< Дочерние фильтры

	#pragma db transient
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterParam> > childParamImplVector_;

	#pragma db inverse(parentSection_)
	std::vector<odb::tr1::lazy_weak_ptr<ICEODB_NS::FilterCount> > filterCountVector_;
		///< Счетчик фильтров

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
