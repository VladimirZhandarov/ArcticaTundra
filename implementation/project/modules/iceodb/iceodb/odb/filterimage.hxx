#ifndef FILTERIMAGE_HXX
#define FILTERIMAGE_HXX

#include "filterbasetype.h"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>

#include <QtCore/QByteArray>

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{

/**
* @class FilterImage
* Изображение для фильтра
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterImage : public FilterBaseType, public std::tr1::enable_shared_from_this<FilterImage>
{
public:
	/**
	* Конструктор
	*/
	FilterImage();

	/**
	* Конструктор
	* @param filename имя файлы
	* @param image изображение
	* @param width ширина изображения
	* @param height высота изображения
	* @param makeSmaller признак уменьшеного изображения
	*/
	FilterImage(const QString& filename, const QByteArray& image,
		int width, int height, int widthStep, int format, bool makeSmaller);

	/**
	* Деструктор
	*/
	virtual ~FilterImage();

	void setId(unsigned int id);
	unsigned int getId() const;

	QString getFilename() const;
	const QByteArray& getImage() const;
	int getWidth() const;
	int getHeight() const;
	int getWidthStep() const;
	int getFormat() const;
	bool isMakeSmaller() const;

protected:
	friend class odb::access;

	#pragma db id auto
	unsigned int id_;		///< Идентификатор БД

	std::string filename_;		///< Имя файлы

	QByteArray image_;		///< Изображение
	int width_;				///< Ширина изображения
	int height_;			///< Высота изображения
	int widthStep_;
	int format_;
	bool makeSmaller_;
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // FILTERIMAGE_HXX
