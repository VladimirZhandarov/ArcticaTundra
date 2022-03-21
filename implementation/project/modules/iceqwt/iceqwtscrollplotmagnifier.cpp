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
// Имя файла: iceqwtscrollplotmagnifier.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "iceqwtscrollplotmagnifier.h"
#include "iceqwtplotrasteritem.h"

#include "iceqwtplot.h"
#include "scrollbar.h"

#include <iceimage/imagebuilder.h>

#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include <QtCore/QEvent>

namespace ICEQWT_NS
{

class ScrollData
{
public:
	ScrollData() : scrollBar(0), mode(Qt::ScrollBarAsNeeded)
	{
	
	}

	~ScrollData()
	{
		delete scrollBar;
	}

	ScrollBar* scrollBar;
	Qt::ScrollBarPolicy mode;
};

//! Конструктор
IceQwtScrollPlotMagnifier::IceQwtScrollPlotMagnifier(QWidget* canvas) : IceQwtPlotMagnifier(canvas),
	d_cornerWidget(0), d_hScrollData(0), d_vScrollData(0), d_inZoom(false)
{
	if(!canvas)
	{
		return;
	}
	d_hScrollData = new ScrollData;
	d_vScrollData = new ScrollData;
}

//! Деструктор
IceQwtScrollPlotMagnifier::~IceQwtScrollPlotMagnifier()
{
	delete d_cornerWidget;
	delete d_vScrollData;
	delete d_hScrollData;
}

void IceQwtScrollPlotMagnifier::rescale(double factor)
{
	IceQwtPlotMagnifier::rescale(factor);
	updateScrollBars();
}

void IceQwtScrollPlotMagnifier::restorePoint()
{
	IceQwtPlotMagnifier::restorePoint();
	updateScrollBars();
}

void IceQwtScrollPlotMagnifier::updateScrollBars()
{
	if(!canvas())
	{
		return;
	}
	const int xAxis = QwtPlot::xTop;
	const int yAxis = QwtPlot::yLeft;

	// Скрол может устанавливаться либо на оси, либо напротив ее
	// как захочет пользователь
	int xScrollBarAxis = QwtPlot::xBottom;// xAxis;
	int yScrollBarAxis = QwtPlot::yRight;// yAxis;

	QwtPlotLayout* layout = plot()->plotLayout();

	// Определяем, нужно ли отображать горизонтальный скролл
	bool showHScrollBar = needScrollBar(Qt::Horizontal);
	if(showHScrollBar)
	{
		ScrollBar* sb = scrollBar(Qt::Horizontal);
		sb->setPalette(plot()->palette());
		sb->setInverted(!plot()->axisScaleDiv(xAxis).isIncreasing());
		sb->setBase(zoomBase().left(), zoomBase().right());
		sb->moveSlider(zoomRect().left(), zoomRect().right());
		if(!sb->isVisibleTo(canvas()))
		{
			sb->show();
			layout->setCanvasMargin(layout->canvasMargin(xScrollBarAxis) + sb->extent(),
				xScrollBarAxis);
		}
	}
	else
	{
		if(horizontalScrollBar())
		{
			horizontalScrollBar()->hide();
			layout->setCanvasMargin(layout->canvasMargin(xScrollBarAxis) -
				horizontalScrollBar()->extent(), xScrollBarAxis);
		}
	}

	bool showVScrollBar = needScrollBar(Qt::Vertical);
	if(showVScrollBar)
	{
		ScrollBar* sb = scrollBar(Qt::Vertical);
		sb->setPalette(plot()->palette());
		sb->setInverted(!plot()->axisScaleDiv(yAxis).isIncreasing());
		sb->setBase(zoomBase().top(), zoomBase().bottom());
		sb->moveSlider(zoomRect().top(), zoomRect().bottom());

		if(!sb->isVisibleTo(canvas()))
		{
			sb->show();
			layout->setCanvasMargin(layout->canvasMargin(yScrollBarAxis)+ sb->extent(),
				yScrollBarAxis);
		}
	}
	else
	{
		if(verticalScrollBar())
		{
			verticalScrollBar()->hide();
			layout->setCanvasMargin(layout->canvasMargin(yScrollBarAxis) -
				verticalScrollBar()->extent(), yScrollBarAxis);
		}
	}

	if(showHScrollBar && showVScrollBar)
	{
		if(!d_cornerWidget)
		{
			d_cornerWidget = new QWidget(canvas());
			d_cornerWidget->setAutoFillBackground(true);
			d_cornerWidget->setPalette(plot()->palette());
		}
		d_cornerWidget->show();
	}
	else
	{
		if(d_cornerWidget)
		{
			d_cornerWidget->hide();
		}
	}
	layoutScrollBars(canvas()->contentsRect());
	plot()->updateLayout();
}

/*!
   \brief Определение, нужно ли отображать скролл
*/ 
bool IceQwtScrollPlotMagnifier::needScrollBar(Qt::Orientation orientation) const
{
	Qt::ScrollBarPolicy mode;
	double zoomMin, zoomMax, baseMin, baseMax;

	if(orientation == Qt::Horizontal)
	{
		mode = d_hScrollData->mode;
		// Размер базового изображения в шкалах
		baseMin = zoomBase().left();
		baseMax = zoomBase().right();
		// Размер отображаемого канваса в шкалах
		zoomMin = zoomRect().left();
		zoomMax = zoomRect().right();
	}
	else
	{
		mode = d_vScrollData->mode;
		baseMin = zoomBase().top();
		baseMax = zoomBase().bottom();
		zoomMin = zoomRect().top();
		zoomMax = zoomRect().bottom();
	}

	bool needed = false;
	switch(mode)
	{
		case Qt::ScrollBarAlwaysOn:
		{
			needed = true; // Всегда отображаем скролл
			break;
		}
		case Qt::ScrollBarAlwaysOff:
		{
			needed = false;
			break;
		}
		default:
		{
			// Отображаем скролл тогда, когда базовое изображение
			// не вмещается в размер отображаемого канваса
			if(baseMin < zoomMin || baseMax > zoomMax)
			{
				needed = true;
			}
			break;
		}
	}
	return needed;
}

ScrollBar *IceQwtScrollPlotMagnifier::scrollBar(Qt::Orientation orientation)
{
	ScrollBar *&sb = (orientation == Qt::Vertical) ? d_vScrollData->scrollBar :
		d_hScrollData->scrollBar;

	if(!sb)
	{
		sb = new ScrollBar(orientation, canvas());
		if(orientation == Qt::Vertical)
		{
			sb->setInvertedAppearance(true);
		}
		sb->hide();
		connect(sb, SIGNAL(valueChanged(Qt::Orientation, double, double)),
			this, SLOT(scrollBarMoved(Qt::Orientation, double, double)));
	}
	return sb;
}

QRectF IceQwtScrollPlotMagnifier::zoomBase() const
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	return plt->rasterItem()->boundingRect();
}

