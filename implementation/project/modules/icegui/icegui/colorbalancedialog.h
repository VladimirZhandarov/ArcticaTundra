#ifndef COLORBALANCEDIALOG_H
#define COLORBALANCEDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_colorbalancedialog.h"

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ColorBalanceDialog : public QDialog
{
	Q_OBJECT
public:
	ColorBalanceDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ColorBalanceDialog();

public slots:
	virtual void reject();

private slots:
	void onRedBalanceChanged(int value);
	void onGreenBalanceChanged(int value);
	void onBlueBalanceChanged(int value);

private:
	Ui::ColorBalanceDialog ui;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
	int m_startRedBalance;
	int m_startGreenBalance;
	int m_startBlueBalance;
};

} // namespace ICEGUI_NS

#endif // COLORBALANCEDIALOG_H