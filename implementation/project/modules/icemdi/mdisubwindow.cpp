//==============================================================================
// Название:        Окно многодокументной зоны
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: shchav $
// Создан:    2011.11.08
// Изменен:   $Date: 2013-04-19 18:46:52 +0400 (РџС‚, 19 Р°РїСЂ 2013) $
// Модуль:    icegui
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: mdisubwindow.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "mdisubwindow.h"

#include "commonwidget.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QProgressDialog>

#include <iceimage/gdalimagebuilder.h>
#include <iceimage/imagecutupsettings.h>
#include <iceimage/imagecutdownthread.h>
#include <iceimage/contouritemmodel.h>

#include <icegui/objectviewdialog.h>
#include <icegui/objecttablewidget.h>

#include <iceqwt/imageeditor.h>
#include <iceqwt/iceqwtplotrescaler.h>
#include <iceqwt/iceqwtplotrasteritem.h>

//TODO refac
#include <iceqwt/iceqwtplot.h>
#include <iceqwt/iceunitratioqwttransform.h>
#include <qwt_scale_engine.h>

#include <icecore/core.h>
#include <icecore/settings.h>

namespace ICEMDI_NS
{

QString MdiSubWindow::strippedName(const QString& fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

MdiSubWindow::MdiSubWindow(QWidget* parent, Qt::WindowFlags flags) : 
	QMdiSubWindow(parent, flags), m_showContoursObjectsAction(0), m_objectViewDialog(0),
	m_imageCutDownThread(0), m_mergeIsOk(false)
{
	// Установка главного виджета mdi-окна
	CommonWidget* commonWidget = new CommonWidget(this, flags);
	connect(commonWidget, SIGNAL(errorProcessed(QString)), this, SIGNAL(errorProcessed(QString)));
	setWidget(commonWidget);

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
	if(!imageBuilder.isNull())
	{
		m_objectViewDialog = new ICEGUI_NS::ObjectViewDialog(imageBuilder, 0,
			Qt::Dialog | Qt::WindowMinimizeButtonHint);
		connect(m_objectViewDialog, SIGNAL(finished(int)), this, SLOT(objectViewDialog_finished(int)));
		connect(commonWidget->getImageEditor(), SIGNAL(mouseDoubleClicked()),
			m_objectViewDialog, SLOT(onMouseDoubleClicked())); //TODO refac
	}

	m_contextMenu = new QMenu(this);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(onCustomContextMenuRequested(const QPoint&)));

	setWindowIcon(QIcon());
	setWindowTitle("*");
	setAttribute(Qt::WA_DeleteOnClose);
}

MdiSubWindow::~MdiSubWindow()
{
	if(m_objectViewDialog)
	{
		m_objectViewDialog->close();
		delete m_objectViewDialog;
	}
	if(m_imageCutDownThread)
	{
		m_imageCutDownThread->stop();
		m_imageCutDownThread->wait();
		delete m_imageCutDownThread;
	}
}

void MdiSubWindow::fillContextMenu()
{
	Q_ASSERT(m_contextMenu);
	m_contextMenu->clear();
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
	if(!imageBuilder.isNull())
	{
		int maskListSize = imageBuilder->getMaskListSize();
		for(int i=0; i<maskListSize; i++)
		{
			QAction* action = m_contextMenu->addAction(tr("Маска ") + QString::number(i + 1) +
				": " + imageBuilder->getMaskName(i));
			action->setData(i);
			connect(action, SIGNAL(triggered(bool)), this, SLOT(showMask_triggered(bool)));
			action->setCheckable(true);
			action->setChecked(true);
			if(imageBuilder->getMatMaskImage(i).empty())
			{
				action->setEnabled(false);
			}
			if(i<imageBuilder->getContourItemModelListSize())
			{
				ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
				if(contourItemModel)
				{
					QAction* action = m_contextMenu->addAction(tr("Объект ") + QString::number(i + 1)
						+ ": " + imageBuilder->getContoursName(i));
					action->setData(i);
					connect(action, SIGNAL(triggered(bool)), this, SLOT(showContours_triggered(bool)));
					action->setCheckable(true);
					action->setChecked(true);
					if(contourItemModel->getMatImage().empty())
					{
						action->setEnabled(false);
					}
				}
			}
			m_contextMenu->addSeparator();
		}
		if(imageBuilder->getNotNullContourItemModelListSize() > 0)
		{
			m_showContoursObjectsAction = m_contextMenu->addAction(tr("Показать объекты"));
			m_showContoursObjectsAction->setCheckable(true);
			m_showContoursObjectsAction->setChecked(false);
			connect(m_showContoursObjectsAction, SIGNAL(triggered(bool)), this, SLOT(showContoursObjectsAction_triggered(bool)));
		}

		int linesListSize = imageBuilder->getLineItemModelListSize();
		for(int i=0; i<linesListSize; i++)
		{
			QAction* action = m_contextMenu->addAction(imageBuilder->getLinesName(i));
			action->setData(i);
			connect(action, SIGNAL(triggered(bool)), this, SLOT(showLines_triggered(bool)));
			action->setCheckable(true);
			action->setChecked(true);
		}
	}
}

bool MdiSubWindow::open(bool cutDown, QWidget* parent)
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	//TODO refac
	if(!cutDown)
	{
		QStringList nameFilters;
		nameFilters << tr("Все файлы (*.*)") << tr("Изображение JPG (*.jpg)") <<
			tr("Изображение TIF (*.tif)") << tr("Изображение PNG (*.png)") << 
			tr("Изображение BMP (*.bmp)");
		QString suffix = settings->getImageLoadSuffix();
		QString filename = QFileDialog::getOpenFileName(parent, tr("Открыть изображение"),
			settings->getImageLoadDir(), nameFilters.join(";;"), &suffix);
		if(filename.isEmpty())
		{
			return false;
		}
		QFileInfo fileInfo(filename);
		if(!open(filename))
		{
			QMessageBox::critical(parent, tr("Ошибка загрузки изображения"),
				tr("Ошибка загрузки изображения"));
			return false;
		}
		settings->setImageLoadDir(fileInfo.absolutePath());
		settings->setImageLoadSuffix(suffix);

		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
		if(imageBuilder.isNull())
		{
			return false;
		}
		//TODO refac
		getImageEditor()->icePlot()->setup();
		getImageEditor()->setWindowModified(false);
		setWindowTitle(strippedName(imageBuilder->getFilename()));
	}
	else
	{
		QString filename = QFileDialog::getOpenFileName(parent, tr("Открыть файл разрезки изображения"),
			settings->getImageCutUpLoadDir(), tr("Файл разрезки изображения (*.") +
			ICEIMAGE_NS::ImageCutUpSettings::suffix + tr(")"));
		if(filename.isEmpty())
		{
			return false;
		}
		QFileInfo fileInfo(filename);
		settings->setImageCutUpLoadDir(fileInfo.absolutePath());
		
		QProgressDialog cutDownProgressDialog(tr(""), tr("Отмена"), 0, 100, this);
		cutDownProgressDialog.setMinimumWidth(300);
		cutDownProgressDialog.setWindowModality(Qt::WindowModal);
		cutDownProgressDialog.setAutoClose(true);
		cutDownProgressDialog.setWindowTitle(tr("Сшивка изображения"));

		QSharedPointer<ICEIMAGE_NS::GdalImageBuilder> imageBuilder =
			qSharedPointerDynamicCast<ICEIMAGE_NS::GdalImageBuilder>(getImageBuilder());
		if(imageBuilder.isNull())
		{
			return false;
		}
		m_imageCutDownThread = new ICEIMAGE_NS::ImageCutDownThread(imageBuilder,
			fileInfo, this);
		connect(m_imageCutDownThread, SIGNAL(maximumCalculated(int)),
			&cutDownProgressDialog, SLOT(setMaximum(int)));
		connect(m_imageCutDownThread, SIGNAL(valueChanged(int)),
			&cutDownProgressDialog, SLOT(setValue(int)));
		connect(m_imageCutDownThread, SIGNAL(finished()),
			this, SLOT(imageCutDownThread_finished()));

		connect(&cutDownProgressDialog, SIGNAL(canceled()),
			m_imageCutDownThread, SLOT(stop()));

		m_imageCutDownThread->start();
		cutDownProgressDialog.exec();
		ICEIMAGE_NS::UNIT unit = ICEIMAGE_NS::NONE;
		double divK = 1.0;
		if(m_imageCutDownThread)
		{
			m_imageCutDownThread->wait();
			m_mergeIsOk = m_imageCutDownThread->isNoError();
			unit = m_imageCutDownThread->getUnit();
			divK = m_imageCutDownThread->getDivK();
			delete m_imageCutDownThread;
			m_imageCutDownThread = 0;
		}
		//TODO refac
		//getImageEditor()->icePlot()->setup();

		ICEQWT_NS::IceQwtPlot* plot = getImageEditor()->icePlot();
		Q_ASSERT(plot);
		ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
		Q_ASSERT(settings);
		ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
		if(!imageLineSettings.isUseUserParam)
		{
			imageLineSettings = settings->getImageLineSettingsDefault();
		}
		imageLineSettings.unit = unit;
		imageLineSettings.ratio = 1.0 / divK;
		plot->setupLine(imageLineSettings);
		ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();
		if(!imageGridSettings.isUseUserParam)
		{
			imageGridSettings = settings->getImageGridSettingsDefault();
		}
		plot->setupGrid(imageGridSettings);
		for(int axis=0; axis<QwtPlot::axisCnt; axis++)
		{
			if(plot->classAxis(QwtPlot::Axis(axis)) == Qt::XAxis)
			{
				plot->rescaler()->setIntervalHint(axis, plot->rasterItem()->interval(Qt::XAxis));
			}
			else
			{
				plot->rescaler()->setIntervalHint(axis, plot->rasterItem()->interval(Qt::YAxis));
			}
		}
		plot->replot();
		getImageEditor()->setWindowModified(false);
		setWindowTitle(strippedName(imageBuilder->getFilename()));
	}
	return true;
}

