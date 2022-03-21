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
// Имя файла: iceqwtscaledraw.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "iceqwtscaledraw.h"

#include "qwt_painter.h"

#include <QtGui/QPalette>
#include <QtGui/QPainter>

namespace ICEQWT_NS
{

class IceQwtScaleDraw::PrivateData
{
public:
	PrivateData() : cursorPos(0)
	{

	}

	int cursorPos;
};

IceQwtScaleDraw::IceQwtScaleDraw() : QwtScaleDraw()
{
	d_data = new PrivateData();
}

IceQwtScaleDraw::~IceQwtScaleDraw()
{
	delete d_data;
}

// Установить положение курсора
void IceQwtScaleDraw::setCursorPos(int cursorPos)
{
	d_data->cursorPos = cursorPos;
}

void IceQwtScaleDraw::draw(QPainter* painter, const QPalette& palette) const
{
	QwtScaleDraw::draw(painter, palette);

	// Отрисовываем на осях положение курсора
	painter->save();
	QPen pen = painter->pen();
	pen.setStyle(Qt::DashLine);
	painter->setPen(pen);

	if(d_data->cursorPos > 0)
	{
		if(orientation() == Qt::Vertical)
		{
			QwtPainter::drawLine(painter, 0, d_data->cursorPos, pos().x(),
				d_data->cursorPos);
		}
		else
		{
			QwtPainter::drawLine(painter, d_data->cursorPos, 0, d_data->cursorPos,
				pos().y());
		}
	}
	painter->restore();
}

} // namespace ICEQWT_NS

