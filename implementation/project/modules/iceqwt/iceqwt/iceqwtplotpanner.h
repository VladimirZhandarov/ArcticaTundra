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
// ��� �����: iceqwtplotpanner.h
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOTPANNER_H
#define ICEQWTPLOTPANNER_H

#include "iceqwtdef.h"

#include <qwt_plot_panner.h>

class QwtInterval;

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtPlotPanner : public QwtPlotPanner
{
	Q_OBJECT
public:
	IceQwtPlotPanner(QWidget* parent);
	~IceQwtPlotPanner();

	void move(const QPoint& pos);
	void moveCenter(double x, double y);

	void setIsReplot(bool isReplot);
	bool isReplot();

protected slots:
	virtual void moveCanvas(int dx, int dy);

protected:
	virtual void widgetMouseReleaseEvent(QMouseEvent* mouseEvent);

private:
	bool isCanSet(double& d1, double& d2, const QwtInterval& interval);
	virtual QPixmap grab() const;

	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOTPANNER_H
