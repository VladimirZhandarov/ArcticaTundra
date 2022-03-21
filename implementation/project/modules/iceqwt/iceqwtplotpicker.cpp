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
// ��� �����: iceqwtplotpicker.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "iceqwtplotpicker.h"

#include "iceqwtplot.h"

#include <qwt_plot.h>
#include <qwt_picker_machine.h>
#include <qwt_picker.h>
#include <qwt_widget_overlay.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_symbol.h>
#include <qwt_plot_directpainter.h>

#include <QtCore/QPointer>

#include <QtGui/QMouseEvent>

namespace ICEQWT_NS
{

class IceQwtPlotPicker::PrivateData
{
public:
	static const double dist;

	struct PointData
	{
		PointData(): dist(-1), index(-1), area(0)
		{

		}

		double dist;
		double index;
		QwtPlotCurve* area;

		void init(double dist, double index, QwtPlotCurve* area)
		{
			this->dist = dist;
			this->index = index;
			this->area = area;
		}

		bool operator==(const PointData& pD)
		{
			return(area == pD.area && index == pD.index);
		}
	} findPointData; // ��������� ����� ��� ��������������

	PrivateData(): findArea(0), closedPolyLine(true)
	{

	}

	QList<QwtPlotCurve*> createdAreas; // ����� ���������� ��������
	QwtPlotCurve* findArea; // ��������� ������� ��� ��������������
	MODE mode;
	QPoint prevPos; // ���������� ����� �������
	QVector<PointData> highlightPoints; // �������������� �����
	bool closedPolyLine;

	static void setStyleArea(QwtPlotCurve* area);

	void deleteAllAreas();
	Modes defineModes(const QPoint& pos, bool changeData);
};

const double IceQwtPlotPicker::PrivateData::dist = 5;

/*!
  \brief ������������ ������� �� 8 ����� �� ������ ��������������

	(8)0---1---2 0-� � 8-� ����� ������ ���� ����������
	   |       | 
	   |       |
	   7       3
	   |       |
	   |       |
	   6---5---4
*/
static QVector<QPointF> vctr(const QRectF& r)
{
	// 9 ����� �� 0 �� 8, 9-� ����� ����� �� ��� � 0-�
	QVector<QPointF> v(9); 

	v[0] = v[8] = r.topLeft();
	v[4] = r.bottomRight();

	v[2] = r.topRight();
	v[6] = r.bottomLeft();

	v[1] = (v[0] + v[2]) / 2.0;
	v[3] = (v[4] + v[2]) / 2.0;
	v[5] = (v[6] + v[4]) / 2.0;
	v[7] = (v[0] + v[6]) / 2.0;
	return v;
}

/*!
  \brief ��������� �������������� �� �������
*/
static QRectF rct(const QwtPlotCurve& area)
{
	QVector<QPointF> v = ((QwtArraySeriesData<QPointF>*)(area.data()))->samples();
	return QRectF(v[0], v[4]);
}

/*!
  \brief ��������� r � ����������� �� ����, ����� ����� r 
  (������������ index, ��. vctr()) ���� �������� �� ����� p. 
  ��������� ���������� � ����������� ������������ ��������������
*/
static void reconf(QRectF& r, int index, const QPointF& p)
{
	switch(index)
	{
		case 0:
		case 8:
		{
			r.setTopLeft(p);
			break;
		}
		case 1:
		{
			r.setTop(p.y());
			break;
		}
		case 2:
		{
			r.setTopRight(p);
			break;
		}
		case 3:
		{
			r.setRight(p.x());
			break;
		}
		case 4:
		{
			r.setBottomRight(p);
			break;
		}
		case 5:
		{
			r.setBottom(p.y());
			break;
		}
		case 6:
		{
			r.setBottomLeft(p);
			break;
		}
		case 7:
		{
			r.setLeft(p.x());
			break;
		}
	}
}

IceQwtPlotPicker::IceQwtPlotPicker(QWidget* parent) : QwtPlotPicker(parent)
{
	d_data = new PrivateData;

	QwtPlotCanvas* canvas = qobject_cast<QwtPlotCanvas*>(plot()->canvas());
	canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
	canvas->setFocus();
	setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton);
	setAxis(QwtPlot::xTop, QwtPlot::yLeft);
	setTrackerMode(ActiveOnly);
	setResizeMode(Stretch);

	QPen pen = rubberBandPen();
	pen.setWidth(3);
	pen.setColor(Qt::red);
	pen.setStyle(Qt::DashLine);
	setRubberBandPen(pen);

	// ��� ��������� ���������������
	connect(this, SIGNAL(selected(const QRectF&)), this, SLOT(onSelected(const QRectF&)));

	// ��� ��������� ����������
	connect(this, SIGNAL(selected(const QVector<QPointF>&)), this, SLOT(onSelected(const QVector<QPointF>&)));
}

