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
// ��� �����: iceqwtplotpicker.h
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#ifndef ICEQWTPLOTPICKER_H
#define ICEQWTPLOTPICKER_H

#include "iceqwtdef.h"

#include <qwt_plot_picker.h>

#include <QtCore/QList>

class QwtPlotCurve;

namespace ICEQWT_NS
{

class ICEQWT_EXPORT IceQwtPlotPicker : public QwtPlotPicker
{
	Q_OBJECT
public:

	enum MODE
	{
		NONE = 0x01, 
		MOVE_AREA = 0x02,      // ����������� �������
		MOVE_POINT_AREA = 0x04, // �������������� ����� �������
		SELECT_POINT_AREA = 0x08 // ��������� ����� �������
		//CREATE_AREA = 0x16,    // �������� ����� �������
	};

	typedef QFlags<MODE> Modes;

	IceQwtPlotPicker(QWidget* parent);
	~IceQwtPlotPicker();

	void setBand(QwtPicker::RubberBand band, bool closed = true);

	void deleteAllAreas();

	virtual bool eventFilter(QObject* object, QEvent* event);

	QVariant getSelectedArea() const;
	//void cleanSelectedArea();

signals:
	void selectedAreaChanged();

protected slots:
	void onSelected(const QRectF& rect);
	void onSelected(const QVector<QPointF>& dpa);

private:
	void moveArea(const QPoint& pos);
	void createArea(const QVector<QPointF>& v);
	void createArea(const QRectF& r);

	void movePoint(const QPoint& pos);

	Modes defineModes(const QPoint& pos, bool changeData = true);

	bool handleMousePressEvent(const QPoint& pos);
	bool handleMouseMoveEvent(const QPoint& pos);
	bool handleMouseReleaseEvent(const QPoint& pos); 

	void highlightPoints();
	void highlightPoint(bool highlight = true);

	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // ICEQWTPLOTPICKER_H
