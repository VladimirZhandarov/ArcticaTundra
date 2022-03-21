#ifndef MASKVIEW_HXX
#define MASKVIEW_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>

#include <QtCore/QStringList>
#include <QtGui/QColor>

#include <vector>

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{

#pragma db object pointer(std::tr1::shared_ptr)
class MaskView : public FilterBaseType
{
public:
	/**
	* Конструктор
	*/
	MaskView();

	/**
	* Конструктор
	* @param name
	* @param color цвет используемый для выделения удовлетворяющих фильтру точек изображения
	* @param useImage
	* @param alphaImageChannel
	*/
	MaskView(const QString& name, const QColor& color, bool useImage = false,
		int alphaImageChannel = 255, const QStringList& filterParamNameList =
		QStringList(), int skippedPointCount = 0);

	/**
	* Деструктор
	*/
	virtual ~MaskView();

	void setName(const QString& name);

	/**
	* Получить название
	* @return название
	*/
	QString getName() const;

	void setColor(const QColor& color);
	QColor getColor() const;

	void setUseImage(bool useImage);
	bool isUseImage() const;

	void setAlphaImageChannel(int alphaImageChannel);
	int getAlphaImageChannel() const;

	void setFilterParamNameList(const QStringList& filterParamNameList);
	QStringList getFilterParamNameList() const;

	void setSkippedPointCount(int skippedPointCount);
	int getSkippedPointCount() const;

	bool equal(MaskView* arg) const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;	///< Идентификатор БД

	std::string name_;	///< Название
	int redChannel_;	///< Красный канал цвета
	int greenChannel_;	///< Зеленый канал цвета
	int blueChannel_;	///< Синий канал цвета
	int alphaChannel_;	///< Alpha-канал цвета
	bool useImage_;
	int alphaImageChannel_;

	#pragma db transient
	std::vector<std::string> filterParamNameVector_;

	#pragma db transient
	int skippedPointCount_;
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // MASKVIEW_HXX
