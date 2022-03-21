#ifndef MASKPARAM_HXX
#define MASKPARAM_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>
#include <odb/tr1/lazy-ptr.hxx>

#include <QtCore/QString>

#include "filterimage.hxx"
#include "maskview.hxx"
#include "lineview.hxx"

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{
	class FilterParam;

#pragma db object pointer(std::tr1::shared_ptr)
class MaskParam : public FilterBaseType, public std::tr1::enable_shared_from_this<MaskParam>
{
public:
	/**
	* Конструктор
	*/
	MaskParam();

	/**
	* Конструктор
	* @param name, 
	* @param jsonParam параметры фильтра
	* @param color цвет используемый для выделения удовлетворяющих фильтру точек изображения
	*/
	MaskParam(const QString& name, const QString& jsonParam, const QColor& color);

	/**
	* Деструктор
	*/
	virtual ~MaskParam();

	QString getJSonParam() const;
	void setJSonParam(const QString& jsonParam);

	const std::tr1::shared_ptr<ICEODB_NS::FilterImage>& getFilterImage() const;
	void setFilterImage(const std::tr1::shared_ptr<ICEODB_NS::FilterImage>& filterImage);

	const std::tr1::shared_ptr<ICEODB_NS::LineView>& getLineView() const;
	void setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);

	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& getMaskView() const;
	void setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);

	/**
	* TODO: comment
	*/
	ICEODB_NS::FilterParam* getParentFilterParam() const;

	/**
	* TODO: comment
	*/
	void setParentFilterParam(ICEODB_NS::FilterParam* parentParam);

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;		///< Идентификатор БД

	std::string jsonParam_;		///< Параметры фильтра

	#pragma db null column("idFilterImage")
	std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage_;	///< Изображение

	#pragma db null column("idLineView")
	std::tr1::shared_ptr<ICEODB_NS::LineView> lineView_;

	#pragma db null column("idMaskView")
	std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView_;

	#pragma db column("idParentFilterParam")
	odb::tr1::lazy_shared_ptr<ICEODB_NS::FilterParam> parentParam_;	///< Родительский фильтр
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef ODB_COMPILER
#include "filterparam.hxx"
#endif

#endif // MASKPARAM_HXX
