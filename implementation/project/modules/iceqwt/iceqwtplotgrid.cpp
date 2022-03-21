//==============================================================================
// Название:        ---
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: shchav $
// Создан:    2011.11.08
// Изменен:   $Date: 2013-04-19 18:46:52 +0400 (РџС‚, 19 Р°РїСЂ 2013) $
// Модуль:    iceqwt
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: iceqwtplotgrid.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "iceqwtplotgrid.h"

#include "iceqwtplot.h"
#include "iceqwtplotrasteritem.h"
#include "iceunitratioqwttransform.h"

#include <qwt_scale_engine.h>

namespace ICEQWT_NS
{

class IceQwtPlotGrid::PrivateData
{
public:
	PrivateData(): hStepPixel(100), vStepPixel(100), hStepMetric(200), vStepMetric(200),
		isOnLine(true), isShow800(false)
	{

	}

	// Шаг сетки в пикселях
	unsigned int hStepPixel; // по горизонтали
	unsigned int vStepPixel; // по вертикали

	// Шаг сетки в метриской системе
	double hStepMetric; // по горизонтали
	double vStepMetric; // по вертикали

	bool isOnLine; // По линейке
	bool isShow800; // Показать сетку пикселей с масштабом 800% и выше
};

IceQwtPlotGrid::IceQwtPlotGrid()
{
	d_data = new PrivateData;
	setItemInterest(QwtPlotItem::ScaleInterest, true);
}

IceQwtPlotGrid::~IceQwtPlotGrid()
{
	delete d_data;
}

void IceQwtPlotGrid::setHStepPixel(unsigned int hStepPixel)
{
	d_data->hStepPixel = hStepPixel;
}

unsigned int IceQwtPlotGrid::hStepPixel() const
{
	return d_data->hStepPixel;
}

void IceQwtPlotGrid::setVStepPixel(unsigned int vStepPixel)
{
	d_data->vStepPixel = vStepPixel;
}

unsigned int IceQwtPlotGrid::vStepPixel() const
{
	return d_data->vStepPixel;
}

void IceQwtPlotGrid::setHStepMetric(double hStepMetric)
{
	d_data->hStepMetric = hStepMetric;
}

double IceQwtPlotGrid::hStepMetric() const
{
	return  d_data->hStepMetric;
}

void IceQwtPlotGrid::setVStepMetric(double vStepMetric)
{
	d_data->vStepMetric = vStepMetric;
}

double IceQwtPlotGrid::vStepMetric() const
{
	return  d_data->vStepMetric;
}

void IceQwtPlotGrid::setIsOnLine(bool isOnLine)
{
	d_data->isOnLine = isOnLine;
}

bool IceQwtPlotGrid::isOnLine() const
{
	return d_data->isOnLine;
}

void IceQwtPlotGrid::setIsShow800(bool isShow800)
{
	d_data->isShow800 = isShow800;
}

bool IceQwtPlotGrid::isShow800() const
{
	return d_data->isShow800;
}

void IceQwtPlotGrid::updateScaleDiv(const QwtScaleDiv& xScaleDiv, const QwtScaleDiv& yScaleDiv)
{
	if(d_data->isOnLine)
	{
		return QwtPlotGrid::updateScaleDiv(xScaleDiv, yScaleDiv);
	}
	IceQwtPlot* plot = (IceQwtPlot*) this->plot();
	QwtInterval xInterval = plot->rasterItem()->interval(Qt::XAxis);
	QwtInterval yInterval = plot->rasterItem()->interval(Qt::YAxis);

	QwtScaleDiv scaleDivX, scaleDivY;

	// Узнаем какие единицы измерения установлены
	IceUnitRatioQwtTransform* transform =
		(IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();
	switch(transform->unit())
	{
		case ICEIMAGE_NS::NONE: // Пиксели
		{
			scaleDivX = QwtLinearScaleEngine().divideScale(
				xInterval.minValue(), xInterval.maxValue(), 10, 10, hStepPixel());
			scaleDivY = QwtLinearScaleEngine().divideScale(
				yInterval.minValue(), yInterval.maxValue(), 10, 10, vStepPixel());
			break;
		}
		default: // Метрическая система
		{
			scaleDivX = QwtLinearScaleEngine().divideScale(
				xInterval.minValue(), xInterval.maxValue(), 10, 10, hStepMetric());
			scaleDivY = QwtLinearScaleEngine().divideScale(
				yInterval.minValue(), yInterval.maxValue(), 10, 10, vStepMetric());
			break;
		}
	}
	QwtPlotGrid::updateScaleDiv(scaleDivX, scaleDivY);
}

void IceQwtPlotGrid::draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
	const QRectF& canvasRect) const
{
	QwtPlotGrid::draw(painter, xMap, yMap, canvasRect);
}

} // namespace ICEQWT_NS
