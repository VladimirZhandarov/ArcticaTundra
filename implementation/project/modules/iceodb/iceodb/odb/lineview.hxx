#ifndef LINEVIEW_HXX
#define LINEVIEW_HXX

#include "maskview.hxx"

#include <odb/core.hxx>
#include <odb/tr1/memory.hxx>

#include <QtGui/QColor>

#ifdef _MSC_VER
#pragma warning (push)
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

namespace ICEODB_NS
{

#pragma db object pointer(std::tr1::shared_ptr)
class LineView : public MaskView
{
public:
	/**
	* Конструктор
	*/
	LineView();

	/**
	* Конструктор
	* @param name
	* @param color цвет используемый для выделения удовлетворяющих фильтру точек изображения
	* @param useImage
	* @param alphaImageChannel
	* @param lineWidth
	*/
	LineView(const QString& name, const QColor& color, bool useImage = false,
		int alphaImageChannel = 255, const QStringList& filterParamNameList = QStringList(),
		int lineWidth = 1);

	/**
	* Деструктор
	*/
	virtual ~LineView();

	void setLineWidth(int lineWidth);
	int getLineWidth() const;

protected:
	friend class odb::access;

	int lineWidth_;
};

} // namespace ICEODB_NS

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // LINEVIEW_HXX
