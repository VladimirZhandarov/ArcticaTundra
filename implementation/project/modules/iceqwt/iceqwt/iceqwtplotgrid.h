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
// Имя файла: iceqwtplotgrid.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOTGRID_H
#define ICEQWTPLOTGRID_H

#include "iceqwtdef.h"

#include <qwt_plot_grid.h>

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtPlotGrid : public QwtPlotGrid
{
public:
	IceQwtPlotGrid();
	virtual ~IceQwtPlotGrid();

	void setHStepPixel(unsigned int vStepPixel);
	unsigned int hStepPixel() const;

	void setVStepPixel(unsigned int);
	unsigned int vStepPixel() const;

	void setHStepMetric(double hStepMetric);
	double hStepMetric() const;

	void setVStepMetric(double vStepMetric);
	double vStepMetric() const;

	void setIsOnLine(bool isOnLine);
	bool isOnLine() const;

	void setIsShow800(bool isShow800);
	bool isShow800() const;

	virtual void updateScaleDiv(const QwtScaleDiv& xScaleDiv, const QwtScaleDiv& yScaleDiv);

	virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
		const QRectF& canvasRect) const;

private:
	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOTGRID_H
