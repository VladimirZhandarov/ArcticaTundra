#ifndef IMAGECUTUPDIALOG_H
#define IMAGECUTUPDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_imagecutupdialog.h"

namespace ICEIMAGE_NS
{
	class ImageBuilder;
	class ImageCutUpThread;
}

namespace ICEQWT_NS
{
	class IceQwtPlot;
	class IceQwtPlotGrid;
}

namespace ICECORE_NS
{
	class Settings;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ImageCutUpDialog : public QDialog
{
	Q_OBJECT
public:
	ImageCutUpDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		ICEQWT_NS::IceQwtPlot* plot, ICECORE_NS::Settings* settings,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ImageCutUpDialog();

protected:
	void closeEvent(QCloseEvent* e);

protected slots:
	void partWidthPointDSB_valueChanged(double value);
	void partHeightPointDSB_valueChanged(double value);
	void partWidthCountDSB_valueChanged(double value);
	void partHeightCountDSB_valueChanged(double value);
	void keepAspectRatioChB_stateChanged(int state);
	void dirTB_clicked();
	void startPB_clicked();
	void openResultFolderPB_clicked();
	void imageCutUpThread_finished();
	void onResultCountMaximumCalculated(int value);

private:
	void blockChildSignals(bool block);
	void checkEnabledStartPB();
	int calcMaxSteps() const;

	Ui::ImageCutUpDialog ui;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
	ICEQWT_NS::IceQwtPlot* m_plot;
	ICECORE_NS::Settings* m_settings;
	ICEQWT_NS::IceQwtPlotGrid* m_grid;
	bool m_lastChangedWidth;
	ICEIMAGE_NS::ImageCutUpThread* m_imageCutUpThread;
	int m_resultCountMaximum;
};

} // namespace ICEGUI_NS

#endif // IMAGECUTUPDIALOG_H
