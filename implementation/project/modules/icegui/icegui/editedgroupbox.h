#ifndef EDITEDGROUPBOX_H
#define EDITEDGROUPBOX_H

#include "iceguidef.h"

#include <QtGui/QGroupBox>

class QLineEdit;

namespace ICEGUI_NS
{

class ICEGUI_EXPORT EditedGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	EditedGroupBox(QWidget* parent = 0);
	EditedGroupBox(const QString& title, QWidget* parent = 0);
	~EditedGroupBox();

protected slots:
	void onEditingFinished();

protected:
	void init();
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	QRect getTextRect() const;

private:
	QLineEdit* m_editor;
};

} // namespace ICEGUI_NS

#endif // EDITEDGROUPBOX_H
