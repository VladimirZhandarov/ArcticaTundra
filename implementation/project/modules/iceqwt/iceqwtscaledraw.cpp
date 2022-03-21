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
// ��� �����: iceqwtscaledraw.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
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

// ���������� ��������� �������
void IceQwtScaleDraw::setCursorPos(int cursorPos)
{
	d_data->cursorPos = cursorPos;
}

void IceQwtScaleDraw::draw(QPainter* painter, const QPalette& palette) const
{
	QwtScaleDraw::draw(painter, palette);

	// ������������ �� ���� ��������� �������
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

