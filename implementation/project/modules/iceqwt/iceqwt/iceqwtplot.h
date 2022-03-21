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
// Имя файла: iceqwtplot.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOT_H
#define ICEQWTPLOT_H

#include "iceqwtdef.h"

#include <qwt_plot.h>

class QwtPlotPanner;

namespace ICECORE_NS
{
	struct ImageGridSettings;
	struct ImageLineSettings;
}

namespace ICEQWT_NS
{
	class IceQwtPlotRasterItem;
	class IceQwtPlotPicker;
	class IceQwtScrollPlotMagnifier;
	class IceQwtPlotPanner;
	class IceQwtPlotGrid;
	class IceQwtPlotRescaler;

class ICEQWT_EXPORT IceQwtPlot : public QwtPlot
{
	Q_OBJECT
public:
	enum MODE
	{
		PICKER_RECT = 0,
		PICKER_ELLIPSE,
		PICKER_POLYGON,
		PICKER_POLYLINE,
		MOVE,
		NOMODE
	};

	static Qt::Axis classAxis(QwtPlot::Axis axisId);

	double transform(QwtPlot::Axis axisId, const double& value) const;
	QPointF transform(const QPointF& scale) const;
	QRectF transform(const QRectF& scale) const;
	QwtInterval transform(QwtPlot::Axis axisId, const QwtInterval& interval) const;
	double invTransform(QwtPlot::Axis axisId, const double& value) const;
	QPointF invTransform(const QPointF& point) const;
	QRectF invTransform(const QRectF& point) const;

	IceQwtPlot(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~IceQwtPlot();

private:
	void init();

public:
	void setup();
	void setCursorColor(const QColor& color = QColor());
	ICECORE_NS::ImageLineSettings getLineSettings() const;
	void setupLine(const ICECORE_NS::ImageLineSettings& imageLineSettings);
	ICECORE_NS::ImageGridSettings getGridSettings() const;
	void setupGrid(const ICECORE_NS::ImageGridSettings& imageGridSettings);
	void setupAxisInterval(const QwtInterval& intervalX, const QwtInterval& intervalY);

	virtual void updateLayout();

	IceQwtScrollPlotMagnifier* magnifier() const;
	IceQwtPlotGrid* grid() const;
	IceQwtPlotPanner* panner() const;
	IceQwtPlotPicker* picker() const;
	IceQwtPlotRasterItem* rasterItem() const;
	IceQwtPlotRescaler* rescaler() const;

	// Сетка
	bool gridVisible() const;
	void setGridVisible(bool tf);

	// Оси
	bool axisVisible() const;
	void setAxisVisible(bool tf);

	void setMouseTracking(bool enable);

	int getMajorStep(double scale);
	void setMode(MODE mode);
	MODE getMode() const;
	void rescale();

signals:
	void cursorPositionChanged(const QPoint& pos);

private:
	void setupLine();

	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOT_H
