//==============================================================================
// Название:        ---
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
// Имя файла: iceqwtplotmagnifier.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "iceqwtplot.h"
#include "iceqwtplotmagnifier.h"
#include "iceqwtplotrescaler.h"
#include "iceqwtplotpanner.h"
#include "iceqwtplotrasteritem.h"

#include <iceimage/imagebuilder.h>

#include <QtGui/QWheelEvent>

#include <QtCore/qmath.h>

namespace ICEQWT_NS
{

class IceQwtPlotMagnifier::PrivateData
{
public:
	PrivateData()
	{

	}

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder;
	QPoint pos;
	QPointF pointBeforeZoom;
};

//! Конструктор
IceQwtPlotMagnifier::IceQwtPlotMagnifier(QWidget* parent) : QwtPlotMagnifier(parent)
{
	d_data = new PrivateData;
}

//! Деструктор
IceQwtPlotMagnifier::~IceQwtPlotMagnifier()
{
	delete d_data;
}

void IceQwtPlotMagnifier::setImageBuilder(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	d_data->imageBuilder = imageBuilder;
}

/*!
   \brief Подготовка осей к масштабирование изображения
*/ 
void IceQwtPlotMagnifier::rescale(double factor)
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	if(!plt)
	{
		return;
	}
	factor = qAbs(factor);
	if(factor == 1.0 || factor == 0.0)
	{
		return;
	}
	// Установка коэффициентов масштабирования 
	plt->rescaler()->setScaleFactor(Qt::XAxis, plt->rescaler()->scaleFactor(Qt::XAxis) * factor);
	plt->rescaler()->setScaleFactor(Qt::YAxis, plt->rescaler()->scaleFactor(Qt::YAxis) * factor);

	// Обновление осей
	IceQwtPlotRescaler::MODE_APPLY modeApply = plt->rescaler()->modeApply();
	plt->rescaler()->setModeApply(IceQwtPlotRescaler::UPDATE_AXIS);
	plt->rescaler()->rescale();
	plt->rescaler()->setModeApply(modeApply);
}

/*!
   \brief Масштабирование изображения
*/
void IceQwtPlotMagnifier::scale()
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	// После установки масштаба изображения будет автоматически вызван replot
	Q_ASSERT(!d_data->imageBuilder.isNull());
	d_data->imageBuilder->setScaleFactor(plt->rescaler()->scaleFactor(Qt::XAxis), 
		plt->rescaler()->scaleFactor(Qt::YAxis));

	// TODOTODOTODOTODOTODOTODO
	plt->updateAxes();
	plt->rasterItem()->setScaleRegionRect();
}

/*!
   \brief Получить масштаб в процентах при котором изображение вписывается в окно
*/
int IceQwtPlotMagnifier::getPercentScaleInSize()
{
	int xScale = getPercentScaleInWidth();
	int yScale = getPercentScaleInHeight();

	if(xScale < yScale)
	{
		return xScale;
	}
	else
	{
		return yScale;
	}
}

/*!
  \brief Получить масштаб в процентах при котором изображение вписывается по высоте
*/
int IceQwtPlotMagnifier::getPercentScaleInHeight()
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	// Длина оси Y
	double intvY = plt->invTransform(QwtPlot::yLeft, plt->canvasMap(QwtPlot::yLeft).pDist());
	// Высота изображения 
	//double rasterX = icePlot->rasterItem()->height();
	QwtInterval rasterX = plt->rescaler()->intervalHint(QwtPlot::yLeft);
	// Коэффициент масштабирования по оси Y
	double K = intvY / rasterX.width();
	return qRound(K * 100.0);
}

