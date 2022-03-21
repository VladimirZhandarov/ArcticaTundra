#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <QtGui/QScrollBar>

class ScrollBar: public QScrollBar
{
	Q_OBJECT
public:
	ScrollBar(QWidget* parent = 0);
	ScrollBar(Qt::Orientation orientation, QWidget* parent = 0);
	ScrollBar(double minBase, double maxBase, Qt::Orientation orientation,
		QWidget* parent = 0);

	void setInverted(bool inverted);
	bool isInverted() const;

	double minBaseValue() const;
	double maxBaseValue() const;

	double minSliderValue() const;
	double maxSliderValue() const;

	int extent() const;

signals:
	void sliderMoved(Qt::Orientation, double, double);
	void valueChanged(Qt::Orientation, double, double);

public slots:
	virtual void setBase(double min, double max);
	virtual void moveSlider(double min, double max);

protected:
	void sliderRange(int value, double& min, double& max) const;
	int mapToTick(double v) const;
	double mapFromTick(int tick) const;

protected slots:
	void onValueChanged(int value);
	void onSliderMoved(int value);

private:
	void init();

	bool m_inverted;
	double m_minBase;
	double m_maxBase;
	int m_baseTicks;
};

#endif // SCROLLBAR_H
