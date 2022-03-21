#ifndef IMAGERESIZEDIALOG_H
#define IMAGERESIZEDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_imageresizedialog.h"

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ImageResizeDialog : public QDialog
{
	Q_OBJECT
public:
	ImageResizeDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ImageResizeDialog();

public slots:
	virtual void accept();

protected slots:
	void widthPointSB_valueChanged(int value);
	void heightPointSB_valueChanged(int value);
	void widthPercentDSB_valueChanged(double value);
	void heightPercentDSB_valueChanged(double value);
	void keepAspectRatioChB_stateChanged(int state);

private:
	void blockChildSignals(bool block);

	Ui::ImageResizeDialog ui;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
	bool m_lastChangedWidth;
};

} // namespace ICEGUI_NS

#endif // IMAGERESIZEDIALOG_H
