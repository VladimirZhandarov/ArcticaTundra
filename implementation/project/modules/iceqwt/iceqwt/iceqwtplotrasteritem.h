//==============================================================================
// ��������:        ������� ����������� �����������
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
// ��� �����: IceQwtPlotRasterItem.h
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOTRASTERITEM_H
#define ICEQWTPLOTRASTERITEM_H

#include "iceqwtdef.h"

#include <qwt_plot_rasteritem.h>

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtPlotRasterItem : public QwtPlotRasterItem
{

public:
	IceQwtPlotRasterItem(const QString& title = QString::null);
	virtual ~IceQwtPlotRasterItem();

	void setImageBuilder(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);

	double width() const;
	double height() const;

	void setScaleRegionRect() const;
	double getScaleFactory(Qt::Axis axis) const;

	virtual QwtInterval interval(Qt::Axis axis) const;

	void setOrigin(const QPointF& origin);
	const QPointF& origin() const;
	double origin(Qt::Axis axis) const;

protected:
	virtual QImage renderImage(const QwtScaleMap& xMap, const QwtScaleMap& yMap,
		const QRectF& area, const QSize& imageSize) const;

	virtual QwtScaleMap imageMap(Qt::Orientation orientation, const QwtScaleMap& map,
		const QRectF& area, const QSize& imageSize, double pixelSize) const;

private:
	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOTRASTERITEM_H
