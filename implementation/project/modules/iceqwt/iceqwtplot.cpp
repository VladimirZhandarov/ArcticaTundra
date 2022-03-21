//==============================================================================
// Название:        Компонент отображения изображения
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
// Имя файла: iceqwtplot.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "iceqwtplot.h"

#include "iceqwtplotrasteritem.h"
#include "iceqwtplotpanner.h"
#include "iceqwtscrollplotmagnifier.h"
#include "iceqwtplotpicker.h"
#include "iceqwtplotcanvas.h"
#include "iceqwtplotgrid.h"
#include "iceqwtplotrescaler.h"
#include "iceunitratioqwttransform.h"

#include <icecore/core.h>
#include <icecore/settings.h>

#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_scale_engine.h>
#include <qwt_painter.h>

#include <QtGui/QLayout>
#include <QtGui/QResizeEvent>

#include <QtCore/qmath.h>

namespace ICEQWT_NS
{

Qt::Axis IceQwtPlot::classAxis(QwtPlot::Axis axisId)
{
	switch(axisId)
	{
		case QwtPlot::xBottom:
		case QwtPlot::xTop:
		{
			return Qt::XAxis;
		}
		case QwtPlot::yLeft:
		case QwtPlot::yRight:
		default:
		{
				return Qt::YAxis;
		}
	}
}

double IceQwtPlot::transform(QwtPlot::Axis axisId, const double& value) const
{
	double result = value;
	result -= rasterItem()->origin(classAxis(axisId));
	QwtScaleMap map = canvasMap(axisId);
	result = map.transformation()->transform(result);
	return result;
}

QPointF IceQwtPlot::transform(const QPointF& scale) const
{
	QPointF result = scale;
	result.setX(transform(QwtPlot::xTop, result.x()));
	result.setY(transform(QwtPlot::yLeft, result.y()));
	return result;
}

QRectF IceQwtPlot::transform(const QRectF& scale) const
{
	QRectF result = scale;
	result.setTopLeft(transform(result.topLeft()));
	result.setBottomRight(transform(result.bottomRight()));
	return result;
}

QwtInterval IceQwtPlot::transform(QwtPlot::Axis axisId, const QwtInterval& interval) const
{
	QwtInterval result;
	result.setMinValue(transform(axisId, interval.minValue()));
	result.setMaxValue(transform(axisId, interval.maxValue()));
	return result;
}

double IceQwtPlot::invTransform(QwtPlot::Axis axisId, const double& value) const
{
	double result = value;
	QwtScaleMap map = canvasMap(axisId);
	result = map.transformation()->invTransform(result);
	result += rasterItem()->origin(classAxis(axisId));
	return result;
}

QPointF IceQwtPlot::invTransform(const QPointF& point) const
{
	QPointF result = point;
	result.setX(invTransform(QwtPlot::xTop, result.x()));
	result.setY(invTransform(QwtPlot::yLeft, result.y()));
	return result;
}

QRectF IceQwtPlot::invTransform(const QRectF& point) const
{
	QRectF result = point;
	result.setTopLeft(invTransform(result.topLeft()));
	result.setTopRight(invTransform(result.topRight()));
	result.setBottomRight(invTransform(result.bottomRight()));
	result.setBottomLeft(invTransform(result.bottomLeft()));
	return result;
}

class IceQwtPlot::PrivateData
{
public:
	PrivateData() : isInit(false), mode(NOMODE), isGridVisible(false), magnifier(0),
		grid(0), panner(0), picker(0), rasterItem(0), rescaler(0)
	{

	}
	bool isInit;
	MODE mode;
	bool isGridVisible;

