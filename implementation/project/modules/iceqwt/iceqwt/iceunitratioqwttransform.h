#ifndef UNITRATIO_H
#define UNITRATIO_H

#include "iceqwtdef.h"

#include <qwt_transform.h>

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <iceimage/global.h>

class QwtPlot;

namespace ICEQWT_NS
{

class IceQwtPlotRasterItem;

class ICEQWT_EXPORT IceUnitRatioQwtTransform : public QwtTransform
{
public:
	IceUnitRatioQwtTransform();
	virtual ~IceUnitRatioQwtTransform();

	static QString unitName(ICEIMAGE_NS::UNIT unit);
	ICEIMAGE_NS::UNIT unit() const;
	void unit(QString& str) const;
	void setUnit(ICEIMAGE_NS::UNIT unit);

	double ratio() const;
	void setRatio(double ratio);
	void setRatio(double ratio, ICEIMAGE_NS::UNIT unit);

	ICEIMAGE_NS::UNIT metricUnit() const;
	double metricRatio() const;

	virtual double transform(double value) const;
	virtual double invTransform(double value) const;

	QPointF transform(const QPointF& point) const;
	QPointF invTransform(const QPointF& point) const;

	QRectF transform(const QRectF& rect) const;
	QRectF invTransform(const QRectF& rect) const;

	virtual QwtTransform* copy() const;

private:
	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // UNITRATIO_H
