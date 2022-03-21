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
// ��� �����: iceqwtscaledraw.h
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#ifndef ICEQWTSCALEDRAW_H
#define ICEQWTSCALEDRAW_H

#include "iceqwtdef.h"

#include <qwt_scale_draw.h>

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtScaleDraw : public QwtScaleDraw
{
public:
	IceQwtScaleDraw();
	virtual ~IceQwtScaleDraw();

	void setCursorPos(int cursorPos);

	virtual void draw(QPainter* painter, const QPalette& palette) const;

private:
	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTSCALEDRAW_H
