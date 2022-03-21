//==============================================================================
// Название:        Элемент отображения изображения
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: shchav $
// Создан:    2011.11.08
// Изменен:   $Date: 2013-04-19 18:46:52 +0400 (РџС‚, 19 Р°РїСЂ 2013) $
// Модуль:    iceapp
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: IceQwtPlotRasterItem.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "iceqwtplotrasteritem.h"

#include "iceqwtplot.h"
#include "iceunitratioqwttransform.h"

#include <iceimage/imagebuilder.h>

#include <qwt_scale_engine.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>

namespace ICEQWT_NS
{

class IceQwtPlotRasterItem::PrivateData
{
public:
	PrivateData()
	{

	}

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder;
	QPointF origin;
};

//! Конструктор
IceQwtPlotRasterItem::IceQwtPlotRasterItem(const QString& title) : QwtPlotRasterItem(title)
{
	d_data = new PrivateData;
	setRenderHint(RenderAntialiased, true);
	setPaintAttribute(PaintInDeviceResolution, true);
	QwtPainter::setRoundingAlignment(false);
}

//! Деструктор
IceQwtPlotRasterItem::~IceQwtPlotRasterItem()
{
	delete d_data;
}

/*!
  \brief Установить построитель изображения
*/
void IceQwtPlotRasterItem::setImageBuilder(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	d_data->imageBuilder = imageBuilder;
}

void IceQwtPlotRasterItem::setScaleRegionRect() const
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	QwtInterval intervalX = plt->axisInterval(QwtPlot::xTop);
	QwtInterval intervalImageX = interval(Qt::XAxis);
	intervalX &= intervalImageX;

	QwtInterval intervalY = plt->axisInterval(QwtPlot::yLeft).normalized();
	QwtInterval intervalImageY = interval(Qt::YAxis);
	intervalY &= intervalImageY;

	QRectF rect(intervalX.minValue(), intervalY.minValue(), intervalX.width(),
		intervalY.width());
	rect = plt->transform(rect);
	d_data->imageBuilder->setScaleRegionRect(rect.toRect());
}

/*!
  \return Возвращает построитель изображения
*/
//const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& IceQwtPlotRasterItem::getImageBuilder() const
//{
//	return d_data->imageBuilder;
//}

/*!
  \brief Получить ширину изображения в размерности шкал
*/
double IceQwtPlotRasterItem::width() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imgBldr = d_data->imageBuilder;
	if(imgBldr.isNull())
	{
		return 0;
	}
	double widthInPK = imgBldr->getImageSizeWidth();
	// Берем первую установленную трансформацию от первой попавшейся горизонтальной оси.
	// Таким образом мы определяем ширину изображения при заданном в трансформации 
	// коэффициенте масштабирования. Мы не можем взять ширину отмасштабированного изображения
	// из imgBldr - так как она появиться только после масштабирования изображения, а данный
	// метод может использоваться еще до масштабирования (например в magnifer), когда надо 
	// "предугадывать" ширину отмасштабированного изображения. Ширина изображения в размерности
	// шкал не зависит от масштаба, поэтому мы берем только трансформацию, где "зашиты" единицы
	// измерения
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	IceUnitRatioQwtTransform* transform = (IceUnitRatioQwtTransform*)plt->axisScaleEngine(QwtPlot::xTop)->transformation();
	if(transform)
	{
		return transform->invTransform(widthInPK); 
	}
	else
	{
		return widthInPK; 
	}
}

/*!
  \brief Получить высоту изображения в размерности шкал
*/
double IceQwtPlotRasterItem::height() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imgBldr = d_data->imageBuilder;
	if(imgBldr.isNull())
	{
		return 0;
	}
	double heightInPK = imgBldr->getImageSizeHeight();

	// см. комментарии в width()
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	IceUnitRatioQwtTransform* transform = (IceUnitRatioQwtTransform*)plt->axisScaleEngine(QwtPlot::yLeft)->transformation();
	if(transform)
	{
		return transform->invTransform(heightInPK);
	}
	else
	{
		return heightInPK;
	}
}

