//==============================================================================
// ��������:        ---
// ��������� �����: ��� "���" DeveloperGroup (�. �����-���������)
//==============================================================================
//
// �����:     $Author: shchav $
// ������:    2011.11.08
// �������:   $Date: 2013-04-19 18:46:52 +0400 (Пт, 19 апр 2013) $
// ������:    iceqwt
// ���������: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// ��� �����: iceqwtplotcanvas.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "iceqwtplotcanvas.h"

#include "iceqwtplot.h"
#include "iceqwtscaledraw.h"

#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include <QtGui/QResizeEvent>

namespace ICEQWT_NS
{

IceQwtPlotCanvas::IceQwtPlotCanvas(QwtPlot* plot) : QwtPlotCanvas(plot)
{
	for(int axis = 0; axis < QwtPlot::axisCnt; axis++)
	{
		plot->setAxisScaleDraw(axis, new IceQwtScaleDraw);
	}
}

IceQwtPlotCanvas::~IceQwtPlotCanvas()
{

}

void IceQwtPlotCanvas::mouseMoveEvent(QMouseEvent* event)
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	double xMargin = plt->plotLayout()->canvasMargin(QwtPlot::xTop);
	double yMargin = plt->plotLayout()->canvasMargin(QwtPlot::yLeft);
	QPoint pos(event->pos());
	pos += QPoint(xMargin, yMargin);

	// ��������� ������ ��� ������� ������� ��� ��������� ������� ������� �� ���
	for(int axis = 0; axis < QwtPlot::axisCnt; axis++)
	{
		if(IceQwtPlot::classAxis(QwtPlot::Axis(axis)) == Qt::XAxis)
		{
			((IceQwtScaleDraw*)(plt->axisScaleDraw(axis)))->setCursorPos(pos.x());
		}
		else
		{
			((IceQwtScaleDraw*)(plt->axisScaleDraw(axis)))->setCursorPos(pos.y());
		}
	}

	//TODO FIX!!!
	plt->axisWidget(QwtPlot::xTop)->update();
	plt->axisWidget(QwtPlot::yLeft)->update();
	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);
	QPointF scalePoint = QwtScaleMap::invTransform(mapX, mapY, event->pos());
	QPointF imagePoint = plt->transform(scalePoint);
	emit cursorPositionChanged(QPoint(imagePoint.x(), imagePoint.y()));

	QwtPlotCanvas::mouseMoveEvent(event);
}

void IceQwtPlotCanvas::mouseDoubleClickEvent(QMouseEvent * event)
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	double xMargin = plt->plotLayout()->canvasMargin(QwtPlot::xTop);
	double yMargin = plt->plotLayout()->canvasMargin(QwtPlot::yLeft);
	QPoint pos(event->pos());
	pos += QPoint(xMargin, yMargin);

	// ��������� ������ ��� ������� ������� ��� ��������� ������� ������� �� ���
	for(int axis = 0; axis < QwtPlot::axisCnt; axis++)
	{
		if(IceQwtPlot::classAxis(QwtPlot::Axis(axis)) == Qt::XAxis)
		{
			((IceQwtScaleDraw*)(plt->axisScaleDraw(axis)))->setCursorPos(pos.x());
		}
		else
		{
			((IceQwtScaleDraw*)(plt->axisScaleDraw(axis)))->setCursorPos(pos.y());
		}
	}

	//TODO FIX!!!
	plt->axisWidget(QwtPlot::xTop)->update();
	plt->axisWidget(QwtPlot::yLeft)->update();
	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);
	QPointF scalePoint = QwtScaleMap::invTransform(mapX, mapY, event->pos());
	QPointF imagePoint = plt->transform(scalePoint);
	emit mouseDoubleClicked(QPoint(imagePoint.x(), imagePoint.y()));

	QwtPlotCanvas::mouseMoveEvent(event);
}

} // namespace ICEQWT_NS