// Для открытия при известном имени файла (например из списка
// недавно открытых файлов)
bool MdiSubWindow::open(const QString& filename)
{
	if(getImageEditor()->load(filename))
	{
		setWindowTitle(strippedName(filename));
		return true;
	}
	else
	{
		return false;
	}
}

bool MdiSubWindow::open(const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView,
	const QString& title)
{
	if(getImageEditor()->load(image, backgroundImageMaskView))
	{
		setWindowTitle(title);
		if(m_objectViewDialog)
		{
			m_objectViewDialog->setWindowTitle(tr("Найденные объекты: ") + title);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MdiSubWindow::save()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
	return save(getFilename(), imageAddedSettings.jpegQuality);
}

bool MdiSubWindow::saveAs(QWidget* parent)
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	QStringList nameFilters;
	nameFilters << tr("Изображение JPG (*.jpg)") <<
		tr("Изображение TIF (*.tif)") << tr("Изображение PNG (*.png)") << 
		tr("Изображение BMP (*.bmp)");
	QString suffix = settings->getImageSaveSuffix();
	QString usedSuffix = suffix.mid(suffix.indexOf("(") + 3,
		suffix.indexOf(")") - suffix.indexOf("*") - 2);
	QDir dir(settings->getImageSaveDir());
	QString fileName = windowTitle().trimmed();
	QFileInfo fileInfo(fileName);
	if(!fileName.isEmpty() && dir.exists(fileInfo.completeBaseName() + "." + usedSuffix))
	{
		QString baseFileName = fileInfo.completeBaseName();
		for(int i=1;i<99999;i++)
		{
			fileName = baseFileName + "_" + QString::number(i);
			if(!dir.exists(fileName + "." + usedSuffix))
			{
				break;
			}
		}
	}
	QString saveDirName = settings->getImageSaveDir();
	fileName = fileName.replace("/", "_");
	fileName = fileName.replace("\\", "_");
	fileName = fileName.replace(":", "_");
	fileName = fileName.replace("*", "_");
	fileName = fileName.replace("?", "_");
	fileName = fileName.replace("\"", "_");
	fileName = fileName.replace("<", "_");
	fileName = fileName.replace("|", "_");

	if(!fileName.isEmpty() && !saveDirName.endsWith("/"))
	{
		fileName = "/" + fileName;
	}
	QString filename = QFileDialog::getSaveFileName(parent, tr("Сохранить изображение как..."),
		saveDirName + fileName , nameFilters.join(";;"), &suffix);
	if(filename.isEmpty())
	{
		return false;
	}
	fileInfo = QFileInfo(filename);
	settings->setImageSaveDir(fileInfo.absolutePath());
	settings->setImageSaveSuffix(suffix);
	ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
	if(!save(filename, imageAddedSettings.jpegQuality))
	{
		return false;
	}
	if(imageAddedSettings.objectTableFileCreateAuto)
	{
		QString fileCsvPathError = ICEGUI_NS::ObjectTableWidget::createCsvFiles(
			fileInfo.absoluteFilePath(), fileInfo.absolutePath(), getImageBuilder());
		if(!fileCsvPathError.isEmpty())
		{
			QMessageBox::warning(parent, tr("Сохранить изображение ..."),
				tr("Ошибка записи файла объктов ") + fileCsvPathError);
			return false;
		}
	}
	if(imageAddedSettings.htmlFileCreateAuto)
	{
		//TODO REFAC
		ICEQWT_NS::IceUnitRatioQwtTransform* transform = (ICEQWT_NS::IceUnitRatioQwtTransform*)
			getImageEditor()->icePlot()->axisScaleEngine(QwtPlot::xTop)->transformation();
		Q_ASSERT(transform);
		ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
		QString fileHtmlPathError = ICEGUI_NS::ObjectTableWidget::createHtmlFile(
			QStringList() << fileInfo.absoluteFilePath(), fileInfo.absolutePath(),
			getImageBuilder(), transform->unit(), transform->ratio(),
			imageAddedSettings.halfArcLength);
		if(!fileHtmlPathError.isEmpty())
		{
			QMessageBox::warning(parent, tr("Сохранить изображение ..."),
				tr("Ошибка записи файла отчёта ") + fileHtmlPathError);
			return false;
		}
	}
	return true;
}

bool MdiSubWindow::save(const QString& filename, int quality)
{
	if(getImageEditor()->save(filename, quality))
	{
		setWindowTitle(strippedName(filename));
		getImageEditor()->setWindowModified(false);
		return true;
	}
	else 
	{
		return false;
	}
}

const QString& MdiSubWindow::getFilename() const
{
	return getImageEditor()->getFilename();
}

bool MdiSubWindow::isWindowModified() const
{
	return getImageEditor()->isWindowModified();
}

ICEQWT_NS::ImageEditor* MdiSubWindow::getImageEditor() const
{
	return getCommonWidget()->getImageEditor();
}

QSharedPointer<ICEIMAGE_NS::ImageBuilder> MdiSubWindow::getImageBuilder() const
{
	return getImageEditor()->getImageBuilder();
}

QSize MdiSubWindow::sizeHint() const
{
	return QSize(144 * fontMetrics().width('x'), 50 * fontMetrics().lineSpacing());
}

void MdiSubWindow::updateState()
{
	if(m_objectViewDialog)
	{
		//TODO ???
		ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
		Q_ASSERT(settings);
		ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
		//TODO refac слишком много вызовов ->
		m_objectViewDialog->updateState((ICEQWT_NS::IceUnitRatioQwtTransform*)
			getImageEditor()->icePlot()->axisScaleEngine(QwtPlot::xTop)->transformation(),
			imageAddedSettings.halfArcLength);
	}
}

void MdiSubWindow::onCustomContextMenuRequested(const QPoint& pos)
{
	m_contextMenu->popup(mapToGlobal(pos));
}

void MdiSubWindow::showMask_triggered(bool checked)
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if(action)
	{
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
		if(!imageBuilder.isNull())
		{
			imageBuilder->showMask(action->data().toInt(), checked);
		}
	}
}

