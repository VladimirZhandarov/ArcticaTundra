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
// Имя файла: iceqwtplotmagnifier.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOTMAGNIFIER_H
#define ICEQWTPLOTMAGNIFIER_H

#include "iceqwtdef.h"

#include <qwt_plot_magnifier.h>

#include <QtCore/QSharedPointer>

class QPoint;

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtPlotMagnifier : public QwtPlotMagnifier
{
	Q_OBJECT
public:
	IceQwtPlotMagnifier(QWidget* parent);
	virtual ~IceQwtPlotMagnifier();

	void setImageBuilder(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);

	void setPercentScale(int);

	int getPercentScaleInWidth();
	int getPercentScaleInHeight();
	int getPercentScaleInSize();
	int getCurrentPercentScale() const;

protected:
	virtual void rescale(double factor);
	virtual void widgetWheelEvent(QWheelEvent* wheelEvent);

	void fixPoint(const QPoint& pos);
	virtual void restorePoint();

signals:
	void zoomIn();
	void zoomOut();

private:
	void updateAxis();
	void scale();

	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOTMAGNIFIER_H