IceQwtPlotPicker::~IceQwtPlotPicker()
{
	delete d_data;
}

/*!
  \brief ��������� ������� ��������� �������
*/
void IceQwtPlotPicker::setBand(QwtPicker::RubberBand band, bool closed)
{
	switch(band)
	{
		// ��������� ���������������
		case QwtPicker::RectRubberBand:
		{
			setRubberBand(RectRubberBand);
			setStateMachine(new QwtPickerDragRectMachine());
			break;
		}
		// ��������� �������
		case QwtPicker::EllipseRubberBand:
		{
			setRubberBand(EllipseRubberBand);
			setStateMachine(new QwtPickerDragRectMachine());
			break;
		}
		// ��������� ���������
		case QwtPicker::PolygonRubberBand:
		{
			setRubberBand(PolygonRubberBand);
			setStateMachine(new QwtPickerPolygonMachine());
			break;
		}
	}
	d_data->closedPolyLine = closed;
}

bool IceQwtPlotPicker::eventFilter(QObject* object, QEvent* event)
{
	if(plot() == 0 || object != plot()->canvas())
	{
		return false;
	}

	switch(event->type())
	{
		case QEvent::MouseButtonPress:
		{
			const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			//// ���� ������ ���� ����� � ������� ������� ����� ���������� �������,
			//// �� ���������� �� �������� ������� �������� ����� �������, � ��������
			//// ������� �������������� ���� �����, � ������ ������ - �� �����������
			if(handleMousePressEvent(mouseEvent->pos()))
			{
				return true;
			}
			break;
		}
		case QEvent::MouseMove:
		{
			const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if(mouseEvent->buttons() == Qt::LeftButton)
			{
				if(handleMouseMoveEvent(mouseEvent->pos()))
				{
					return true;
				}
			}
			break;
		}
		case QEvent::MouseButtonRelease:
		{
			const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if(mouseEvent->button() == Qt::LeftButton)
			{
				if(handleMouseReleaseEvent(mouseEvent->pos()))
				{
					return true;
				}
			}
			break;
		}
	}
	return QwtPlotPicker::eventFilter(object, event);
}

/*!
  \brief ��������� ��������� �������. ��������� ������� ������������
  � ����������� �������� �����������
*/
QVariant IceQwtPlotPicker::getSelectedArea() const
{
	const IceQwtPlot* plt = (IceQwtPlot*)plot();
	if(!stateMachine())
	{
		return QVariant();
	}
	switch((int)stateMachine()->selectionType())
	{
		case QwtPickerMachine::RectSelection:
		{
			for(int i=0; i<d_data->createdAreas.size(); i++)
			{
				QRectF r = rct(*d_data->createdAreas[i]);
				r = plt->transform(r);
				return QVariant(r.toRect()); // � ������ ������ ��������� ����� ���� ������ ���� ���������
			}
			break;
		}
		case QwtPickerMachine::PolygonSelection:
		default:
		{
			for(int i=0; i<d_data->createdAreas.size(); i++)
			{
				QList<QVariant> result;
				QVector<QPointF> v = ((QwtArraySeriesData<QPointF>*)(d_data->createdAreas[i]->data()))->samples();
				for(int j=0; j<v.size(); j++)
				{
					result.push_back(plt->transform(v[j]));
				}
				return result; // � ������ ������ ��������� ����� ���� ������ ���� ���������
			}
		}
	}
	return QVariant();
}

void IceQwtPlotPicker::onSelected(const QRectF& r)
{
	createArea(r); 
}

void IceQwtPlotPicker::onSelected(const QVector<QPointF>& v)
{
	// �������� ���������
	QVector<QPointF> vctr(v);
	if(d_data->closedPolyLine)
	{
		vctr.push_back(vctr[0]);
	}
	createArea(vctr); 
}

/*!
  \brief ������ ����� �������
*/
void IceQwtPlotPicker::PrivateData::setStyleArea(QwtPlotCurve* area)
{
	QColor color(Qt::red);
	area->setPen(color, 2);
	area->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::gray, color, QSize(8, 8)));
	area->setAxes(QwtPlot::xTop, QwtPlot::yLeft);
}

/*!
  \brief �������� ������� �� ������ ��������������
*/
void IceQwtPlotPicker::createArea(const QRectF& r)
{
	createArea(vctr(r));
}

/*!
  \brief �������� ���� ��������
*/
void IceQwtPlotPicker::PrivateData::deleteAllAreas()
{
	for(int i=0; i < createdAreas.size(); i++)
	{
		QwtPlotCurve* area = createdAreas[i];
		area->detach();
		delete area;
	}
	createdAreas.clear();
}

/*!
  \brief �������� ���� �������� � ������������ �����
*/
void IceQwtPlotPicker::deleteAllAreas()
{
	d_data->deleteAllAreas();
	plot()->replot();
	emit selectedAreaChanged();
}