void MdiSubWindow::showContours_triggered(bool checked)
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if(action)
	{
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
		if(!imageBuilder.isNull())
		{
			imageBuilder->showContours(action->data().toInt(), checked);
		}
	}
}

void MdiSubWindow::showContoursObjectsAction_triggered(bool checked)
{
	Q_ASSERT(m_objectViewDialog);
	if(checked)
	{
		m_objectViewDialog->show();
	}
	else
	{
		m_objectViewDialog->hide();
	}
}

void MdiSubWindow::objectViewDialog_finished(int /*result*/)
{
	if(m_showContoursObjectsAction)
	{
		m_showContoursObjectsAction->setChecked(false);
	}
}

void MdiSubWindow::showLines_triggered(bool checked)
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if(action)
	{
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
		if(!imageBuilder.isNull())
		{
			imageBuilder->showLines(action->data().toInt(), checked);
		}
	}
}

void MdiSubWindow::imageCutDownThread_finished()
{
	bool mergeIsOk = false;
	if(m_imageCutDownThread)
	{
		mergeIsOk = m_imageCutDownThread->isNoError();
		m_imageCutDownThread->stop();
		m_imageCutDownThread->wait();
		delete m_imageCutDownThread;
		m_imageCutDownThread = 0;
	}
	else
	{
		mergeIsOk = m_mergeIsOk;
	}
	if(mergeIsOk)
	{
		QMessageBox::information(this, tr("Сшивка изображения"),
			tr("Сшивка изображения успешно завершена"));
		fillContextMenu(); //TODO ???
	}
	else
	{
		QMessageBox::warning(this, tr("Сшивка изображения"),
			tr("В процессе сшивки изображения возникли ошибки"));
	}
}

void MdiSubWindow::closeEvent(QCloseEvent* event)
{
	if(okToContinue())
	{
		event->accept();
		// Без этого не работает закрытие главного окна приложения
		mdiArea()->removeSubWindow(this);
		emit closed();
	}
	else
	{
		event->ignore();
	}
}

CommonWidget* MdiSubWindow::getCommonWidget() const
{
	return qobject_cast<CommonWidget*>(widget());
}

bool MdiSubWindow::okToContinue()
{
	if(isWindowModified())
	{
		int r = QMessageBox::warning(this, tr("Сохранение"),
			tr("Изображение было изменено.\n\n"
			"Вы хотите сохранить Ваши изменения?"),
			tr("Да"), tr("Нет"), tr("Отмена"));
		if(r == 0)
		{
			return save();
		}
		else if(r == 2)
		{
			return false;
		}
	}
	return true;
}

} // namespace ICEMDI_NS
