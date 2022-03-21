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
// Имя файла: iceqwtplotpicker.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
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
	} findPointData; // Выбранная точка для редактирования

	PrivateData(): findArea(0), closedPolyLine(true)
	{

	}

	QList<QwtPlotCurve*> createdAreas; // Набор выделенных областей
	QwtPlotCurve* findArea; // Выбранная область для редактирования
	MODE mode;
	QPoint prevPos; // Предыдущая точка курсора
	QVector<PointData> highlightPoints; // Подсвечиваемые точки
	bool closedPolyLine;

	static void setStyleArea(QwtPlotCurve* area);

	void deleteAllAreas();
	Modes defineModes(const QPoint& pos, bool changeData);
};

const double IceQwtPlotPicker::PrivateData::dist = 5;

/*!
  \brief Формирование вектора из 8 точек на основе прямоугольника

	(8)0---1---2 0-я и 8-я точка должны быть одинаковые
	   |       | 
	   |       |
	   7       3
	   |       |
	   |       |
	   6---5---4
*/
static QVector<QPointF> vctr(const QRectF& r)
{
	// 9 точек от 0 до 8, 9-я точка такая же как и 0-я
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
  \brief Получение прямоугольника по области
*/
static QRectF rct(const QwtPlotCurve& area)
{
	QVector<QPointF> v = ((QwtArraySeriesData<QPointF>*)(area.data()))->samples();
	return QRectF(v[0], v[4]);
}

/*!
  \brief Изменение r в зависимости от того, какая точка r 
  (определяется index, см. vctr()) была изменена на точку p. 
  Изменение происходит с сохранением конфигурации прямоугольника
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

	// Для выделения прямоугольником
	connect(this, SIGNAL(selected(const QRectF&)), this, SLOT(onSelected(const QRectF&)));

	// Для выделения полилинией
	connect(this, SIGNAL(selected(const QVector<QPointF>&)), this, SLOT(onSelected(const QVector<QPointF>&)));
}

IceQwtPlotPicker::~IceQwtPlotPicker()
{
	delete d_data;
}

/*!
  \brief Установка способа выделения области
*/
void IceQwtPlotPicker::setBand(QwtPicker::RubberBand band, bool closed)
{
	switch(band)
	{
		// Выделение прямоугольником
		case QwtPicker::RectRubberBand:
		{
			setRubberBand(RectRubberBand);
			setStateMachine(new QwtPickerDragRectMachine());
			break;
		}
		// Выделение элипсом
		case QwtPicker::EllipseRubberBand:
		{
			setRubberBand(EllipseRubberBand);
			setStateMachine(new QwtPickerDragRectMachine());
			break;
		}
		// Выделение полигоном
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
			//// Если курсор мыши попал в область опорной точки выделенной области,
			//// то необходимо не начинать процесс создания новой области, а начинать
			//// процесс редактирования этой точки, в данном случае - ее перемещение
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
  \brief Получение выбранной области. Выбранная область возвращается
  в размерности базового изображения
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
				return QVariant(r.toRect()); // В данной версии программы может быть только одно выделение
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
				return result; // В данной версии программы может быть только одно выделение
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
	// Замыкаем полилинию
	QVector<QPointF> vctr(v);
	if(d_data->closedPolyLine)
	{
		vctr.push_back(vctr[0]);
	}
	createArea(vctr); 
}

/*!
  \brief Задать стили области
*/
void IceQwtPlotPicker::PrivateData::setStyleArea(QwtPlotCurve* area)
{
	QColor color(Qt::red);
	area->setPen(color, 2);
	area->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::gray, color, QSize(8, 8)));
	area->setAxes(QwtPlot::xTop, QwtPlot::yLeft);
}

/*!
  \brief Создание области на основе прямоугольника
*/
void IceQwtPlotPicker::createArea(const QRectF& r)
{
	createArea(vctr(r));
}

/*!
  \brief Удаление всех областей
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
  \brief Удаление всех областей с перерисовкой плота
*/
void IceQwtPlotPicker::deleteAllAreas()
{
	d_data->deleteAllAreas();
	plot()->replot();
	emit selectedAreaChanged();
}

