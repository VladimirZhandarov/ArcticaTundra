#include "imagecutupdialog.h"

#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>

#include <QtCore/QUrl>

#include <icecore/settings.h>

#include <iceimage/gdalimagebuilder.h>
#include <iceimage/imagecutupthread.h>

#include <iceqwt/iceqwtplot.h>
#include <iceqwt/iceqwtplotgrid.h>
#include <iceqwt/iceunitratioqwttransform.h>

#include <qwt_scale_engine.h>

namespace ICEGUI_NS
{

ImageCutUpDialog::ImageCutUpDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	ICEQWT_NS::IceQwtPlot* plot, ICECORE_NS::Settings* settings, QWidget* parent,
	Qt::WindowFlags f) : QDialog(parent, f), m_imageBuilder(imageBuilder), m_plot(plot),
	m_settings(settings), m_lastChangedWidth(true), m_imageCutUpThread(0), m_resultCountMaximum(0)
{
	ui.setupUi(this);

	Q_ASSERT(m_plot);
	m_grid = new ICEQWT_NS::IceQwtPlotGrid();
	m_grid->attach(m_plot);
	m_grid->setAxes(QwtPlot::xTop, QwtPlot::yLeft);
	m_grid->setPen(Qt::red, 1.0, Qt::DashDotLine);
	m_grid->show();

	Q_ASSERT(!m_imageBuilder.isNull());
	quint64 width = m_imageBuilder->getImageSizeWidth();
	quint64 height = m_imageBuilder->getImageSizeHeight();
	ui.totalWidthValueL->setText(QString::number(width));
	ui.totalHeightValueL->setText(QString::number(height));
	quint64 maxImageSize = ICEIMAGE_NS::GdalImageBuilder::getMaxImageSize();
	maxImageSize = maxImageSize / 4;

	ui.partWidthPointDSB->setMaximum (width);
	ui.partHeightPointDSB->setMaximum (height);

	if(maxImageSize > 1.0)
	{
		double count = ((double)(width * height)) / maxImageSize;
		ui.partWidthPointDSB->setValue(width / count);
		ui.partHeightPointDSB->setValue(height / count);
	}
	else
	{
		ui.partWidthPointDSB->setValue(width);
		ui.partHeightPointDSB->setValue(height);
	}
	Q_ASSERT(m_settings);
	ui.dirLE->setText(m_settings->getImageCutUpSaveDir());
}

ImageCutUpDialog::~ImageCutUpDialog()
{

}

void ImageCutUpDialog::closeEvent(QCloseEvent* event)
{
	Q_ASSERT(m_grid);
	m_grid->detach();
	delete m_grid;
	Q_ASSERT(m_plot);
	m_plot->replot();
	event->accept();
}

void ImageCutUpDialog::partWidthPointDSB_valueChanged(double value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	double count = m_imageBuilder->getImageSizeWidth() / value;
	ui.partWidthCountDSB->setValue(count);
	m_lastChangedWidth = true;
	Q_ASSERT(m_grid);
	Q_ASSERT(m_plot);
	ICEQWT_NS::IceUnitRatioQwtTransform* transform = (ICEQWT_NS::IceUnitRatioQwtTransform*)
		m_plot->axisScaleEngine(QwtPlot::xTop)->transformation();
	if(transform && transform->unit() != ICEIMAGE_NS::NONE)
	{
		m_grid->setHStepMetric(transform->invTransform(value));
	}
	else
	{
		m_grid->setHStepPixel(transform->invTransform(value));
	}
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.partHeightPointDSB->setValue(value / factor);
		if(transform && transform->unit() != ICEIMAGE_NS::NONE)
		{
			m_grid->setVStepMetric(transform->invTransform(value) / factor);
		}
		else
		{
			m_grid->setVStepPixel(value / factor);
		}
		ui.partHeightCountDSB->setValue(count);
	}
	m_grid->setIsOnLine(false);
	Q_ASSERT(m_plot);
	m_plot->replot();
	checkEnabledStartPB();
	ui.resultCountValueL->setText(QString::number(calcMaxSteps()));
	blockChildSignals(false);
}

