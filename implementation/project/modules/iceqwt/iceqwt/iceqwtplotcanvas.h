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
// Имя файла: iceqwtplotcanvas.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOTCANVAS_H
#define ICEQWTPLOTCANVAS_H

#include "iceqwtdef.h"

#include <qwt_plot_canvas.h>

class QwtPlot;

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtPlotCanvas : public QwtPlotCanvas
{
	Q_OBJECT
public:
	explicit IceQwtPlotCanvas(QwtPlot* = 0);
	virtual ~IceQwtPlotCanvas();

protected:
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);

signals:
	void cursorPositionChanged(const QPoint& pos);
	void mouseDoubleClicked(const QPoint& pos);
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOTCANVAS_H