/*!
  \brief Получить масштаб в процентах при котором изображение вписывается по ширине
*/
int IceQwtPlotMagnifier::getPercentScaleInWidth()
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	// Длина оси X 
	double intvX = plt->invTransform(QwtPlot::xTop, plt->canvasMap(QwtPlot::xTop).pDist());
	// Ширина изображения
	//double rasterX = icePlot->rasterItem()->width();
	QwtInterval rasterX = plt->rescaler()->intervalHint(QwtPlot::xTop);
	// Коэффициент масштабирования по оси Y
	double K = intvX / rasterX.width();
	return qRound(K * 100.0);
}

/*!
  \brief Фиксирование точки, положение которой необходимо вернуть после масштабирования
*/
void IceQwtPlotMagnifier::fixPoint(const QPoint& pos)
{
	IceQwtPlot* plt = ((IceQwtPlot*)(plot()));

	// Если уже этим механизмом пользуются, то надо отказать
	if(!d_data->pos.isNull())
	{
		return;
	}
	// Точка до масштабирования в пикселях канваса
	d_data->pos = pos;

	// Точка до масштабирования в размерности шкал
	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);
	d_data->pointBeforeZoom = QwtScaleMap::invTransform(mapX, mapY, d_data->pos);
}

/*!
  \brief Возврат положения фиксированной точки
*/
void IceQwtPlotMagnifier::restorePoint()
{
	IceQwtPlot* plt = (IceQwtPlot*)(plot());
	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);
	QPointF pointAfterZoom = QwtScaleMap::transform(mapX, mapY, d_data->pointBeforeZoom);
	QPointF pointDiff = d_data->pos - pointAfterZoom;

	// Перемещаем изображение, чтобы точка до масштабирования была в том
	// же месте что и после масштабирования, при этом панер не должен делать
	// replot, так как он будет сделан косвенно ниже (в вызываемой ф-ии). Тем не
	// менее updateAxis необходимо делать, так как он изменяте scaleDiv у осей, чем
	// пользуется скалер
	bool isReplotPanner = plt->panner()->isReplot();
	plt->panner()->setIsReplot(false);
	plt->panner()->move(pointDiff.toPoint());
	plt->panner()->setIsReplot(isReplotPanner);

	// Панер оси передвинул - надо обновить скалер. При этом не надо делать
	// replot и updateAxis, так как после масштабирования (см. вызываемую ф-ии) 
	// будет автоматически вызван replot а тот уже вызовет updateAxis
	IceQwtPlotRescaler::MODE_APPLY modeApply = plt->rescaler()->modeApply();
	plt->rescaler()->setModeApply(IceQwtPlotRescaler::NONE);
	plt->rescaler()->rescale();
	plt->rescaler()->setModeApply(modeApply);

	// Механизмов закончили пользоваться
	d_data->pos = QPoint();
}

/*!
   \brief Установить масштаб в процентах
*/
void IceQwtPlotMagnifier::setPercentScale(int percentScale)
{
	IceQwtPlot* plt = ((IceQwtPlot*)(plot()));

	// Центр окна до масштабирования
	QRect rc = plt->canvas()->contentsRect();
	QPoint pos = rc.center();
	fixPoint(pos);

	// Перерасчет адаптирует входной параметр для rescale
	rescale((percentScale / 100.0) / plt->rescaler()->scaleFactor(Qt::XAxis));

	// Восстанавливаем точку
	restorePoint();

	// Масштабирование изображения
	scale();
}

/*!
   \brief Получить текущий масштаб изображения в процентах
*/
int IceQwtPlotMagnifier::getCurrentPercentScale() const
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	return plt->rescaler()->scaleFactor(Qt::XAxis) * 100;
}

void IceQwtPlotMagnifier::widgetWheelEvent(QWheelEvent* wheelEvent)
{
	// Будем позиционировать изображение по текущей точке мыши.
	// В конце метода restorePoint() вызывать не надо, так как эта
	// ф-ия вызовется в setPercentScale()
	fixPoint(wheelEvent->pos());

	// Масштабируем
	if(wheelEvent->delta() > 0)
	{
		emit zoomIn();
	}
	else
	{
		emit zoomOut();
	}
}

} // namespace ICEQWT_NS