QRectF IceQwtScrollPlotMagnifier::zoomRect() const
{
	QPointF leftTop(plot()->axisScaleDiv(QwtPlot::xTop).lowerBound(),
		plot()->axisScaleDiv(QwtPlot::yLeft).upperBound()); 
	QPointF rightBottom(plot()->axisScaleDiv(QwtPlot::xTop).upperBound(),
		plot()->axisScaleDiv(QwtPlot::yLeft).lowerBound());
	return QRectF(leftTop, rightBottom);
}

ScrollBar* IceQwtScrollPlotMagnifier::horizontalScrollBar() const
{
	return d_hScrollData->scrollBar;
}

ScrollBar* IceQwtScrollPlotMagnifier::verticalScrollBar() const
{
	return d_vScrollData->scrollBar;
}

void IceQwtScrollPlotMagnifier::layoutScrollBars(const QRect& rect)
{
	int hPos = QwtPlot::xBottom;
	int vPos = QwtPlot::yRight;
	ScrollBar* hScrollBar = horizontalScrollBar();
	ScrollBar* vScrollBar = verticalScrollBar();
	const int hdim = hScrollBar ? hScrollBar->extent() : 0;
	const int vdim = vScrollBar ? vScrollBar->extent() : 0;

	if(hScrollBar && hScrollBar->isVisible())
	{
		int x = rect.x();
		int y = (hPos == QwtPlot::xTop) ? rect.top() : rect.bottom() - hdim + 1;
		int w = rect.width();
		if(vScrollBar && vScrollBar->isVisible())
		{
			if(vPos == QwtPlot::yLeft)
			{
				x += vdim;
			}
			w -= vdim;
		}
		hScrollBar->setGeometry(x, y, w, hdim);
	}
	if(vScrollBar && vScrollBar->isVisible())
	{
		int pos = QwtPlot::yRight;
		int x = (vPos == QwtPlot::yLeft) ? rect.left() : rect.right() - vdim + 1;
		int y = rect.y();
		int h = rect.height();

		if(hScrollBar && hScrollBar->isVisible())
		{
			if(hPos == QwtPlot::xTop)
			{
				y += hdim;
			}
			h -= hdim;
		}
		vScrollBar->setGeometry(x, y, vdim, h);
	}
	if(hScrollBar && hScrollBar->isVisible() && vScrollBar && vScrollBar->isVisible())
	{
		if(d_cornerWidget)
		{
			QRect cornerRect(vScrollBar->pos().x(), hScrollBar->pos().y(), vdim, hdim);
			d_cornerWidget->setGeometry(cornerRect);
		}
	}
}

void IceQwtScrollPlotMagnifier::scrollBarMoved(Qt::Orientation o, double min, double max)
{
	Q_UNUSED(max);

	IceQwtPlot* plt = (IceQwtPlot*)plot();
	if(o == Qt::Horizontal)
	{
		plt->setAxisScale(QwtPlot::xTop, min, max, plt->axisStepSize(QwtPlot::xTop));
	}
	else
	{
		plt->setAxisScale(QwtPlot::yLeft, max, min, plt->axisStepSize(QwtPlot::yLeft));
	}
	plt->rasterItem()->setScaleRegionRect();
	plt->replot();

}

void IceQwtScrollPlotMagnifier::pannedFinished(int /*dx*/, int /*dy*/)
{
	updateScrollBars();
}

bool IceQwtScrollPlotMagnifier::eventFilter(QObject* object, QEvent* event)
{
	if(object == canvas())
	{
		switch(event->type())
		{
			case QEvent::Resize:
			case QEvent::Show:
			{
				updateScrollBars();
				break;
			}
			case QEvent::ChildRemoved:
			{
				const QObject* child = static_cast<QChildEvent*>(event)->child();
				if(child == d_cornerWidget)
				{
					d_cornerWidget = 0;
				}
				else if(child == d_hScrollData->scrollBar)
				{
					d_hScrollData->scrollBar = 0;
				}
				else if(child == d_vScrollData->scrollBar)
				{
					d_vScrollData->scrollBar = 0;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
	return IceQwtPlotMagnifier::eventFilter(object, event);
}

} // namespace ICEQWT_NS
