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
// Имя файла: iceqwtscrollplotmagnifier.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef ICEQWTSCROLLPLOTMAGNIFIER_H
#define ICEQWTSCROLLPLOTMAGNIFIER_H

#include "iceqwtdef.h"

#include "iceqwtplotmagnifier.h"

#include <QtCore/QRectF>

class ScrollBar;

namespace ICEQWT_NS
{

class ScrollData;

class ICEQWT_EXPORT IceQwtScrollPlotMagnifier : public IceQwtPlotMagnifier
{
	Q_OBJECT
public:
	IceQwtScrollPlotMagnifier(QWidget* canvas);
	~IceQwtScrollPlotMagnifier();

	virtual bool eventFilter(QObject* object, QEvent* event);

signals:
	void moved(double dx, double dy);

public slots:
	void pannedFinished(int dx, int dy);

protected:
	virtual void rescale(double factor);
	virtual ScrollBar* scrollBar(Qt::Orientation orientation);
	virtual void updateScrollBars();
	virtual void layoutScrollBars(const QRect& rect);
	virtual void restorePoint();

private slots:
	void scrollBarMoved(Qt::Orientation o, double min, double max);

private:
	bool needScrollBar(Qt::Orientation orientation) const;
	QRectF zoomBase() const;
	QRectF zoomRect() const;
	ScrollBar *horizontalScrollBar() const;
	ScrollBar *verticalScrollBar() const;

	ScrollData* d_hScrollData;
	ScrollData* d_vScrollData;
	QWidget* d_cornerWidget;
	bool d_inZoom;
};

} // namespace ICEQWT_NS

#endif // ICEQWTSCROLLPLOTMAGNIFIER_H
