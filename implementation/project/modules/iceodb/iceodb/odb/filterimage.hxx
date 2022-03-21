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
* ����������� ��� �������
*/
#pragma db object pointer(std::tr1::shared_ptr)
class FilterImage : public FilterBaseType, public std::tr1::enable_shared_from_this<FilterImage>
{
public:
	/**
	* �����������
	*/
	FilterImage();

	/**
	* �����������
	* @param filename ��� �����
	* @param image �����������
	* @param width ������ �����������
	* @param height ������ �����������
	* @param makeSmaller ������� ����������� �����������
	*/
	FilterImage(const QString& filename, const QByteArray& image,
		int width, int height, int widthStep, int format, bool makeSmaller);

	/**
	* ����������
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
	unsigned int id_;		///< ������������� ��

	std::string filename_;		///< ��� �����

	QByteArray image_;		///< �����������
	int width_;				///< ������ �����������
	int height_;			///< ������ �����������
	int widthStep_;
	int format_;
	bool makeSmaller_;
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // FILTERIMAGE_HXX
