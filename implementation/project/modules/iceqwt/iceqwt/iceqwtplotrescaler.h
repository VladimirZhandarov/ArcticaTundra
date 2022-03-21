#ifndef ICEQWTPLOTRESCALER_H
#define ICEQWTPLOTRESCALER_H

#include "iceqwtdef.h"

#include <qwt_plot_rescaler.h>

namespace ICEQWT_NS
{

/*
	��� ������������� ExpandSmart, referenceAxis ����� �� �����������. 
	��������� ���� ���� ���������� �� ������ � ���� �� ��������� � 
	����������� �� ��������� ������������� ���, ��� "�����s" ������� ���������
	�����. ������� ����� ����������� � ��������������� ������� interval � ��������
	����� � �������� �������
*/

class ICEQWT_EXPORT IceQwtPlotRescaler: public QwtPlotRescaler
{
public:
	enum ExpandingDirection
	{
		// "�����" ��������� �������� ����
		ExpandSmart = QwtPlotRescaler::ExpandBoth + 1
	};

	enum RescalePolicy
	{
		// "�����" ��������� �������� ����
		ExpandingSmart = QwtPlotRescaler::Fitting + 1
	};

	// ����� ������ rescale ���������� c������:
	enum MODE_APPLY
	{
		REPLOT, // replot (����� ������� ��������� ���� � ������ ����������� �����)
		UPDATE_AXIS, // ������ ��������� ���
		NONE // ������ �� ������
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