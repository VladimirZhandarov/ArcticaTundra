//==============================================================================
// ��������:        ������� ����������� �����������
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
// ��� �����: IceQwtPlotRasterItem.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "iceqwtplotrasteritem.h"

#include "iceqwtplot.h"
#include "iceunitratioqwttransform.h"

#include <iceimage/imagebuilder.h>

#include <qwt_scale_engine.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>

namespace ICEQWT_NS
{

class IceQwtPlotRasterItem::PrivateData
{
public:
	PrivateData()
	{

	}

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder;
	QPointF origin;
};

//! �����������
IceQwtPlotRasterItem::IceQwtPlotRasterItem(const QString& title) : QwtPlotRasterItem(title)
{
	d_data = new PrivateData;
	setRenderHint(RenderAntialiased, true);
	setPaintAttribute(PaintInDeviceResolution, true);
	QwtPainter::setRoundingAlignment(false);
}

//! ����������
IceQwtPlotRasterItem::~IceQwtPlotRasterItem()
{
	delete d_data;
}

/*!
  \brief ���������� ����������� �����������
*/
void IceQwtPlotRasterItem::setImageBuilder(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	d_data->imageBuilder = imageBuilder;
}

void IceQwtPlotRasterItem::setScaleRegionRect() const
{
	IceQwtPlot* plt = (IceQwtPlot*)plot();

	QwtInterval intervalX = plt->axisInterval(QwtPlot::xTop);
	QwtInterval intervalImageX = interval(Qt::XAxis);
	intervalX &= intervalImageX;

	QwtInterval intervalY = plt->axisInterval(QwtPlot::yLeft).normalized();
	QwtInterval intervalImageY = interval(Qt::YAxis);
	intervalY &= intervalImageY;

	QRectF rect(intervalX.minValue(), intervalY.minValue(), intervalX.width(),
		intervalY.width());
	rect = plt->transform(rect);
	d_data->imageBuilder->setScaleRegionRect(rect.toRect());
}

/*!
  \return ���������� ����������� �����������
*/
//const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& IceQwtPlotRasterItem::getImageBuilder() const
//{
//	return d_data->imageBuilder;
//}

/*!
  \brief �������� ������ ����������� � ����������� ����
*/
double IceQwtPlotRasterItem::width() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imgBldr = d_data->imageBuilder;
	if(imgBldr.isNull())
	{
		return 0;
	}
	double widthInPK = imgBldr->getImageSizeWidth();
	// ����� ������ ������������� ������������� �� ������ ���������� �������������� ���.
	// ����� ������� �� ���������� ������ ����������� ��� �������� � ������������� 
	// ������������ ���������������. �� �� ����� ����� ������ ������������������� �����������
	// �� imgBldr - ��� ��� ��� ��������� ������ ����� ��������������� �����������, � ������
	// ����� ����� �������������� ��� �� ��������������� (�������� � magnifer), ����� ���� 
	// "�������������" ������ ������������������� �����������. ������ ����������� � �����������
	// ���� �� ������� �� ��������, ������� �� ����� ������ �������������, ��� "������" �������
	// ���������
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	IceUnitRatioQwtTransform* transform = (IceUnitRatioQwtTransform*)plt->axisScaleEngine(QwtPlot::xTop)->transformation();
	if(transform)
	{
		return transform->invTransform(widthInPK); 
	}
	else
	{
		return widthInPK; 
	}
}

/*!
  \brief �������� ������ ����������� � ����������� ����
*/
double IceQwtPlotRasterItem::height() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imgBldr = d_data->imageBuilder;
	if(imgBldr.isNull())
	{
		return 0;
	}
	double heightInPK = imgBldr->getImageSizeHeight();

	// ��. ����������� � width()
	IceQwtPlot* plt = (IceQwtPlot*)plot();
	IceUnitRatioQwtTransform* transform = (IceUnitRatioQwtTransform*)plt->axisScaleEngine(QwtPlot::yLeft)->transformation();
	if(transform)
	{
		return transform->invTransform(heightInPK);
	}
	else
	{
		return heightInPK;
	}
}

