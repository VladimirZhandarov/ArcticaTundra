//==============================================================================
// ��������:        ---
// ��������� �����: ��� "���" DeveloperGroup (�. �����-���������)
//==============================================================================
//
// �����:     $Author: shchav $
// ������:    2011.11.08
// �������:   $Date: 2013-04-19 18:46:52 +0400 (Пт, 19 апр 2013) $
// ������:    iceapp
// ���������: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// ��� �����: iceqwtplotpanner.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "iceqwtplotpanner.h"

#include "iceqwtplot.h"
#include "iceqwtplotrasteritem.h"

#include <qwt_scale_div.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>

#include <QtGui/QMouseEvent>

namespace ICEQWT_NS
{

class IceQwtPlotPanner::PrivateData
{
public:
	PrivateData(): isReplot(true)
	{}

	bool isReplot;
};

IceQwtPlotPanner::IceQwtPlotPanner(QWidget* parent):
	QwtPlotPanner(parent)
{
	d_data = new PrivateData();
}

IceQwtPlotPanner::~IceQwtPlotPanner()
{
	 delete d_data;
}

void IceQwtPlotPanner::move(const QPoint& pos)
{
	moveCanvas(pos.x(), pos.y());
}

void IceQwtPlotPanner::moveCenter(double x, double y)
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);

	QPoint centerPoint(mapX.p1() + mapX.pDist() / 2.0, mapY.p2() + mapY.pDist() / 2.0);
	QPoint requirestPoint(mapX.transform(x), mapY.transform(y));
	QPoint deltaPoint = requirestPoint - centerPoint;
	moveCanvas(deltaPoint.x(), deltaPoint.y());
}

void IceQwtPlotPanner::setIsReplot(bool isReplot)
{
	d_data->isReplot = isReplot;
}

bool IceQwtPlotPanner::isReplot()
{
	return d_data->isReplot;
}

void IceQwtPlotPanner::moveCanvas(int dx, int dy)
{
	if(dx == 0 && dy == 0)
	{
		return;
	}
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	if(!plt)
	{
		return;
	}
	const bool doAutoReplot = plt->autoReplot();
	plt->setAutoReplot(false);

	for(int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		const QwtScaleMap map = plt->canvasMap(axis);
		const double p1 = map.transform(plt->axisScaleDiv(axis).lowerBound());
		const double p2 = map.transform(plt->axisScaleDiv(axis).upperBound());

		QwtInterval interval; // +
		double d1, d2;
		if(axis == QwtPlot::xBottom || axis == QwtPlot::xTop)
		{
			d1 = map.invTransform(p1 - dx);
			d2 = map.invTransform(p2 - dx);
			interval = plt->rasterItem()->interval(Qt::XAxis); // +
		}
		else
		{
			d1 = map.invTransform(p1 - dy);
			d2 = map.invTransform(p2 - dy);
			interval = plt->rasterItem()->interval(Qt::YAxis); // +
		}

		if(isCanSet(d1, d2, interval)) // +
		{
			plt->setAxisScale(axis, d1, d2, plt->axisStepSize(axis));
		}
	}
	plt->setAutoReplot(doAutoReplot);

	// TODO ���� ��� ��������� - ��� �� ���� ��� isCanSet. ���� �� ����, ��
	// ������ ��������������� � �������������� �� ����
	if(isReplot())
	{
		//plt->replot();
		// TODO �� ����� ����������. � ���������� ������ setScaleRegionRect ����� ������ replot,
		// ������� ������� updateAxes, �� ���� setScaleRegionRect ����� ���������� ������� 
		// updateAxes ��� ��� ��� setScaleRegionRect ����� ��� ����������� ���
		plt->updateAxes();
		plt->rasterItem()->setScaleRegionRect();
	}
	else
	{
		plt->updateAxes();
	}
}

void IceQwtPlotPanner::widgetMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	// ��� ������ ����������� ����������� ��� ��������� �� ������� � ������������, ���
	// ���� paintEvent � ������� �����������. ����� ���� ��� ����������� ��������� (������
	// ��� �����), ������� �������� paintEvent � �� ������������ �� �����������, �������
	// ���� �� ����������� - ��� �������� �������� �������� ����������� ����� �����������.
	// ����� ����������� � �������� (��. ����) ��������� �������� � ���� �������������� 
	// ����������� �� ��� �����������, �.�. �������� �������� ����������� ����� �����������
	((QwtPlotCanvas*)(canvas()))->setPaintAttribute(QwtPlotCanvas::BackingStore, false);
	((QwtPlotCanvas*)(canvas()))->setPaintAttribute(QwtPlotCanvas::BackingStore, true);
	QwtPanner::widgetMouseReleaseEvent(mouseEvent);
}

bool IceQwtPlotPanner::isCanSet(double& d1, double& d2, const QwtInterval& interval)
{
	// ����� ���������� ������������ �������� ����

	bool wasSwapped = false;
	if(d2 < d1)
	{
		qSwap(d2, d1);
		wasSwapped = true;
	}
	double delta = d2 - d1;

	if(delta >= interval.width())
	{
		return false;
	}
	if(d1 < interval.minValue())
	{
		if(d2 > interval.maxValue())
		{
			d2 = interval.maxValue();
			d1 = d2 - delta;
		}
		else
		{
			d1 = interval.minValue();
			d2 = d1 + delta;
		}
	}
	if(d2 > interval.maxValue())
	{
		if(d1 < interval.minValue())
		{
			d1 = interval.minValue();
			d2 = d1 + delta;
		}
		else
		{
			d2 = interval.maxValue();
			d1 = d2 - delta;
		}
	}

	if(wasSwapped)
	{
		qSwap(d2, d1);
	}
	return true;
}

QPixmap IceQwtPlotPanner::grab() const
{

	// ��������� ������ ���������, ����� ��� ����������� �����������
	// ������ ��������� �� �������� � ��� ������������� �����������

	const QwtPlotLayout* layout = plot()->plotLayout();
	QRect rect = canvas()->rect();
	rect.setRight(rect.right() - layout->canvasMargin(QwtPlot::yRight) + 2); //  +2 �� ���� ������ - �������� ???
	rect.setBottom(rect.bottom() - layout->canvasMargin(QwtPlot::xBottom) + 2);

	// ��� ����� �� QwtPanner::grab()
#if QT_VERSION >= 0x050000
	return parentWidget()->grab(parentWidget()->rect());
#else
	return QPixmap::grabWidget(parentWidget(), rect);
#endif
}

} // namespace ICEQWT_NS
