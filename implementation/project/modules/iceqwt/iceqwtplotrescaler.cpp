#include "iceqwtplotrescaler.h"

#include "iceqwtplot.h"

#include <qwt_scale_engine.h>

namespace ICEQWT_NS
{

class IceQwtPlotRescaler::AxisData
{
public:
	AxisData() : scaleFactor(1.0)
	{
	
	}

	double scaleFactor;
};

class IceQwtPlotRescaler::PrivateData
{
public:
	PrivateData() : modeApply(UPDATE_AXIS)
	{

	}

	bool isRePlot;
	IceQwtPlotRescaler::AxisData axisData[Qt::ZAxis]; // Qt::ZAxis - используется только как кол-во осей
	MODE_APPLY modeApply;
};

IceQwtPlotRescaler::IceQwtPlotRescaler(QWidget* canvas, QwtPlotRescaler::RescalePolicy rescalePolicy,
	int referenceAxis) : QwtPlotRescaler(canvas, referenceAxis, rescalePolicy)
{
	d_data = new PrivateData;

	// Все шкалы используют алгоритм изменения одинаковый, нет разницы между referenceAxis и
	// другими шкалами. На разницу изменения зависит только настройки трансформации шкал, где
	// "зашиты" единицы измерения
	setAspectRatio(1);
}

IceQwtPlotRescaler::~IceQwtPlotRescaler()
{
	delete d_data;
}

/*!
   \brief Установить масштаб изображения
*/
void IceQwtPlotRescaler::setScaleFactor(double scaleFactor)
{
	for(int axis=0; (Qt::Axis)axis<Qt::ZAxis; axis++)
	{
		setScaleFactor((Qt::Axis)axis, scaleFactor);
	}
}

void IceQwtPlotRescaler::setScaleFactor(Qt::Axis axis, double scaleFactor)
{
	d_data->axisData[axis].scaleFactor = scaleFactor;
}

/*!
   \brief Получить масштаб изображения
*/
double IceQwtPlotRescaler::scaleFactor(Qt::Axis axis) const
{
	return d_data->axisData[axis].scaleFactor;
}

/*!
   \brief Задать режим применения расчитанных осей. При режиме
	 REPLOT будет вызван replot, где сначала будет вызван updateAxis, потом
	 перерисовка канваса. При режиме UPDATE_AXIS будет вызван только updateAxis
*/
void IceQwtPlotRescaler::setModeApply(MODE_APPLY modeApply)
{
	d_data->modeApply = modeApply;
}

/*!
   \brief Получить режим применения расчитанных осей
*/
IceQwtPlotRescaler::MODE_APPLY IceQwtPlotRescaler::modeApply() const
{
	return d_data->modeApply;
}

/*!
   \brief Синхронизация вспомогательных осей (не главной, не той что
	 задается setReference). Как говорилось выше, разделение осей не ведется
	 на основную и вспомогательную, поэтому изменения вспомогательных осей
	 происходят по тем же алгоритмам что и изменения основной оси
*/
QwtInterval IceQwtPlotRescaler::syncScale(int axis, const QwtInterval& reference,
	const QSize& size) const
{
	switch((int)rescalePolicy())
	{
		case ExpandingSmart:
		{
			// Как и говорилось выше, все шкалы изменяются одинаково
			return expandScale(axis, QSize(), QSize());
		}
		default:
		{
			return IceQwtPlotRescaler::syncScale(axis, reference, size);
		}
	}
}

/*!
   \brief Изменить ось. Для изменения оси не используется ни oldSize и
		newSize. Эти размеры в базовом классе используются для определения 
		смещения размера оси. Этот механизм определения оказался не точным и 
		наблюдалось дрожание шкал. Поэтом принято решение, что алгорит изменения
		шкалы будет брать текущии размер шкалы в пикселах канваса из QwtScaleMap
*/
QwtInterval IceQwtPlotRescaler::expandScale(int axisId, const QSize& oldSize,
	const QSize& newSize) const
{
	// oldSize и newSize игнорируются, так как они не дают точного механизма
	// изменения размера канваса (происходит дрожание)

	IceQwtPlot* plt = (IceQwtPlot*)plot();
	switch((int)rescalePolicy())
	{
		case ExpandingSmart:
		{
			// Запрашиваем размеры изображеня по данной оси в размерности шкал. Для
			// каждой из 4 осей этот размер может быть разным, так как он будет зависеть
			// от единиц измерения шкалы, которые устанавливаются в трансформации.
			QwtInterval hintInterval = intervalHint(axisId);
			if(!hintInterval.isValid())
			{
				return QwtInterval();
			}
			// Определение точного размера канваса в единицах резмерности шкал. Именно этот
			// механизм используется взамен использования oldSize и newSize
			QwtScaleMap scaleMap = plt->canvasMap(axisId);
			// Получаем трансформацию оси
			QwtTransform* transform = plt->axisScaleEngine(axisId)->transformation();
			double width = 0;
			// По хорошему, при использовании подхода ICE трансформация должна быть установлена
			// всегда. Тем не менее проверку надо делать ...
			// Ширина оси в размерности шкал должна быть приведена в соответствие с размерностью
			// шкалы в пикселях. По сути это новый размер оси
			Qt::Axis axis = plt->classAxis((QwtPlot::Axis)axisId);
			if(transform)
			{
				width = transform->invTransform(scaleMap.pDist()) / d_data->axisData[axis].scaleFactor;
			}
			else
			{
				width = scaleMap.invTransform(scaleMap.pDist()) / d_data->axisData[axis].scaleFactor;
			}
			// Получаем размер шкалы еще до изменения ее размера
			QwtInterval oldInterval = interval(axisId);
			return expandInterval(oldInterval, width, hintInterval);
		}
		default:
		{
			return QwtPlotRescaler::expandScale(axisId, oldSize, newSize);
		}
	}
}

/*!
   \brief Расчитываем новый интервал оси исходя из размера изображения по этой оси,
	 старого интервала и нового размера оси
*/
QwtInterval IceQwtPlotRescaler::expandInterval(const QwtInterval &oldInterval, 
	double futureWidth, const QwtInterval& hintInterval) const
{
	//Здесь непосредственно алгоритм настройки осей
	if(futureWidth >= hintInterval.width())
	{
		if(oldInterval.minValue() <= hintInterval.minValue() &&
			oldInterval.maxValue() >= hintInterval.maxValue())
		{
			return QwtPlotRescaler::expandInterval(oldInterval, futureWidth, ExpandBoth);
		}
		else
		{
			return QwtPlotRescaler::expandInterval(hintInterval, futureWidth, ExpandBoth);
		}
	}
	else 
	{
		// Сначала тянем правую сторону насколько это возможно, потом подтягиваем левую.
		// Далее уже выравниваем изображения по середине
		double deltaLeft = oldInterval.minValue() - hintInterval.minValue();
		double deltaRight = hintInterval.maxValue() - oldInterval.maxValue();
		QwtInterval newInterval = oldInterval;
		if(deltaRight < 0 || deltaLeft < 0)
		{
			return QwtInterval(hintInterval.minValue(), hintInterval.minValue() +
				futureWidth);
		}
		newInterval = QwtPlotRescaler::expandInterval(newInterval, futureWidth,
			ExpandUp);
		double delta = newInterval.maxValue() - hintInterval.maxValue();
		if(delta > 0)
		{
			newInterval.setMaxValue(hintInterval.maxValue());
			newInterval = QwtPlotRescaler::expandInterval(newInterval, futureWidth,
				ExpandDown);
			return newInterval;
		}
		return newInterval;
	}
}

void IceQwtPlotRescaler::updateScales(QwtInterval intervals[QwtPlot::axisCnt]) const
{
	QwtPlot* plt = const_cast<QwtPlot*>(plot());
	for(int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		if(axis == referenceAxis() || aspectRatio(axis) > 0.0)
		{
			if(!intervals[axis].isValid())
			{
				continue;
			}
			double v1 = intervals[axis].minValue();
			double v2 = intervals[axis].maxValue();
			if(!plt->axisScaleDiv(axis).isIncreasing())
			{
				qSwap(v1, v2);
			}
			plt->setAxisScale(axis, v1, v2, plt->axisStepSize(axis));
		}
	}

	// Изменение шкал происходит либо при изменении размера канваса, либо при 
	// масштабировании (при перемещении, изменения шкал можно расчитать исходя из
	// текущих настроек шкал и нет необходимости прибегать к данному классу (что и 
	// происходит в panner)). При изменении размера нет необходимости вызывать replot, так
	// как replot вызовет помимо updateAxes еще и перерисовку канваса. Перерисовка канваса
	// и так будет сделана, так как после события изменения размера система создаст событие 
	// перерисовки
	switch(d_data->modeApply)
	{
		case REPLOT :
		{
			plt->replot();
			break;
		}
		case UPDATE_AXIS :
		{
			plt->updateAxes();
			break;
		}
		case NONE :
		{

		}
	}
}

} // namespace ICEQWT_NS
