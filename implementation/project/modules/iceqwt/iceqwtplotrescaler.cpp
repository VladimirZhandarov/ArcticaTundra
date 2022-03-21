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
	IceQwtPlotRescaler::AxisData axisData[Qt::ZAxis]; // Qt::ZAxis - ������������ ������ ��� ���-�� ����
	MODE_APPLY modeApply;
};

IceQwtPlotRescaler::IceQwtPlotRescaler(QWidget* canvas, QwtPlotRescaler::RescalePolicy rescalePolicy,
	int referenceAxis) : QwtPlotRescaler(canvas, referenceAxis, rescalePolicy)
{
	d_data = new PrivateData;

	// ��� ����� ���������� �������� ��������� ����������, ��� ������� ����� referenceAxis �
	// ������� �������. �� ������� ��������� ������� ������ ��������� ������������� ����, ���
	// "������" ������� ���������
	setAspectRatio(1);
}

IceQwtPlotRescaler::~IceQwtPlotRescaler()
{
	delete d_data;
}

/*!
   \brief ���������� ������� �����������
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
   \brief �������� ������� �����������
*/
double IceQwtPlotRescaler::scaleFactor(Qt::Axis axis) const
{
	return d_data->axisData[axis].scaleFactor;
}

/*!
   \brief ������ ����� ���������� ����������� ����. ��� ������
	 REPLOT ����� ������ replot, ��� ������� ����� ������ updateAxis, �����
	 ����������� �������. ��� ������ UPDATE_AXIS ����� ������ ������ updateAxis
*/
void IceQwtPlotRescaler::setModeApply(MODE_APPLY modeApply)
{
	d_data->modeApply = modeApply;
}

/*!
   \brief �������� ����� ���������� ����������� ����
*/
IceQwtPlotRescaler::MODE_APPLY IceQwtPlotRescaler::modeApply() const
{
	return d_data->modeApply;
}

/*!
   \brief ������������� ��������������� ���� (�� �������, �� ��� ���
	 �������� setReference). ��� ���������� ����, ���������� ���� �� �������
	 �� �������� � ���������������, ������� ��������� ��������������� ����
	 ���������� �� ��� �� ���������� ��� � ��������� �������� ���
*/
QwtInterval IceQwtPlotRescaler::syncScale(int axis, const QwtInterval& reference,
	const QSize& size) const
{
	switch((int)rescalePolicy())
	{
		case ExpandingSmart:
		{
			// ��� � ���������� ����, ��� ����� ���������� ���������
			return expandScale(axis, QSize(), QSize());
		}
		default:
		{
			return IceQwtPlotRescaler::syncScale(axis, reference, size);
		}
	}
}

/*!
   \brief �������� ���. ��� ��������� ��� �� ������������ �� oldSize �
		newSize. ��� ������� � ������� ������ ������������ ��� ����������� 
		�������� ������� ���. ���� �������� ����������� �������� �� ������ � 
		����������� �������� ����. ������ ������� �������, ��� ������� ���������
		����� ����� ����� ������� ������ ����� � �������� ������� �� QwtScaleMap
*/
QwtInterval IceQwtPlotRescaler::expandScale(int axisId, const QSize& oldSize,
	const QSize& newSize) const
{
	// oldSize � newSize ������������, ��� ��� ��� �� ���� ������� ���������
	// ��������� ������� ������� (���������� ��������)

	IceQwtPlot* plt = (IceQwtPlot*)plot();
	switch((int)rescalePolicy())
	{
		case ExpandingSmart:
		{
			// ����������� ������� ���������� �� ������ ��� � ����������� ����. ���
			// ������ �� 4 ���� ���� ������ ����� ���� ������, ��� ��� �� ����� ��������
			// �� ������ ��������� �����, ������� ��������������� � �������������.
			QwtInterval hintInterval = intervalHint(axisId);
			if(!hintInterval.isValid())
			{
				return QwtInterval();
			}
			// ����������� ������� ������� ������� � �������� ����������� ����. ������ ����
			// �������� ������������ ������ ������������� oldSize � newSize
			QwtScaleMap scaleMap = plt->canvasMap(axisId);
			// �������� ������������� ���
			QwtTransform* transform = plt->axisScaleEngine(axisId)->transformation();
			double width = 0;
			// �� ��������, ��� ������������� ������� ICE ������������� ������ ���� �����������
			// ������. ��� �� ����� �������� ���� ������ ...
			// ������ ��� � ����������� ���� ������ ���� ��������� � ������������ � ������������
			// ����� � ��������. �� ���� ��� ����� ������ ���
			Qt::Axis axis = plt->classAxis((QwtPlot::Axis)axisId);
			if(transform)
			{
				width = transform->invTransform(scaleMap.pDist()) / d_data->axisData[axis].scaleFactor;
			}
			else
			{
				width = scaleMap.invTransform(scaleMap.pDist()) / d_data->axisData[axis].scaleFactor;
			}
			// �������� ������ ����� ��� �� ��������� �� �������
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
   \brief ����������� ����� �������� ��� ������ �� ������� ����������� �� ���� ���,
	 ������� ��������� � ������ ������� ���
*/
QwtInterval IceQwtPlotRescaler::expandInterval(const QwtInterval &oldInterval, 
	double futureWidth, const QwtInterval& hintInterval) const
{
	//����� ��������������� �������� ��������� ����
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
		// ������� ����� ������ ������� ��������� ��� ��������, ����� ����������� �����.
		// ����� ��� ����������� ����������� �� ��������
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

	// ��������� ���� ���������� ���� ��� ��������� ������� �������, ���� ��� 
	// ��������������� (��� �����������, ��������� ���� ����� ��������� ������ ��
	// ������� �������� ���� � ��� ������������� ��������� � ������� ������ (��� � 
	// ���������� � panner)). ��� ��������� ������� ��� ������������� �������� replot, ���
	// ��� replot ������� ������ updateAxes ��� � ����������� �������. ����������� �������
	// � ��� ����� �������, ��� ��� ����� ������� ��������� ������� ������� ������� ������� 
	// �����������
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
