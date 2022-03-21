#ifndef SCALEDIALOG_H
#define SCALEDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_scaledialog.h"

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ScaleDialog : public QDialog
{
	Q_OBJECT
public:
	ScaleDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ScaleDialog();

	void setCurrentPercentType(int percentType);
	int getCurrentPercentType() const;

private:
	Ui::ScaleDialog ui;	///< Ui класс
};

} // namespace ICEGUI_NS

#endif // SCALEDIALOG_H