/*!
  \brief �������� ������������ ��������������� ���
*/
double IceQwtPlotRasterItem::getScaleFactory(Qt::Axis axis) const
{
	switch(axis)
	{
		case Qt::XAxis:
		{
			if(d_data->imageBuilder.isNull())
			{
				return 0;
			}
			return d_data->imageBuilder->getScaleFactorX();
		}
		case Qt::YAxis:
		{
			if(d_data->imageBuilder.isNull())
			{
				return 0;
			}
			return d_data->imageBuilder->getScaleFactorY();
		}
		default:
		{
			return 0;
		}
	}
}

QwtInterval IceQwtPlotRasterItem::interval(Qt::Axis axis) const
{
	switch(axis)
	{
		case Qt::XAxis:
		{
			return QwtInterval(d_data->origin.x(), d_data->origin.x() + width());
		}
		case Qt::YAxis:
		{
			return QwtInterval(d_data->origin.y(), d_data->origin.y() + height());
		}
		default:
		{
			return QwtInterval();
		}
	}
}

/*!
  \brief ���������� ������ ���������
*/
void IceQwtPlotRasterItem::setOrigin(const QPointF& origin)
{
	d_data->origin = origin;
}

/*!
  \brief �������� ������ ���������
*/
const QPointF& IceQwtPlotRasterItem::origin() const
{
	return d_data->origin;
}

double IceQwtPlotRasterItem::origin(Qt::Axis axis) const
{
	if(Qt::XAxis == axis)
	{
		return d_data->origin.x();
	}
	else
	{
		return d_data->origin.y();
	}
}

QImage IceQwtPlotRasterItem::renderImage(const QwtScaleMap& xMap, const QwtScaleMap& yMap,
	const QRectF& /*area*/, const QSize& /*imageSize*/) const
{
	/*
	����� �� ��� �������� ���������������, �������������� ��� ������������������ ����������� �
	����� �� ������������ �������� ��� ������������ ������� ��������� ����������� 
	�� ������������������� �����������
	*/
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = d_data->imageBuilder;
	Q_ASSERT(!imageBuilder.isNull());
	return imageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::SCALED_RESULT_IMAGE,
		QRect(qRound(xMap.p1()), qRound(yMap.p1()), qRound(xMap.pDist() + 1),
		qRound(yMap.pDist() + 1)));
}

QwtScaleMap IceQwtPlotRasterItem::imageMap(Qt::Orientation orientation, 
	const QwtScaleMap& map, const QRectF& area, const QSize& imageSize, 
	double pixelSize) const
{
	Q_ASSERT(pixelSize == 0);
	QwtScaleMap mmap = QwtPlotRasterItem::imageMap(orientation, map, area, imageSize, pixelSize);

	const IceQwtPlot* plt = (IceQwtPlot*)plot();
	QwtPlot::Axis axisId = Qt::Horizontal == orientation ? QwtPlot::xTop : QwtPlot::yLeft;
	double d1 = plt->transform(axisId, mmap.s1());
	double d2 = plt->transform(axisId, mmap.s2());
	// ����������� ��������������� ��� ������� ������������ �������� ������������������� �����������
	mmap.setPaintInterval(d1 * getScaleFactory(Qt::XAxis), d2 * getScaleFactory(Qt::XAxis));

	/*
	������ �-�� ���������� ��� ��� ������������ ��� ��� � ��� �������������� ���
	��������. ��� ���� �������� ������������ ��������������� ��� ������ �� ����
	���� ����������, ������� � ������ ����, ��� ������ ��������� ��� Y ����������
	� ������� ����� ���� (���������� �� ���������� � ������ ����� ����). ����� 
	��������� ��� �� ���������� ������ � QwtPlot::init()
	*/
	return mmap;
}

} // namespace ICEQWT_NS
