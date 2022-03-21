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
// ��� �����: iceqwtplotmagnifier.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
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

//! �����������
IceQwtPlotMagnifier::IceQwtPlotMagnifier(QWidget* parent) : QwtPlotMagnifier(parent)
{
	d_data = new PrivateData;
}

//! ����������
IceQwtPlotMagnifier::~IceQwtPlotMagnifier()
{
	delete d_data;
}

void IceQwtPlotMagnifier::setImageBuilder(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	d_data->imageBuilder = imageBuilder;
}

/*!
   \brief ���������� ���� � ��������������� �����������
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
	// ��������� ������������� ��������������� 
	plt->rescaler()->setScaleFactor(Qt::XAxis, plt->rescaler()->scaleFactor(Qt::XAxis) * factor);
	plt->rescaler()->setScaleFactor(Qt::YAxis, plt->rescaler()->scaleFactor(Qt::YAxis) * factor);

	// ���������� ����
	IceQwtPlotRescaler::MODE_APPLY modeApply = plt->rescaler()->modeApply();
	plt->rescaler()->setModeApply(IceQwtPlotRescaler::UPDATE_AXIS);
	plt->rescaler()->rescale();
	plt->rescaler()->setModeApply(modeApply);
}

/*!
   \brief ��������������� �����������
*/
void IceQwtPlotMagnifier::scale()
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	// ����� ��������� �������� ����������� ����� ������������� ������ replot
	Q_ASSERT(!d_data->imageBuilder.isNull());
	d_data->imageBuilder->setScaleFactor(plt->rescaler()->scaleFactor(Qt::XAxis), 
		plt->rescaler()->scaleFactor(Qt::YAxis));

	// TODOTODOTODOTODOTODOTODO
	plt->updateAxes();
	plt->rasterItem()->setScaleRegionRect();
}

/*!
   \brief �������� ������� � ��������� ��� ������� ����������� ����������� � ����
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
  \brief �������� ������� � ��������� ��� ������� ����������� ����������� �� ������
*/
int IceQwtPlotMagnifier::getPercentScaleInHeight()
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	// ����� ��� Y
	double intvY = plt->invTransform(QwtPlot::yLeft, plt->canvasMap(QwtPlot::yLeft).pDist());
	// ������ ����������� 
	//double rasterX = icePlot->rasterItem()->height();
	QwtInterval rasterX = plt->rescaler()->intervalHint(QwtPlot::yLeft);
	// ����������� ��������������� �� ��� Y
	double K = intvY / rasterX.width();
	return qRound(K * 100.0);
}

/*!
  \brief �������� ������� � ��������� ��� ������� ����������� ����������� �� ������
*/
int IceQwtPlotMagnifier::getPercentScaleInWidth()
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	// ����� ��� X 
	double intvX = plt->invTransform(QwtPlot::xTop, plt->canvasMap(QwtPlot::xTop).pDist());
	// ������ �����������
	//double rasterX = icePlot->rasterItem()->width();
	QwtInterval rasterX = plt->rescaler()->intervalHint(QwtPlot::xTop);
	// ����������� ��������������� �� ��� Y
	double K = intvX / rasterX.width();
	return qRound(K * 100.0);
}

/*!
  \brief ������������ �����, ��������� ������� ���������� ������� ����� ���������������
*/
void IceQwtPlotMagnifier::fixPoint(const QPoint& pos)
{
	IceQwtPlot* plt = ((IceQwtPlot*)(plot()));

	// ���� ��� ���� ���������� ����������, �� ���� ��������
	if(!d_data->pos.isNull())
	{
		return;
	}
	// ����� �� ��������������� � �������� �������
	d_data->pos = pos;

	// ����� �� ��������������� � ����������� ����
	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);
	d_data->pointBeforeZoom = QwtScaleMap::invTransform(mapX, mapY, d_data->pos);
}

/*!
  \brief ������� ��������� ������������� �����
*/
void IceQwtPlotMagnifier::restorePoint()
{
	IceQwtPlot* plt = (IceQwtPlot*)(plot());
	QwtScaleMap mapX = plt->canvasMap(QwtPlot::xTop);
	QwtScaleMap mapY = plt->canvasMap(QwtPlot::yLeft);
	QPointF pointAfterZoom = QwtScaleMap::transform(mapX, mapY, d_data->pointBeforeZoom);
	QPointF pointDiff = d_data->pos - pointAfterZoom;

	// ���������� �����������, ����� ����� �� ��������������� ���� � ���
	// �� ����� ��� � ����� ���������������, ��� ���� ����� �� ������ ������
	// replot, ��� ��� �� ����� ������ �������� ���� (� ���������� �-��). ��� ��
	// ����� updateAxis ���������� ������, ��� ��� �� �������� scaleDiv � ����, ���
	// ���������� ������
	bool isReplotPanner = plt->panner()->isReplot();
	plt->panner()->setIsReplot(false);
	plt->panner()->move(pointDiff.toPoint());
	plt->panner()->setIsReplot(isReplotPanner);

	// ����� ��� ���������� - ���� �������� ������. ��� ���� �� ���� ������
	// replot � updateAxis, ��� ��� ����� ��������������� (��. ���������� �-��) 
	// ����� ������������� ������ replot � ��� ��� ������� updateAxis
	IceQwtPlotRescaler::MODE_APPLY modeApply = plt->rescaler()->modeApply();
	plt->rescaler()->setModeApply(IceQwtPlotRescaler::NONE);
	plt->rescaler()->rescale();
	plt->rescaler()->setModeApply(modeApply);

	// ���������� ��������� ������������
	d_data->pos = QPoint();
}

/*!
   \brief ���������� ������� � ���������
*/
void IceQwtPlotMagnifier::setPercentScale(int percentScale)
{
	IceQwtPlot* plt = ((IceQwtPlot*)(plot()));

	// ����� ���� �� ���������������
	QRect rc = plt->canvas()->contentsRect();
	QPoint pos = rc.center();
	fixPoint(pos);

	// ���������� ���������� ������� �������� ��� rescale
	rescale((percentScale / 100.0) / plt->rescaler()->scaleFactor(Qt::XAxis));

	// ��������������� �����
	restorePoint();

	// ��������������� �����������
	scale();
}

/*!
   \brief �������� ������� ������� ����������� � ���������
*/
int IceQwtPlotMagnifier::getCurrentPercentScale() const
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	return plt->rescaler()->scaleFactor(Qt::XAxis) * 100;
}

void IceQwtPlotMagnifier::widgetWheelEvent(QWheelEvent* wheelEvent)
{
	// ����� ��������������� ����������� �� ������� ����� ����.
	// � ����� ������ restorePoint() �������� �� ����, ��� ��� ���
	// �-�� ��������� � setPercentScale()
	fixPoint(wheelEvent->pos());

	// ������������
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
