#ifndef ICEQWTPLOTRESCALER_H
#define ICEQWTPLOTRESCALER_H

#include "iceqwtdef.h"

#include <qwt_plot_rescaler.h>

namespace ICEQWT_NS
{

/*
	При использовании ExpandSmart, referenceAxis никак не учитывается. 
	Изменение всех осей происходит по одному и тому же алгоритму в 
	зависимости от настройки трансформации оси, где "зашитs" единицы измерения
	шкалы. Масштаб шкалы учитывается в соответствующей разнице interval и размером
	шкалы в пикселях канваса
*/

class ICEQWT_EXPORT IceQwtPlotRescaler: public QwtPlotRescaler
{
public:
	enum ExpandingDirection
	{
		// "Умное" изменение размеров шкал
		ExpandSmart = QwtPlotRescaler::ExpandBoth + 1
	};

	enum RescalePolicy
	{
		// "Умное" изменение размеров шкал
		ExpandingSmart = QwtPlotRescaler::Fitting + 1
	};

	// После вызова rescale необходимо cделать:
	enum MODE_APPLY
	{
		REPLOT, // replot (будет сделана настройка осей и полная перерисовка плота)
		UPDATE_AXIS, // Только настройть оси
		NONE // Ничего не делать
	};

	explicit IceQwtPlotRescaler(QWidget* canvas, QwtPlotRescaler::RescalePolicy = Expanding,
		int referenceAxis = QwtPlot::xBottom);

	virtual ~IceQwtPlotRescaler();

	void setModeApply(MODE_APPLY modeApply);
	MODE_APPLY modeApply() const;

	void setScaleFactor(double scaleFactor);
	void setScaleFactor(Qt::Axis axis, double scaleFactor);
	double scaleFactor(Qt::Axis axis) const;

protected:
	virtual void updateScales(QwtInterval intervals[QwtPlot::axisCnt]) const;
	virtual QwtInterval expandScale(int axis, const QSize& oldSize, const QSize& newSize) const;
	virtual QwtInterval syncScale(int axis, const QwtInterval& reference, const QSize& size) const;

private:
	QwtInterval expandInterval(const QwtInterval& oldInterval, double futureWidth,
		const QwtInterval& hintInterval) const;

	class AxisData;
	class PrivateData;
	PrivateData* d_data;
};

}

#endif