void ImageCutUpDialog::partHeightPointDSB_valueChanged(double value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	double count = m_imageBuilder->getImageSizeHeight() /value;
	ui.partHeightCountDSB->setValue(count);
	m_lastChangedWidth = false;
	Q_ASSERT(m_grid);
	Q_ASSERT(m_plot);
	ICEQWT_NS::IceUnitRatioQwtTransform* transform = (ICEQWT_NS::IceUnitRatioQwtTransform*)
		m_plot->axisScaleEngine(QwtPlot::yLeft)->transformation();
	if(transform && transform->unit() != ICEIMAGE_NS::NONE)
	{
		m_grid->setVStepMetric(transform->invTransform(value));
	}
	else
	{
		m_grid->setVStepPixel(value);
	}
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.partWidthPointDSB->setValue(value * factor);
		if(transform && transform->unit() != ICEIMAGE_NS::NONE)
		{
			m_grid->setHStepMetric(transform->invTransform(value) * factor);
		}
		else
		{
			m_grid->setHStepPixel(value * factor);
		}
		ui.partWidthCountDSB->setValue(count);
	}
	m_grid->setIsOnLine(false);
	Q_ASSERT(m_plot);
	m_plot->replot();
	checkEnabledStartPB();
	ui.resultCountValueL->setText(QString::number(calcMaxSteps()));
	blockChildSignals(false);
}

void ImageCutUpDialog::partWidthCountDSB_valueChanged(double value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	ui.partWidthPointDSB->setValue(m_imageBuilder->getImageSizeWidth() / value);
	m_lastChangedWidth = true;
	Q_ASSERT(m_grid);
	Q_ASSERT(m_plot);
	ICEQWT_NS::IceUnitRatioQwtTransform* transform = (ICEQWT_NS::IceUnitRatioQwtTransform*)
		m_plot->axisScaleEngine(QwtPlot::xTop)->transformation();
	if(transform && transform->unit() != ICEIMAGE_NS::NONE)
	{
		m_grid->setHStepMetric(transform->invTransform(m_imageBuilder->getImageSizeWidth()) / value);
	}
	else
	{
		m_grid->setHStepPixel(m_imageBuilder->getImageSizeWidth() / value);
	}
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.partHeightPointDSB->setValue(m_imageBuilder->getImageSizeHeight() / value);
		if(transform && transform->unit() != ICEIMAGE_NS::NONE)
		{
			m_grid->setVStepMetric(transform->invTransform(m_imageBuilder->getImageSizeHeight()) / value);
		}
		else
		{
			m_grid->setVStepPixel(m_imageBuilder->getImageSizeHeight() / value);
		}
		ui.partHeightCountDSB->setValue(value);
	}
	m_grid->setIsOnLine(false);
	Q_ASSERT(m_plot);
	m_plot->replot();
	checkEnabledStartPB();
	ui.resultCountValueL->setText(QString::number(calcMaxSteps()));
	blockChildSignals(false);
}

void ImageCutUpDialog::partHeightCountDSB_valueChanged(double value)
{
	blockChildSignals(true);
	Q_ASSERT(!m_imageBuilder.isNull());
	ui.partHeightPointDSB->setValue(m_imageBuilder->getImageSizeHeight() / value);
	m_lastChangedWidth = false;
	Q_ASSERT(m_grid);
	Q_ASSERT(m_plot);
	ICEQWT_NS::IceUnitRatioQwtTransform* transform = (ICEQWT_NS::IceUnitRatioQwtTransform*)
		m_plot->axisScaleEngine(QwtPlot::xTop)->transformation();
	if(transform && transform->unit() != ICEIMAGE_NS::NONE)
	{
		m_grid->setVStepMetric(transform->invTransform(m_imageBuilder->getImageSizeHeight()) / value);
	}
	else
	{
		m_grid->setVStepPixel(m_imageBuilder->getImageSizeHeight() / value);
	}
	if(ui.keepAspectRatioChB->checkState() != Qt::Unchecked)
	{
		double factor = ((double)m_imageBuilder->getImageSizeWidth()) / m_imageBuilder->getImageSizeHeight();
		ui.partWidthPointDSB->setValue(m_imageBuilder->getImageSizeWidth() / value);
		if(transform && transform->unit() != ICEIMAGE_NS::NONE)
		{
			m_grid->setHStepMetric(transform->invTransform(m_imageBuilder->getImageSizeWidth()) / value);
		}
		else
		{
			m_grid->setHStepPixel(m_imageBuilder->getImageSizeWidth() / value);
		}
		ui.partWidthCountDSB->setValue(value);
	}
	m_grid->setIsOnLine(false);
	Q_ASSERT(m_plot);
	m_plot->replot();
	checkEnabledStartPB();
	ui.resultCountValueL->setText(QString::number(calcMaxSteps()));
	blockChildSignals(false);
}

void ImageCutUpDialog::keepAspectRatioChB_stateChanged(int state)
{
	if(state != Qt::Unchecked)
	{
		if(m_lastChangedWidth)
		{
			partWidthPointDSB_valueChanged(ui.partWidthPointDSB->value());
		}
		else
		{
			partHeightPointDSB_valueChanged(ui.partHeightPointDSB->value());
		}
	}
}