/*!
  \brief �������� ������� �� ������ ���������
*/
void IceQwtPlotPicker::createArea(const QVector<QPointF>& v)
{
	QwtPlot* plt = plot();

	// �������� ����� �������
	QwtPlotCurve* area = new QwtPlotCurve();
	IceQwtPlotPicker::PrivateData::setStyleArea(area);
	area->setSamples(v);

	// �������� ���� ������ ���������. 
	// ������� ������ ���� ������ ����
	d_data->deleteAllAreas();

	// ������������ ��������� �������
	area->attach(plt);
	d_data->createdAreas.push_back(area);
	plt->replot();

	emit selectedAreaChanged();
}


/*!
  \brief �������� ��������� ������ ������ � ����������� �� ��������� �������
*/
IceQwtPlotPicker::Modes IceQwtPlotPicker::defineModes(const QPoint& pos, bool changeData)
{
	/*
		����������, ��� ����� �������������� ����� ��� ���. ���� ��������� �����
		� ������ ��������� ������� � ��������� ������� � ������ �������� dist
	*/
	QwtPlot *plt = plot();
	Modes modes = NONE;
	double findDist = IceQwtPlotPicker::PrivateData::dist;
	QwtPlotCurve* currentArea = 0;
	for(QList<QwtPlotCurve*>::iterator iter = d_data->createdAreas.begin();
		iter != d_data->createdAreas.end(); iter++)
	{
		currentArea = static_cast<QwtPlotCurve*>(*iter);
		double currentDist = 0;
		int index = currentArea->closestPoint(pos, &currentDist);
		if(currentDist < findDist)
		{
			if(changeData)
			{
				d_data->findPointData.init(currentDist, index, currentArea);
			}
			findDist = currentDist;
			modes |= MOVE_POINT_AREA;   // ����� ���� ��� ����������� 
			modes |= SELECT_POINT_AREA; // ��� ��� ��������� � �����, �������� ��������
		}
		// ������ ���������, ����� ��������� ������� ����������� ����� �������.
		// ��� ������������ ��������� ������ ��� ������ ����������� �������. ���
		// �� ����� ����� MOVE_POINT_AREA ����� ��������� ����� MOVE_AREA
		double xScale = plt->invTransform(currentArea->xAxis(), pos.x());
		double yScale = plt->invTransform(currentArea->yAxis(), pos.y());
		if(currentArea->boundingRect().contains(xScale, yScale))
		{
			if(changeData)
			{
				d_data->findArea = currentArea;
			}
			modes |= MOVE_AREA;
		}
	}
	// ���������� ����� �������
	d_data->prevPos = pos;
	return modes;
}

/*!
  \brief ����������� ������� 
*/
void IceQwtPlotPicker::moveArea(const QPoint& pos)
{
	QwtPlot* plt = plot();
	QwtPlotCurve* area = d_data->findArea;

	double xScaleBegin = plt->invTransform(area->xAxis(), d_data->prevPos.x());
	double yScaleBegin = plt->invTransform(area->yAxis(), d_data->prevPos.y());
	double xScaleEnd = plt->invTransform(area->xAxis(), pos.x());
	double yScaleEnd = plt->invTransform(area->yAxis(), pos.y());
	QPointF deltaPoint(xScaleEnd-xScaleBegin, yScaleEnd-yScaleBegin);

	QVector<QPointF> movedPoint = ((QwtArraySeriesData<QPointF>*)(area->data()))->samples();
	for(int i=0; i<movedPoint.size(); i++)
	{
		movedPoint[i] += deltaPoint;
	}
	area->setSamples(movedPoint);

	d_data->prevPos = pos;
	// Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
	// updated before we paint the cursor on it.
	QwtPlotCanvas* plotCanvas = qobject_cast<QwtPlotCanvas*>(plt->canvas());
	plotCanvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, true);
	plt->replot();
	plotCanvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);

	emit selectedAreaChanged();
}