/*!
  \brief Получить коэффициента масштабирования оси
*/
double IceQwtPlotRasterItem::getScaleFactory(Qt::Axis axis) const
{
	switch(axis)
	{
		case Qt::XAxis:
		{
			if(d_data->imageBuilder.isNull())
			{
				return 0;
			}
			return d_data->imageBuilder->getScaleFactorX();
		}
		case Qt::YAxis:
		{
			if(d_data->imageBuilder.isNull())
			{
				return 0;
			}
			return d_data->imageBuilder->getScaleFactorY();
		}
		default:
		{
			return 0;
		}
	}
}

QwtInterval IceQwtPlotRasterItem::interval(Qt::Axis axis) const
{
	switch(axis)
	{
		case Qt::XAxis:
		{
			return QwtInterval(d_data->origin.x(), d_data->origin.x() + width());
		}
		case Qt::YAxis:
		{
			return QwtInterval(d_data->origin.y(), d_data->origin.y() + height());
		}
		default:
		{
			return QwtInterval();
		}
	}
}

/*!
  \brief Установить начало координат
*/
void IceQwtPlotRasterItem::setOrigin(const QPointF& origin)
{
	d_data->origin = origin;
}

/*!
  \brief Получить начало координат
*/
const QPointF& IceQwtPlotRasterItem::origin() const
{
	return d_data->origin;
}

double IceQwtPlotRasterItem::origin(Qt::Axis axis) const
{
	if(Qt::XAxis == axis)
	{
		return d_data->origin.x();
	}
	else
	{
		return d_data->origin.y();
	}
}

QImage IceQwtPlotRasterItem::renderImage(const QwtScaleMap& xMap, const QwtScaleMap& yMap,
	const QRectF& /*area*/, const QSize& /*imageSize*/) const
{
	/*
	Здесь мы уже получаем преобразователи, адаптированный под отмасштабированное изображение и
	можем их использовать напрямую для формирования нужного фрагмента изображения 
	из отмасштабированного изображения
	*/
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = d_data->imageBuilder;
	Q_ASSERT(!imageBuilder.isNull());
	return imageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::SCALED_RESULT_IMAGE,
		QRect(qRound(xMap.p1()), qRound(yMap.p1()), qRound(xMap.pDist() + 1),
		qRound(yMap.pDist() + 1)));
}

QwtScaleMap IceQwtPlotRasterItem::imageMap(Qt::Orientation orientation, 
	const QwtScaleMap& map, const QRectF& area, const QSize& imageSize, 
	double pixelSize) const
{
	Q_ASSERT(pixelSize == 0);
	QwtScaleMap mmap = QwtPlotRasterItem::imageMap(orientation, map, area, imageSize, pixelSize);

	const IceQwtPlot* plt = (IceQwtPlot*)plot();
	QwtPlot::Axis axisId = Qt::Horizontal == orientation ? QwtPlot::xTop : QwtPlot::yLeft;
	double d1 = plt->transform(axisId, mmap.s1());
	double d2 = plt->transform(axisId, mmap.s2());
	// Настраиваем преобразователь под будущий отображаемый фрагмент отмасштабированного изображения
	mmap.setPaintInterval(d1 * getScaleFactory(Qt::XAxis), d2 * getScaleFactory(Qt::XAxis));

	/*
	Данная ф-ия вызывается как для вертикальной оси так и для горизонтальной оси
	отдельно. При этом алгоритм формирования преобразователя для каждой из этих
	осей одинаковый, конечно с учетом того, что начало координат оси Y находиться
	в верхнем левом углу (изначально он находиться в нижнем левом углу). Такую 
	настройку оси мы изначально делаем в QwtPlot::init()
	*/
	return mmap;
}

} // namespace ICEQWT_NS