	IceQwtScrollPlotMagnifier* magnifier;
	IceQwtPlotGrid* grid;
	IceQwtPlotPanner* panner;
	IceQwtPlotPicker* picker;
	IceQwtPlotRasterItem* rasterItem;
	IceQwtPlotRescaler* rescaler;
};

//! Конструктор
IceQwtPlot::IceQwtPlot(QWidget* parent, Qt::WindowFlags /*f*/) : QwtPlot(parent)
{
	setCanvas(new IceQwtPlotCanvas(this));

	init();

	parent->setLayout(new QHBoxLayout());
	parent->layout()->addWidget(this);

	setAutoReplot(false);

	enableAxis(QwtPlot::yLeft, false);
	enableAxis(QwtPlot::yRight, false);
	enableAxis(QwtPlot::xBottom, false);
	enableAxis(QwtPlot::xTop, false);
}

//! Деструктор
IceQwtPlot::~IceQwtPlot()
{
	delete d_data;
}

/*!
  \brief Инициализация IceQwtPlot
 */
void IceQwtPlot::init()
{
	d_data = new PrivateData;

	// Элемент отображения изображения
	d_data->rasterItem = new IceQwtPlotRasterItem();
	d_data->rasterItem->setXAxis(QwtPlot::xTop);
	d_data->rasterItem->setYAxis(QwtPlot::yLeft);
	d_data->rasterItem->attach(this);
	//d_data->rasterItem->setItemAttribute(QwtPlotItem::AutoScale);

	// Механизм перемещения
	d_data->panner = new IceQwtPlotPanner(canvas());
	d_data->panner->setEnabled(true);
	connect(d_data->panner, SIGNAL(cursorPositionChanged(cost QPoint&)),
		this, SIGNAL(cursorPositionChanged(cost QPoint&)));

	// Механизм масштабирования
	d_data->magnifier = new IceQwtScrollPlotMagnifier(canvas());

	/*connect(d_data->magnifier, SIGNAL(moved(double, double)),
		d_data->panner, SLOT(moveCanvasNotOffset(double, double)));*/
	connect(d_data->panner, SIGNAL(panned(int, int)),
		d_data->magnifier, SLOT(pannedFinished(int, int)));

	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
	if(imageAddedSettings.useColorCursor)
	{
		setCursorColor(QColor(imageAddedSettings.colorCursorR,
			imageAddedSettings.colorCursorG, imageAddedSettings.colorCursorB,
			imageAddedSettings.colorCursorA));
	}
	else
	{
		setCursorColor();
	}

	// Механизм выделения
	d_data->picker = new IceQwtPlotPicker(canvas());
	setMode(NOMODE);

	// Оси
	//setAxisScaleEngine(QwtPlot::xTop, new IceXQwtLinearScaleEngine(this, d_data->rasterItem));
	//setAxisScaleEngine(QwtPlot::yLeft, new IceYQwtLinearScaleEngine(this, d_data->rasterItem));
	setAxisAutoScale(QwtPlot::xTop, false);
	setAxisAutoScale(QwtPlot::yLeft, false);
	setAxisMaxMinor(QwtPlot::xTop, 10);
	setAxisMaxMinor(QwtPlot::yLeft, 10);
	enableAxis(xBottom, false);
	enableAxis(yRight, false);
	//setAxisMaxMajor(QwtPlot::xTop, 20);
	//setAxisMaxMajor(QwtPlot::yLeft, 20);
	// Не будет использовать округление чисел, например при расчете  шкал
	// - так получается плавнее их отрисовка
//	QwtPainter::setRoundingAlignment(true);
	// На шкалах будут и метки и цифры
	axisScaleDraw(3)->enableComponent(QwtAbstractScaleDraw::Labels, true);
	axisScaleDraw(3)->enableComponent(QwtAbstractScaleDraw::Ticks, true);
	// Если эти значения больше размеров шрифта, то шкала не прыгает, когда
	// она находится на грани отрисовки цифр шкал
	axisWidget(0)->setMinBorderDist(10, 0);
	axisWidget(3)->setMinBorderDist(10, 0);
	//axisWidget(3)->setTitle("пиксели");
	//axisWidget(3)->setLabelAlignment("пиксели");

	// Сетка
	d_data->grid = new IceQwtPlotGrid();
	d_data->grid->attach(this);
	d_data->grid->setAxes(QwtPlot::xTop, QwtPlot::yLeft);
	d_data->grid->hide();
	//plotLayout()->setAlignCanvasToScales(false);

	// Ось Y необходимо инвертировать, так как ее 0 должен быть в левом верхнем углу
	// (по умолчанию ее 0 в нижнем левом углу)
	QwtScaleDiv scaleDiv = axisScaleDraw(QwtPlot::yLeft)->scaleDiv();
	scaleDiv.invert();
	setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
	axisScaleDraw(QwtPlot::yLeft)->setScaleDiv(scaleDiv);

	d_data->rescaler = new IceQwtPlotRescaler(canvas(), 
		(QwtPlotRescaler::RescalePolicy)(IceQwtPlotRescaler::ExpandingSmart));

	axisScaleEngine(QwtPlot::xTop)->setTransformation(new IceUnitRatioQwtTransform());
	axisScaleEngine(QwtPlot::yLeft)->setTransformation(new IceUnitRatioQwtTransform());
}

void IceQwtPlot::updateLayout()
{
	QwtPlot::updateLayout();
}

void IceQwtPlot::setup()
{
	//Настройка линеек 
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	if(!imageLineSettings.isUseUserParam)
	{
		imageLineSettings = settings->getImageLineSettingsDefault();
	}
	setupLine(imageLineSettings);

	// Настройка сетки
	// Получаем пользовательские параметры сетки.
	// Они всегда есть. Если файл пользовательских настроек недоступен то в пользовательские
	// параметы записываюся параметры по умолчанию. Если пользовательский файл настроек неполон, то
	// те значения настроек сетки, которых нет в файле или повреждены, заменяются настройками по умолчанию
	ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();
	// По данному флагу определяем, какие параметры надо устанавливать в сетку, пользовательские или параметры
	// по умолчания (что жестко зашиты в программу)
	if(!imageGridSettings.isUseUserParam)
	{
		imageGridSettings = settings->getImageGridSettingsDefault();
	}
	setupGrid(imageGridSettings);

	// Установка в rescaler размеров изображения
	for(int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		if(classAxis(QwtPlot::Axis(axis)) == Qt::XAxis)
		{
			d_data->rescaler->setIntervalHint(axis, d_data->rasterItem->interval(Qt::XAxis));
		}
		else
		{
			d_data->rescaler->setIntervalHint(axis, d_data->rasterItem->interval(Qt::YAxis));
		}
	}
}

void IceQwtPlot::setCursorColor(const QColor& color)
{
	if(color.isValid())
	{
		QPixmap cursorPixmap(32, 32);
		cursorPixmap.fill(QColor(0, 0, 0, 0));
		QPainter painter;
		QPen pen(color, 2);
		painter.begin(&cursorPixmap);
		painter.setPen(pen);
		painter.drawLine(0, (cursorPixmap.height() - 1) / 2,
			cursorPixmap.width() - 1, (cursorPixmap.height() - 1) / 2);
		painter.drawLine((cursorPixmap.width() - 1) / 2, 0,
			(cursorPixmap.width() - 1) / 2, cursorPixmap.height() - 1);
		painter.end();
		canvas()->setCursor(QCursor(cursorPixmap));
	}
	else
	{
		canvas()->setCursor(Qt::CrossCursor);
	}
}

ICECORE_NS::ImageLineSettings IceQwtPlot::getLineSettings() const
{
	ICECORE_NS::ImageLineSettings result;
	IceUnitRatioQwtTransform* transformX = (IceUnitRatioQwtTransform*)axisScaleEngine(QwtPlot::xTop)->transformation()->copy();
	Q_ASSERT(transformX);
	result.unit = transformX->unit();
	result.ratio = transformX->ratio();
	result.origin = rasterItem()->origin();
	result.isShow = axisVisible();
	return result;
}

void IceQwtPlot::setupLine(const ICECORE_NS::ImageLineSettings& imageLineSettings)
{
	// Устанавливаем параметры линеики
	IceUnitRatioQwtTransform* transformX = (IceUnitRatioQwtTransform*)axisScaleEngine(QwtPlot::xTop)->transformation()->copy();
	IceUnitRatioQwtTransform* transformY = (IceUnitRatioQwtTransform*)axisScaleEngine(QwtPlot::yLeft)->transformation()->copy();
	Q_ASSERT(transformX && transformY);
	transformX->setRatio(imageLineSettings.ratio, imageLineSettings.unit);
	transformY->setRatio(imageLineSettings.ratio, imageLineSettings.unit);
	rasterItem()->setOrigin(imageLineSettings.origin);

	axisScaleEngine(QwtPlot::xTop)->setTransformation(transformX);
	axisScaleEngine(QwtPlot::yLeft)->setTransformation(transformY);
	setAxisVisible(imageLineSettings.isShow);
}

ICECORE_NS::ImageGridSettings IceQwtPlot::getGridSettings() const
{
	ICECORE_NS::ImageGridSettings result;
	IceQwtPlotGrid* grid = this->grid();
	Q_ASSERT(grid);
	result.hStepPixel = grid->hStepPixel();
	result.vStepPixel = grid->vStepPixel();
	result.hStepMetric = grid->hStepMetric();
	result.vStepMetric = grid->vStepMetric();
	result.isOnLine = grid->isOnLine();
	result.isShow = gridVisible();
	result.isShow800 = grid->isShow800();
	QPen pen = grid->majorPen();
	result.color = pen.color();
	return result;
}

void IceQwtPlot::setupGrid(const ICECORE_NS::ImageGridSettings& imageGridSettings)
{
	// Устанавливаем параметры сетки
	IceQwtPlotGrid* grid = this->grid();
	Q_ASSERT(grid);
	grid->setHStepPixel(imageGridSettings.hStepPixel);
	grid->setVStepPixel(imageGridSettings.vStepPixel);
	grid->setHStepMetric(imageGridSettings.hStepMetric);
	grid->setVStepMetric(imageGridSettings.vStepMetric);
	grid->setIsOnLine(imageGridSettings.isOnLine);
	setGridVisible(imageGridSettings.isShow);
	grid->setIsShow800(imageGridSettings.isShow800);
	// Установка цвета и прозрачности сетки
	QPen pen = grid->majorPen();
	pen.setColor(imageGridSettings.color);
	grid->setMajorPen(pen);
}

void IceQwtPlot::setupAxisInterval(const QwtInterval& intervalX, const QwtInterval& intervalY)
{
	QwtScaleDiv scaleDivX = axisScaleDiv(QwtPlot::xTop);
	scaleDivX.setInterval(intervalX);
	setAxisScaleDiv(QwtPlot::xTop, scaleDivX);
	QwtScaleDiv scaleDivY = axisScaleDiv(QwtPlot::yLeft);
	scaleDivY.setInterval(intervalY);
	setAxisScaleDiv(QwtPlot::yLeft, scaleDivY);
	rescale();
}

void IceQwtPlot::rescale()
{
	// Обновление осей
	//TODO refac
	d_data->rescaler->setIntervalHint(QwtPlot::xTop, d_data->rasterItem->interval(Qt::XAxis));
	d_data->rescaler->setIntervalHint(QwtPlot::yLeft, d_data->rasterItem->interval(Qt::YAxis));
	ICEQWT_NS::IceQwtPlotRescaler::MODE_APPLY modeApply = d_data->rescaler->modeApply();
	d_data->rescaler->setModeApply(ICEQWT_NS::IceQwtPlotRescaler::UPDATE_AXIS);
	d_data->rescaler->rescale();
	d_data->rescaler->setModeApply(modeApply);

}

/*!
  \brief Возвращает механизм масштабирования
  \return Механизм масштабирования
*/
IceQwtScrollPlotMagnifier* IceQwtPlot::magnifier() const
{
	return d_data->magnifier;
}

/*!
  \brief Возвращает сетку
  \return Сетка
*/
IceQwtPlotGrid* IceQwtPlot::grid() const
{
	return d_data->grid;
}

/*!
  \brief Возвращает механизм перемещения
  \return Механизм перемещения
*/
IceQwtPlotPanner* IceQwtPlot::panner() const
{
	return d_data->panner;
}

/*!
  \brief Возвращает механизм выделение
  \return Механизм выделение
*/
IceQwtPlotPicker* IceQwtPlot::picker() const
{
	return d_data->picker;
}

/*!
  \brief Возвращает элемент отображения изображения
  \return Элемент отображения изображения
*/
IceQwtPlotRasterItem* IceQwtPlot::rasterItem() const
{
	return d_data->rasterItem;
}

IceQwtPlotRescaler* IceQwtPlot::rescaler() const
{
	return d_data->rescaler;
}

/*!
  \return \c True, если сетка отображается
*/
bool IceQwtPlot::gridVisible() const
{
	return d_data->isGridVisible;
}

/*!
  \brief Отобразить/скрыть сетку
  \param tf флаг отображения/скрытия сетки
*/
void IceQwtPlot::setGridVisible(bool tf)
{
	if(d_data->isGridVisible == tf)
	{
		return;
	}
	(d_data->isGridVisible = tf) ? d_data->grid->show() : d_data->grid->hide();
	replot();
}

/*!
  \return \c True, если оси отображаются
*/
bool IceQwtPlot::axisVisible() const
{
	// Об отображении всех осей можно судить отображению 
	// одной оси, так как все оси отображаются одновременно
	return axisEnabled(QwtPlot::yLeft);
}

/*!
  \brief Отобразить/скрыть оси

  \param tf флаг отображения/скрытия осей
*/
void IceQwtPlot::setAxisVisible(bool tf)
{
	if(axisVisible() == tf)
	{
		return;
	}
	enableAxis(QwtPlot::xTop, tf);
	enableAxis(QwtPlot::yLeft, tf);
}

/*!
  \brief Настроить оси под загруженное изображение

  После загрузки нового изображения оси необходимо
  настроить под размер этого изображения.
*/
//void IceQwtPlot::initAxis()
//{
//	//setAxisScale(QwtPlot::xTop, 0, canvas()->contentsRect().width(), 50);
//	//setAxisScale(QwtPlot::yLeft, canvas()->contentsRect().height(), 0, 50);
//	replot();
//	d_data->isInit = true;
//}

void IceQwtPlot::setMouseTracking(bool enable)
{
	d_data->panner->setMouseTracking(enable);
	canvas()->setMouseTracking(enable);
}

/*!
   \brief Получить цену основного элемента деления линейки оси от масштаба
*/
int IceQwtPlot::getMajorStep(double scale)
{
	int step = 0;
	if(scale <= 0.02)
	{
		step = 5000;
	}
	else if(scale > 0.02 && scale <= 0.05)
	{
		step = 2000;
	}
	else if(scale > 0.05 && scale <= 0.1)
	{
		step = 1000;
	}
	else if(scale > 0.1 && scale <= 0.25)
	{
		step = 500;
	}
	else if(scale > 0.25 && scale <= 0.5)
	{
		step = 200;
	}
	else if(scale > 0.5 && scale <= 1)
	{
		step = 100;
	}
	else if(scale > 1 && scale <= 2.5)
	{
		step = 50;
	}
	else if(scale > 2.5 && scale <= 5)
	{
		step = 20;
	}
	else if(scale > 5 && scale <= 10)
	{
		step = 10;
	}
	else if(scale > 10 && scale <= 25)
	{
		step = 5;
	}
	else if(scale > 25 && scale <= 32)
	{
		step = 2;
	}
	else
	{
		step = 1;
	}
	return step;
}

/*!
   \brief Установить режим работы плота
*/
void IceQwtPlot::setMode(MODE mode)
{
	d_data->mode = mode;

	switch(mode)
	{
		case PICKER_RECT:
		{
			picker()->setBand(QwtPicker::RectRubberBand);
			picker()->setEnabled(true);
			panner()->setEnabled(false);
			break;
		}
		case PICKER_ELLIPSE:
		{
			picker()->setBand(QwtPicker::EllipseRubberBand);
			picker()->setEnabled(true);
			panner()->setEnabled(false);
			break;
		}
		case PICKER_POLYGON:
		{
			picker()->setBand(QwtPicker::PolygonRubberBand);
			picker()->setEnabled(true);
			panner()->setEnabled(false);
			break;
		}
		case PICKER_POLYLINE:
		{
			picker()->setBand(QwtPicker::PolygonRubberBand, false);
			picker()->setEnabled(true);
			panner()->setEnabled(false);
			break;
		}
		case MOVE:
		{
			picker()->setEnabled(false);
			panner()->setEnabled(true);
			break;
		}
		case NOMODE:
		{
			picker()->setEnabled(false);
			panner()->setEnabled(false);
			break;
		}
	}
}

/*!
   \brief Получить режим работы плота
*/
IceQwtPlot::MODE IceQwtPlot::getMode() const
{
	return d_data->mode;
}

} // namespace ICEQWT_NS