/*!
  \brief ����������� ����� �������
*/
void IceQwtPlotPicker::movePoint(const QPoint& pos)
{
	QwtPlot* plt = plot();
	QwtPlotCurve* area = d_data->findPointData.area; 

	double xScale = plt->invTransform(area->xAxis(), pos.x());
	double yScale = plt->invTransform(area->yAxis(), pos.y()); 
	QPointF p(xScale, yScale);

	QVector<QPointF> v = ((QwtArraySeriesData<QPointF>*)(area->data()))->samples();

	QwtPickerMachine::SelectionType st = stateMachine()->selectionType();
	switch(st)
	{
		case QwtPickerMachine::RectSelection:
		{
			QRectF r(v[0], v[4]);
			reconf(r, d_data->findPointData.index, p);
			area->setSamples(vctr(r));
			break;
		}
		case QwtPickerMachine::PolygonSelection:
		default:
		{
			v[d_data->findPointData.index] = p;
			// ��������� � ������ ����� ��������� ������ ���� ����������
			if(d_data->findPointData.index == 0)
			{
				v[int(area->dataSize() - 1)] = p;
			}
			else if(area->dataSize() - 1 == d_data->findPointData.index)
			{
				v[0] = p;
			}
			area->setSamples(v);
		}
	}

	d_data->prevPos = pos;
	// Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
	// updated before we paint the cursor on it.
	QwtPlotCanvas* plotCanvas =  qobject_cast<QwtPlotCanvas*>(plot()->canvas());
	plotCanvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, true);
	plot()->replot();
	plotCanvas->setPaintAttribute(QwtPlotCanvas::ImmediatePaint, false);

	emit selectedAreaChanged();
}

/*!
  \brief ���������� ������� ������� ������ ����
*/
bool IceQwtPlotPicker::handleMousePressEvent(const QPoint& pos)
{
	// ���������� ��������� ����� ������
	Modes modes = defineModes(pos);
	bool handle = true;

	// ������������ ������� �������� ����� ����������� � ��������� �����
	if(modes & MOVE_POINT_AREA)
	{
		//highlightPoint();
		d_data->mode = MOVE_POINT_AREA;
	}
	// ����� ����������� �������
	else if(modes & MOVE_AREA)
	{
		d_data->mode = MOVE_AREA;
	}
	// ����� �������� �������
	else
	{
		d_data->mode = NONE;
		handle = false;
	}

	// ���� ��� ������� ��� �������������� ����� ��� ����������� 
	// �������, �� ��� ������ ������������ �� ����, ����� �� ����������
	// ���, ��� ��� ����������� ������� ����������� � �����
	return handle;
}

/*!
  \brief ���������� ������� ����������� ����
*/
bool IceQwtPlotPicker::handleMouseMoveEvent(const QPoint& pos)
{
	bool handle = true;

	switch(d_data->mode)
	{
		case MOVE_POINT_AREA:
		{
			movePoint(pos);
			break;
		}
		case MOVE_AREA:
		{
			moveArea(pos);
			break;
		}
		case NONE:
		default:
		{
			handle = false;
			break;
		}
	}
	return handle;
}

bool IceQwtPlotPicker::handleMouseReleaseEvent(const QPoint& /*pos*/)
{
	bool handle = true;

	switch(d_data->mode)
	{
		case MOVE_POINT_AREA:
		{
			//highlightPoint(false);
			break;
		}
		//case MOVE_AREA: moveArea(pos); break;
		case NONE:
		default:
		{
			handle = false;
			break;
		}
	}
	return handle;
}

void IceQwtPlotPicker::highlightPoints()
{
	QwtPlotDirectPainter directPainter;

	// � ���� �������� ���������� ��� �����
	for(int i=0; i<d_data->createdAreas.size(); i++)
	{
		QwtPlotCurve* area = d_data->createdAreas[i];
		QwtSymbol* symbol = const_cast<QwtSymbol*>(area->symbol());
		const QBrush brush = symbol->brush();
		symbol->setBrush(symbol->brush().color().dark(10));
		directPainter.drawSeries(area, 0, int(area->dataSize() - 1));
	}

	// ������������ �������� ����� ��� ���������
	for(int i=0; i<d_data->highlightPoints.size(); i++)
	{
		IceQwtPlotPicker::PrivateData::PointData pD = d_data->highlightPoints[i];
		QwtSymbol* symbol = const_cast<QwtSymbol*>(pD.area->symbol());
		const QBrush brush = symbol->brush();
		symbol->setBrush(symbol->brush().color().dark(180));
		directPainter.drawSeries(pD.area, pD.index, pD.index);
		//symbol->setBrush(brush); // ����������
	}
}

void IceQwtPlotPicker::highlightPoint(bool highlight)
{
	// ������� ������� ��������� ����� � ������ ������������
	IceQwtPlotPicker::PrivateData::PointData fPD;
	int index = -1;
	for(int i=0; i<d_data->highlightPoints.size(); i++)
	{
		if(d_data->highlightPoints[i] == d_data->findPointData)
		{
			fPD = d_data->findPointData;
			index = i;
			break;
		}
	}

	// ��� ����������
	if(fPD.area && highlight)
	{
		return;
	}
	// ��� ���������
	if(!fPD.area && !highlight)
	{
		return;
	}
	// ���������
	if(fPD.area && !highlight)
	{
		d_data->highlightPoints.remove(index);
	}
	else if(!fPD.area && highlight)
	// ������������
	{
		d_data->highlightPoints.push_back(d_data->findPointData);
	}
	highlightPoints();
}

} // namespace ICEQWT_NS