void ImageCutUpDialog::dirTB_clicked()
{
	Q_ASSERT(m_settings);
	QString dir = QFileDialog::getExistingDirectory(this, tr("Выбор директории"),
		m_settings->getImageCutUpSaveDir());
	if(!dir.isEmpty())
	{
		m_settings->setImageCutUpSaveDir(dir);
	}
	ui.dirLE->setText(dir);
	ui.openResultFolderPB->setEnabled(!dir.isEmpty());
	checkEnabledStartPB();
}

void ImageCutUpDialog::startPB_clicked()
{
	QSharedPointer<ICEIMAGE_NS::GdalImageBuilder> imageBuilder =
		qSharedPointerDynamicCast<ICEIMAGE_NS::GdalImageBuilder>(m_imageBuilder);
	if(imageBuilder.isNull())
	{
		return;
	}
	if(!m_imageCutUpThread)
	{
		int maxSteps = calcMaxSteps();
		Q_ASSERT(m_settings);
		int jpegQuality = m_settings->getImageAddedSettings().jpegQuality;
		m_imageCutUpThread = new ICEIMAGE_NS::ImageCutUpThread(imageBuilder,
			ui.dirLE->text(), QSizeF(ui.partWidthPointDSB->value(),
			ui.partHeightPointDSB->value()), maxSteps, jpegQuality, this);
		connect(m_imageCutUpThread, SIGNAL(maximumCalculated(int)),
			ui.progressBar, SLOT(setMaximum(int)));
		connect(m_imageCutUpThread, SIGNAL(maximumCalculated(int)),
			this, SLOT(onResultCountMaximumCalculated(int)));
		connect(m_imageCutUpThread, SIGNAL(valueChanged(int)),
			ui.progressBar, SLOT(setValue(int)));
		connect(m_imageCutUpThread, SIGNAL(finished()),
			this, SLOT(imageCutUpThread_finished()));
		m_imageCutUpThread->start();
		ui.startPB->setText(tr("Отмена"));
		ui.closePB->setEnabled(false);
	}
	else
	{
		m_imageCutUpThread->stop();
		m_imageCutUpThread->wait();
		delete m_imageCutUpThread;
		m_imageCutUpThread = 0;
		ui.startPB->setText(tr("Начать"));
		ui.closePB->setEnabled(true);
	}
}

void ImageCutUpDialog::openResultFolderPB_clicked()
{
	 QDesktopServices::openUrl(QUrl("file:///" + ui.dirLE->text(), QUrl::TolerantMode));
}

void ImageCutUpDialog::imageCutUpThread_finished()
{
	bool cutIsOk = false;
	if(m_imageCutUpThread)
	{
		cutIsOk = m_imageCutUpThread->isNoError();
		m_imageCutUpThread->stop();
		m_imageCutUpThread->wait();
		delete m_imageCutUpThread;
	}
	m_imageCutUpThread = 0;
	ui.startPB->setText(tr("Начать"));
	ui.closePB->setEnabled(true);
	if(cutIsOk)
	{
		ui.resultCountValueL->setText(QString::number(m_resultCountMaximum));
		QMessageBox::information(this, tr("Разрезка изображения"),
			tr("Разрезка изображения успешно завершена"));
		if(ui.openResultFolderChB->isChecked())
		{
			openResultFolderPB_clicked();
		}
	}
	else
	{
		ui.resultCountValueL->setText("-");
		QMessageBox::warning(this, tr("Разрезка изображения"),
			tr("В процессе разрезки изображения возникли ошибки"));
	}
}

void ImageCutUpDialog::onResultCountMaximumCalculated(int value)
{
	m_resultCountMaximum = value;
}

void ImageCutUpDialog::blockChildSignals(bool block)
{
	ui.partHeightPointDSB->blockSignals(block);
	ui.partWidthPointDSB->blockSignals(block);
	ui.partWidthCountDSB->blockSignals(block);
	ui.partHeightCountDSB->blockSignals(block);
}

void ImageCutUpDialog::checkEnabledStartPB()
{
	if(ui.partWidthCountDSB->value() == 1.0 &&
		ui.partHeightCountDSB->value() == 1.0)
	{
		ui.startPB->setEnabled(false);
	}
	else if(ui.dirLE->text().trimmed().isEmpty())
	{
		ui.startPB->setEnabled(false);
	}
	else
	{
		ui.startPB->setEnabled(true);
	}
}

int ImageCutUpDialog::calcMaxSteps() const
{
	return cvCeil(ui.partWidthCountDSB->value()) * cvCeil(ui.partHeightCountDSB->value());
}

} // namespace ICEGUI_NS