/*!
  \brief Создание области на основе полилинии
*/
void IceQwtPlotPicker::createArea(const QVector<QPointF>& v)
{
	QwtPlot* plt = plot();

	// Создание новой области
	QwtPlotCurve* area = new QwtPlotCurve();
	IceQwtPlotPicker::PrivateData::setStyleArea(area);
	area->setSamples(v);

	// Удаление всех других областеей. 
	// Область должна быть только одна
	d_data->deleteAllAreas();

	// Регистрируем созданную область
	area->attach(plt);
	d_data->createdAreas.push_back(area);
	plt->replot();

	emit selectedAreaChanged();
}


/*!
  \brief Получить возможные режимы работы в зависимости от положения курсора
*/
IceQwtPlotPicker::Modes IceQwtPlotPicker::defineModes(const QPoint& pos, bool changeData)
{
	/*
		Определяем, это режим редактирования точки или нет. Ищем ближайшую точку
		у каждой созданной области к положению курсора с учетом критерия dist
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
			modes |= MOVE_POINT_AREA;   // Может быть для перемещения 
			modes |= SELECT_POINT_AREA; // или для выделения с целью, например удаления
		}
		// Заодно проверяем, какой выбранной области принадлежит точка курсора.
		// Это одновременно получение данных для режима перемещения области. Тем
		// не менее режим MOVE_POINT_AREA имеет приоритет перед MOVE_AREA
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
	// Запоминаем точку курсора
	d_data->prevPos = pos;
	return modes;
}

/*!
  \brief Передвинуть область 
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
  \brief Передвинуть точку области
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
			// Последняя и первая точка полилинии должны быть одинаковые
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
  \brief Обработчик события нажатия кнопки мыши
*/
bool IceQwtPlotPicker::handleMousePressEvent(const QPoint& pos)
{
	// Определяем возможный режим работы
	Modes modes = defineModes(pos);
	bool handle = true;

	// Приоритетным режимом является режим перемещения и выделения точки
	if(modes & MOVE_POINT_AREA)
	{
		//highlightPoint();
		d_data->mode = MOVE_POINT_AREA;
	}
	// Потом перемещение области
	else if(modes & MOVE_AREA)
	{
		d_data->mode = MOVE_AREA;
	}
	// Потом создание области
	else
	{
		d_data->mode = NONE;
		handle = false;
	}

	// Если это событие для редактирование точки или перемещения 
	// области, то его больше обрабатывать не надо, чтобы не получилось
	// так, что при перемещении области перемещался и кавас
	return handle;
}

/*!
  \brief Обработчик события перемещения мыши
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

	// У всех областей сбрасываем все точки
	for(int i=0; i<d_data->createdAreas.size(); i++)
	{
		QwtPlotCurve* area = d_data->createdAreas[i];
		QwtSymbol* symbol = const_cast<QwtSymbol*>(area->symbol());
		const QBrush brush = symbol->brush();
		symbol->setBrush(symbol->brush().color().dark(10));
		directPainter.drawSeries(area, 0, int(area->dataSize() - 1));
	}

	// Подсвечиваем заданные точки для подсветки
	for(int i=0; i<d_data->highlightPoints.size(); i++)
	{
		IceQwtPlotPicker::PrivateData::PointData pD = d_data->highlightPoints[i];
		QwtSymbol* symbol = const_cast<QwtSymbol*>(pD.area->symbol());
		const QBrush brush = symbol->brush();
		symbol->setBrush(symbol->brush().color().dark(180));
		directPainter.drawSeries(pD.area, pD.index, pD.index);
		//symbol->setBrush(brush); // Возвращаем
	}
}

void IceQwtPlotPicker::highlightPoint(bool highlight)
{
	// Находим текушую найденную точку в списке подсвеченных
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

	// Уже подсвечена
	if(fPD.area && highlight)
	{
		return;
	}
	// Уже выключена
	if(!fPD.area && !highlight)
	{
		return;
	}
	// Отключаем
	if(fPD.area && !highlight)
	{
		d_data->highlightPoints.remove(index);
	}
	else if(!fPD.area && highlight)
	// Подсвечиваем
	{
		d_data->highlightPoints.push_back(d_data->findPointData);
	}
	highlightPoints();
}

} // namespace ICEQWT_NS
