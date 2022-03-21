#ifndef SCALECOMBOBOX_H
#define SCALECOMBOBOX_H

#include "iceguidef.h"

#include <QtGui/QComboBox>

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ScaleComboBox : public QComboBox
{
	Q_OBJECT
public:
	enum ScaleType
	{
		CURRENT_SCALE = 0,
		TO_SIZE_SCALE = -1,
		TO_HEIGHT_SCALE = -2,
		TO_WIDTH_SCALE = -3
	};

	ScaleComboBox(QWidget* parent = 0);
	~ScaleComboBox();

	void addPercent(unsigned int percent);
	void setCurrentPercentType(int percentType);
	int getCurrentPercentType() const;
	void setScaleType(int scaleType);
	int getScaleType() const;

	void zoomIn();
	void zoomOut();

public slots:
	void onActivated(int index);
protected slots:
	void onEditingFinished();

signals:
	void scaleChanged(int scalePercentType);

private:
	QString getString(int type);
	int m_scaleType;
};

} // namespace ICEGUI_NS

#endif // SCALECOMBOBOX_H
