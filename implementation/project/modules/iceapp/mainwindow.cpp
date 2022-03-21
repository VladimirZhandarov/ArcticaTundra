//==============================================================================
// Название:        Главное окно приложения
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: Vladimir Zhandarov $
// Создан:    2011.11.08
// Изменен:   $Date:$
// Модуль:    iceapp
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: mainwindow.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "mainwindow.h"

#include <QtGui/QApplication>
#include <QtGui/QMdiArea>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QMessageBox>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QCloseEvent>
#include <QtGui/QClipboard>
#include <QtGui/QCompleter>
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>

#include <QtCore/QTimer>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QRect>

#include "aboutdialog.h"

#include <icecore/core.h>
#include <icecore/filteritemmodel.h>
#include <icecore/settings.h>

#include <icegui/dbsettingdialog.h>
#include <icegui/brightnesscontrastdialog.h>
#include <icegui/turndialog.h>
#include <icegui/colorbalancedialog.h>
#include <icegui/imageresizedialog.h>
#include <icegui/imagecutupdialog.h>
#include <icegui/filtermanagedialog.h>
#include <icegui/filtereditdialog.h>
#include <icegui/scaledialog.h>
#include <icegui/paramdlg.h>
#include <icegui/cursorimageinfowidget.h>
#include <icegui/batchprocessingdlg.h>

#include <iceimage/gdalimagebuilder.h>

#include <icemdi/mdisubwindow.h>

#include <iceproc/imageprocinterface.h>

#include <iceqwt/imageeditor.h>
#include <iceqwt/iceqwtplot.h>
#include <iceqwt/iceqwtplotpicker.h>
#include <iceqwt/iceqwtplotgrid.h>

//TODO refac
#include <qwt_scale_engine.h>
#include <iceqwt/iceunitratioqwttransform.h>

#include <iceodb/odb/filterparam.hxx>

#include "icetoolstlbr.h"

QString MainWindow::dllNameKeyName = "dllName";
QString MainWindow::filterIdKeyName = "filterId";

MainWindow::MainWindow(QWidget* parent, Qt::WFlags flags) :
	ICEPROC_NS::ImageWindowInterface(parent, flags)
{
	ui.setupUi(this);

	ICEIMAGE_NS::ImageBuilder builder;
	connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
		this, SLOT(onSubWindowActivated(QMdiSubWindow*)));

	createActions();
	createMenus();
	createToolBars();
	fillFilterMenu();

	QDockWidget* cursorImageDockWidget = new QDockWidget(tr("Сведения об изображении"), this);
	cursorImageDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_cursorImageInfoWidget = new ICEGUI_NS::CursorImageInfoWidget(this);
	cursorImageDockWidget->setWidget(m_cursorImageInfoWidget);
	addDockWidget(Qt::RightDockWidgetArea, cursorImageDockWidget);
	ui.windowMenu->addAction(cursorImageDockWidget->toggleViewAction());

	setWindowIcon(QIcon(appIconStr));
	setWindowTitle(tr("ПО ТУНДРА"));

	updateActions();

	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	restoreGeometry(settings->getMainWindowGeometry());
	restoreState(settings->getMainWindowState());

	QClipboard* clipboard = QApplication::clipboard();
	if(clipboard)
	{
		connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboard_dataChanged()));
	}
	clipboard_dataChanged();

	QTimer::singleShot(0, this, SLOT(loadFiles()));
}

MainWindow::~MainWindow()
{

}

QString MainWindow::getCurrentFilename() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	if(!imageBuilder.isNull())
	{
		return imageBuilder->getFilename();
	}
	return "";
}

QImage MainWindow::getCurrentSelectedImage(const QRect& rect) const
{
	//TODO refac
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	if(!imageBuilder.isNull())
	{
		ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
		if(mdiSubWindow)
		{
			ICEQWT_NS::ImageEditor* imageEditor = mdiSubWindow->getImageEditor();
			Q_ASSERT(imageEditor);
			//TODO
			if(rect.isNull())
			{
				imageBuilder->setUserCreatedObject(imageEditor->getSelectedArea());
			}
			else
			{
				imageBuilder->setUserCreatedObject(rect);
			}
		}
		else
		{
			imageBuilder->setUserCreatedObject();
		}
		return imageBuilder->getUserCreatedObjectQImage();
	}
	else
	{
		return QImage();
	}
}

QRect MainWindow::getCurrentSelectedRect() const
{
	//TODO refac
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	if(!imageBuilder.isNull())
	{
		return imageBuilder->getUserCreatedObjectRect();
	}
	else
	{
		return QRect();
	}
}

QImage MainWindow::getCurrentImage() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	if(!imageBuilder.isNull())
	{
		return imageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::RESULT_IMAGE_AND_MASK, QRect(), true);
	}
	else
	{
		return QImage();
	}
}

QSharedPointer<ICEIMAGE_NS::ImageBuilder> MainWindow::getCurrentImageBuilder() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> result;
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(mdiSubWindow)
	{
		result = mdiSubWindow->getImageBuilder();
	}
	return result;
}

ICEMDI_NS::MdiSubWindow* MainWindow::createMdiWindow(bool cutDown, const QString& filename,
	const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView,
	const QString& title)
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = new ICEMDI_NS::MdiSubWindow(ui.mdiArea);
	connect(mdiSubWindow, SIGNAL(errorProcessed(QString)), this, SLOT(onErrorProcessed(QString)));
	connect(mdiSubWindow, SIGNAL(closed()), this, SLOT(onMdiSubWindowClosed()));
	mdiSubWindow->setWindowIcon(QIcon(":/dvg/iceapp/images/applicationico.png"));
	bool opened = false;
	QString actionName = "";
	if(!image.isNull())
	{
		opened = mdiSubWindow->open(image, backgroundImageMaskView, title);
		actionName = title;
	}
	//TODO refac
	else if(filename.isEmpty())
	{
		opened = mdiSubWindow->open(cutDown, this);
	}
	else
	{
		opened = mdiSubWindow->open(filename);
	}
	if(opened)
	{
		if(actionName.isEmpty())
		{
			QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = mdiSubWindow->getImageBuilder();
			if(!imageBuilder.isNull())
			{
				actionName = ICEMDI_NS::MdiSubWindow::strippedName(imageBuilder->getFilename());
			}
		}
		QAction* windowMenuAction = new QAction(this);
		windowMenuAction->setCheckable(true);
		windowMenuAction->setText(actionName);
		connect(windowMenuAction, SIGNAL(triggered()), mdiSubWindow, SLOT(show()));
		connect(windowMenuAction, SIGNAL(triggered()), mdiSubWindow, SLOT(setFocus()));
		m_mdiSubWindowActionMap.insert(mdiSubWindow, windowMenuAction);

		ui.windowMenu->addAction(windowMenuAction);
		m_openWindowsAction->addAction(windowMenuAction);
		if(image.isNull())
		{
			addRecentFile(mdiSubWindow->getFilename());
		}
		ICEQWT_NS::ImageEditor* imageEditor = mdiSubWindow->getImageEditor();
		Q_ASSERT(imageEditor);
		connect(imageEditor, SIGNAL(windowChangeModified()), this, SLOT(updateActions()));
		Q_ASSERT(m_cursorImageInfoWidget);
		//TODO refac
		connect(imageEditor, SIGNAL(cursorPositionChanged()),
			m_cursorImageInfoWidget, SLOT(onCursorPositionChanged()));
		connect(imageEditor, SIGNAL(selectedAreaChanged()),
			m_cursorImageInfoWidget, SLOT(onSelectedAreaChanged()));
		connect(imageEditor, SIGNAL(selectedAreaChanged()),
			this, SLOT(onSelectedAreaChanged()));


		// Масштабирование
		connect(imageEditor, SIGNAL(zoomIn()), this, SLOT(zoomInAction_triggered()));
		connect(imageEditor, SIGNAL(zoomOut()), this, SLOT(zoomOutAction_triggered()));

		mdiSubWindow->show();

		toolsTlBr->setInitial();

		// Устанавливаем масштаб по умолчанию
		Q_ASSERT(m_scaleCB);
		ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
		Q_ASSERT(settings);
		m_scaleCB->setCurrentPercentType(settings->getImageScale());
		return mdiSubWindow;
	}
	else 
	{
		delete mdiSubWindow;
		return 0;
	}
}

void MainWindow::setFilerDialogActionVisible(ICEGUI_NS::FilterEditDialog* dialog,
	bool visible)
{
	if(dialog)
	{
		QString dllName = dialog->getDllName();
		unsigned int key = dialog->getFilterParamId();
		QMenu* menu = m_filterDialogMenuMap.value(dllName);
		if(menu)
		{
			QList<QAction*> actionList = menu->actions();
			QList<QAction*>::const_iterator iterEnd = actionList.constEnd();
			for(QList<QAction*>::const_iterator iter = actionList.constBegin();
				iter != iterEnd; iter++)
			{
				QAction* action = (*iter);
				if(action)
				{
					QVariantMap map = action->data().toMap();
					if(map[filterIdKeyName].toUInt() == key)
					{
							if(!visible)
							{
								menu->removeAction(action);
								delete action;
							}
						return;
					}
				}
			}
			QAction* action = new QAction(dialog->getName(), this);
			QVariantMap dataMap;
			dataMap.insert(dllNameKeyName, dllName);
			dataMap.insert(filterIdKeyName, key);
			action->setData(dataMap);
			connect(action, SIGNAL(triggered()), this, SLOT(onOpenFilterDialog()));
			menu->addAction(action);
		}
	}
}

void MainWindow::createActions()
{
	for(int i=0; i<MaxRecentFiles; i++)
	{
		m_recentFileActions[i] = new QAction(this);
		m_recentFileActions[i]->setVisible(false);
		connect(m_recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}
	m_exitAction = new QAction(tr("Выход"), this);
	m_exitAction->setIcon(QIcon(":/dvg/iceapp/images/exit.png"));
	m_exitAction->setShortcut(tr("Ctrl+Q"));
	m_exitAction->setStatusTip(tr("Выйти из программы"));
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

	// --- меню "Окно" ---
	m_separatorWindowAction = ui.windowMenu->addSeparator();

	m_openWindowsAction = new QActionGroup(this);
	m_openWindowsAction->setExclusive(true);
}

void MainWindow::createMenus()
{
	// --- меню "Файл" ---
	m_recentFileSeparator = ui.fileMenu->addSeparator();
	for(int i=0; i<MaxRecentFiles; i++)
	{
		ui.fileMenu->addAction(m_recentFileActions[i]);
	}
	ui.fileMenu->addSeparator();
	ui.fileMenu->addAction(m_exitAction);
}

void MainWindow::createToolBars()
{
	// --- панель инструментов "Файл" ---
	ui.fileToolBar->addAction(ui.openAction);
	ui.fileToolBar->addAction(ui.saveAction);

	// --- панель инструментов "Вид" ---
	ui.viewToolBar->addAction(ui.rulerAction);
	ui.viewToolBar->addAction(ui.gridAction);
	ui.viewToolBar->addSeparator();
	ui.viewToolBar->addAction(ui.zoomInAction);
	ui.viewToolBar->addAction(ui.zoomOutAction);

	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	m_scaleCB = new ICEGUI_NS::ScaleComboBox();
	m_scaleCB->setCurrentPercentType(settings->getImageScale());
	connect(m_scaleCB, SIGNAL(scaleChanged(int)), this, SLOT(onScaleChanged(int)));
	ui.viewToolBar->addWidget(m_scaleCB);
	ui.viewToolBar->addAction(ui.inSizeOfWindowAction);

	// --- панель инструментов "Изображение" ---
	ui.imageToolBar->addAction(ui.brightnessContrastAction);
	ui.imageToolBar->addAction(ui.balanceColorAction);
	ui.imageToolBar->addAction(ui.grayAction);
	ui.imageToolBar->addAction(ui.turnAction);
	ui.imageToolBar->addAction(ui.resolutionAction);

	toolsTlBr = new IceToolsTlBr("Инструменты", this);
	toolsTlBr->setMovable(false);
	toolsTlBr->setAllowedAreas(Qt::LeftToolBarArea);
	toolsTlBr->setFloatable(false);
	addToolBar(Qt::LeftToolBarArea, toolsTlBr);
}

void MainWindow::onFilterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam)
{
	Q_ASSERT(filterParam.get());

	ICEPROC_NS::ImageProcInterface* proc = ICECORE_NS::Core::Instance()->getImageProc(
		filterParam->getFilterType()->getDllName());
	ICEMDI_NS::MdiSubWindow* sourceMdiSubWindow = activeMdiSubWindow();
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> sourceImageBuilder = getCurrentImageBuilder();
	if(proc && sourceMdiSubWindow && !sourceImageBuilder.isNull())
	{
		Qt::WindowStates sourceMdiSubWindowState = sourceMdiSubWindow->windowState();
		QRect sourceMdiSubWindowRect = sourceMdiSubWindow->geometry();
		Q_ASSERT(m_scaleCB);
		int currentPercentType = m_scaleCB->getCurrentPercentType();
		QString title = ICEMDI_NS::MdiSubWindow::strippedName(sourceImageBuilder->getFilename());
		QFileInfo fileInfo(title);
		QString suffix = "";
		if(!fileInfo.completeBaseName().isEmpty())
		{
			title = fileInfo.completeBaseName();
			suffix = fileInfo.suffix();
			if(!suffix.isEmpty())
			{
				suffix = "." + suffix;
			}
		}
		if(title.isEmpty())
		{
			//TODO: refac "Созданное из буфера обмена"
			title = tr("Созданное из буфера обмена");
		}
		int usedFilterNumber = sourceImageBuilder->getUsedFilterCount();
		if(usedFilterNumber > 0)
		{
			title += " (" + QString::number(usedFilterNumber) + ")";
		}
		title += " " + proc->getShortName() + ": " + filterParam->getName();
		QImage image = sourceImageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::RESULT_IMAGE_NO_MASK,
			QRect(), true);
		ICEMDI_NS::MdiSubWindow* mdiSubWindow = createMdiWindow(false, "", image,
			filterParam->getBackgroundView(), title + suffix);

		if(mdiSubWindow)
		{
			//TODO refac
			ICEQWT_NS::IceQwtPlot* sourcePlot =(ICEQWT_NS::IceQwtPlot*)
				sourceMdiSubWindow->getImageEditor()->icePlot();
			Q_ASSERT(sourcePlot);
			ICEQWT_NS::IceQwtPlot* resultPlot = mdiSubWindow->getImageEditor()->icePlot();
			Q_ASSERT(resultPlot);

			// Изображение в окне фильтра должно быть отображено с таким же масштабом и смещением
			// как и оригинальное изображение
			m_scaleCB->setCurrentPercentType(currentPercentType);
			QSharedPointer<ICEIMAGE_NS::ImageBuilder> filterImageBuilder = mdiSubWindow->getImageBuilder();
			Q_ASSERT(!filterImageBuilder.isNull());
			filterImageBuilder->setScaleRegionRect(sourceImageBuilder->getScaleRegionRect());

			resultPlot->setupLine(sourcePlot->getLineSettings());
			resultPlot->setupGrid(sourcePlot->getGridSettings());
			resultPlot->setupAxisInterval(sourcePlot->axisInterval(QwtPlot::xTop),
				sourcePlot->axisInterval(QwtPlot::yLeft));
			resultPlot->replot();

			mdiSubWindow->setWindowState(sourceMdiSubWindowState);
			mdiSubWindow->resize(sourceMdiSubWindowRect.width(), sourceMdiSubWindowRect.height());
			filterImageBuilder->copyData(sourceImageBuilder);
			//TODO refac слишком много вызовов ->
			ICEQWT_NS::IceUnitRatioQwtTransform* transformation =(ICEQWT_NS::IceUnitRatioQwtTransform*)
				mdiSubWindow->getImageEditor()->icePlot()->axisScaleEngine(QwtPlot::xTop)->transformation();
			Q_ASSERT(transformation);
			proc->calcMaskVector(filterImageBuilder, filterParam, QVector<int>(),
				transformation->ratio(), transformation->unit());
			mdiSubWindow->fillContextMenu();

			filterImageBuilder->apply(true);
		}
	}
}

void MainWindow::onErrorProcessed(QString errorMsg)
{
	QMessageBox::warning(0, tr("Ошибка работы графическим ускорителем"), errorMsg);
}

void MainWindow::onMdiSubWindowClosed()
{
	QAction* windowAction = m_mdiSubWindowActionMap.take(qobject_cast<ICEMDI_NS::MdiSubWindow*>(sender()));
	if(windowAction)
	{
		ui.windowMenu->removeAction(windowAction);
		m_openWindowsAction->removeAction(windowAction);
		delete windowAction;
	}
}

void MainWindow::onScaleChanged(int scalePercentType)
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(!mdiSubWindow)
	{
		return;
	}
	ICEQWT_NS::ImageEditor* imageEditor = mdiSubWindow->getImageEditor();
	Q_ASSERT(imageEditor);
	Q_ASSERT(m_scaleCB);
	switch(scalePercentType)
	{
		case ICEGUI_NS::ScaleComboBox::TO_SIZE_SCALE: // "Уместить всю"
		{
			inSizeOfWindowAction_triggered();
			break;
		}
		case ICEGUI_NS::ScaleComboBox::TO_HEIGHT_SCALE: // "По высоте"
		{
			m_scaleCB->setCurrentPercentType(imageEditor->getPercentScaleInHeight());
			break;
		}
		case ICEGUI_NS::ScaleComboBox::TO_WIDTH_SCALE: // "По ширине"
		{
			m_scaleCB->setCurrentPercentType(imageEditor->getPercentScaleInWidth());
			break;
		}
		default:
		{
			imageEditor->setPercentScale(scalePercentType);
			break;
		}
	}
}

void MainWindow::onSelectedAreaChanged()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	if(imageBuilder.isNull())
	{
		ui.copySelectedToClipboardAction->setEnabled(false);
	}
	else
	{
		QRect rect = imageBuilder->getUserCreatedObjectRect();
		ui.copySelectedToClipboardAction->setEnabled(!rect.isNull());
	}
}

void MainWindow::onSubWindowActivated(QMdiSubWindow* subWindow)
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = 0;
	if(subWindow)
	{
		mdiSubWindow = qobject_cast<ICEMDI_NS::MdiSubWindow*>(subWindow);
	}
	Q_ASSERT(m_cursorImageInfoWidget);
	if(mdiSubWindow)
	{
		m_cursorImageInfoWidget->setImageEditor(mdiSubWindow->windowTitle(), mdiSubWindow->getImageEditor());
	}
	else
	{
		m_cursorImageInfoWidget->setImageEditor("", 0);
	}
	updateActions();
}

void MainWindow::openAction_triggered()
{
	ICEMDI_NS::MdiSubWindow* mdi = createMdiWindow();
	if(mdi)
	{
		onSubWindowActivated(mdi);
	}
}

void MainWindow::openFilterFromFileAction_triggered()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	QString filename = QFileDialog::getOpenFileName(this, tr("Открыть фильтр"),
		settings->getFilterParamLoadDir(), tr("Файл фильтра (*.ifl)"));
	if(filename.isEmpty())
	{
		return;
	}
	QFileInfo fileInfo(filename);
	settings->setFilterParamLoadDir(fileInfo.absolutePath());
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam =
		ICECORE_NS::Core::Instance()->loadFilterParam(filename);
	if(!filterParam.get())
	{
		return;
	}
	ICECORE_NS::Core* core = ICECORE_NS::Core::Instance();
	Q_ASSERT(core);
	ICEPROC_NS::ImageProcInterface* proc = core->getImageProc(filterParam->getFilterType()->getDllName());
	if(proc)
	{
		ICEGUI_NS::FilterEditDialog* editDialog = m_openFromFileFilterEditDialogMap.value(filename);
		if(!editDialog)
		{
			editDialog = ICEGUI_NS::FilterEditDialog::createInstance(proc, this,
				core->getFilterItemModel(), false, 0, Qt::Dialog | Qt::WindowMinimizeButtonHint);
			if(editDialog)
			{
				connect(editDialog, SIGNAL(filterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>&)),
					this, SLOT(onFilterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>&)));
				connect(editDialog, SIGNAL(started(ICEGUI_NS::FilterEditDialog*)),
					this, SLOT(onEditFilterStarted(ICEGUI_NS::FilterEditDialog*)));
				connect(editDialog, SIGNAL(finished(ICEGUI_NS::FilterEditDialog*)),
					this, SLOT(onEditFilterFinished(ICEGUI_NS::FilterEditDialog*)));
				connect(this, SIGNAL(currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)),
					editDialog, SLOT(onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)));
				connect(this, SIGNAL(databaseConnectionChanged(bool)),
					editDialog, SLOT(onDatabaseConnectionChanged(bool)));

				editDialog->setWindowIcon(proc->getViewIcon());
				editDialog->setModel(core->getFilterItemModel());
				m_openFromFileFilterEditDialogMap.insert(filename, editDialog);
			}
		}
		if(editDialog)
		{
			editDialog->show(filterParam);
		}
	}
}

void MainWindow::createFromClipboardAction_triggered()
{
	QClipboard* clipboard = QApplication::clipboard();
	if(!clipboard)
	{
		return;
	}
	createMdiWindow(false, "", clipboard->image());
}

void MainWindow::saveAction_triggered()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(mdiSubWindow)
	{
		mdiSubWindow->save();
		QAction* windowAction = m_mdiSubWindowActionMap.value(mdiSubWindow);
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = mdiSubWindow->getImageBuilder();
		if(!imageBuilder.isNull() && windowAction)
		{
			windowAction->setText(ICEMDI_NS::MdiSubWindow::strippedName(imageBuilder->getFilename()));
		}
	}
}

void MainWindow::saveAsAction_triggered()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(mdiSubWindow->saveAs(this))
	{
		QAction* windowAction = m_mdiSubWindowActionMap.value(mdiSubWindow);
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = mdiSubWindow->getImageBuilder();
		if(!imageBuilder.isNull() && windowAction)
		{
			windowAction->setText(ICEMDI_NS::MdiSubWindow::strippedName(imageBuilder->getFilename()));
		}
		addRecentFile(mdiSubWindow->getFilename());
	}
}

void MainWindow::dbSettingAction_triggered()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::DbSettings dbSettings = settings->getDbSettings();
	ICEGUI_NS::DbSettingDialog dialog(dbSettings.user, dbSettings.password, dbSettings.dbName,
		dbSettings.host, dbSettings.port, this);
	dialog.setWindowIcon(QIcon(":/dvg/iceapp/images/connectdb.png"));
	if(dialog.exec() == QDialog::Accepted)
	{
		dbSettings.host = dialog.getHost();
		dbSettings.port = dialog.getPort();
		dbSettings.dbName = dialog.getDbName();
		dbSettings.user = dialog.getUser();
		dbSettings.password = dialog.getPassword();
		settings->setDbSettings(dbSettings);
		initDb(dbSettings.user, dbSettings.password, dbSettings.dbName, dbSettings.host,
			dbSettings.port);
	}
}

void MainWindow::copyAllToClipboardAction_triggered()
{
	QClipboard* clipboard = QApplication::clipboard();
	if(clipboard)
	{
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
		Q_ASSERT(!imageBuilder.isNull());
		if(!imageBuilder->isGdalUsed())
		{
			clipboard->setImage(imageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::RESULT_IMAGE_AND_MASK,
				QRect(), true));
		}
		else
		{

		}
	}
}

void MainWindow::copySelectedToClipboardAction_triggered()
{
	QClipboard* clipboard = QApplication::clipboard();
	if(clipboard)
	{
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
		Q_ASSERT(!imageBuilder.isNull());
		QRect rect = imageBuilder->getUserCreatedObjectRect();
		Q_ASSERT(!rect.isNull());
		quint64 x = rect.width();
		quint64 y = rect.height();
		quint64 maxSize = ICEIMAGE_NS::GdalImageBuilder::getMaxImageSize();
		quint64 size = x * y * ICEIMAGE_NS::ImageBuilder::maxChannelCount;
		if(size <= maxSize)
		{
			clipboard->setImage(imageBuilder->getUserCreatedObjectQImage());
		}
		else
		{
			QMessageBox::warning(this, tr("Копировать выделенное в буфер обмена"),
				tr("Выбранный участок занимает слишком много памяти.\n") +
				tr("Поэтому не может быть помещен в буфер обмена.\n") +
				tr("Максимальный размер: ") + ICEIMAGE_NS::BaseImageBuilder::createMemoryString(maxSize) +
				tr("\nРазмер выбранного участка: ") + ICEIMAGE_NS::BaseImageBuilder::createMemoryString(size));
		}
	}
}

void MainWindow::rulerAction_triggered()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(mdiSubWindow)
	{
		ICEQWT_NS::ImageEditor* imageEditor = mdiSubWindow->getImageEditor();
		Q_ASSERT(imageEditor);
		bool isAxis = imageEditor->axisVisible();
		imageEditor->setAxisVisible(!isAxis);
	}
}

void MainWindow::defaultScaleParamAction_triggered()
{
	ICEGUI_NS::ScaleDialog scaleDialog(this);
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	scaleDialog.setCurrentPercentType(settings->getImageScale());
	if(scaleDialog.exec() == QDialog::Accepted)
	{
		int scaleType = scaleDialog.getCurrentPercentType();
		settings->setImageScale(scaleType);
		Q_ASSERT(m_scaleCB);
		m_scaleCB->setCurrentPercentType(scaleType);
	}
}

void MainWindow::gridParamAction_triggered()
{
	ICEGUI_NS::ParamDlg paramDlg(activeMdiSubWindow()->getImageEditor(), this);
	paramDlg.setCurrentSection(ICEGUI_NS::ParamDlg::GRID);
	if(paramDlg.exec() == QDialog::Accepted)
	{
		//TODO refac
		updateStateAction();
		//TOD не понятно нужно ли это для m_cursorImageInfoWidget
		Q_ASSERT(m_cursorImageInfoWidget);
		m_cursorImageInfoWidget->updateState();
	}
}

void MainWindow::lineParamAction_triggered()
{
	ICEGUI_NS::ParamDlg paramDlg(activeMdiSubWindow()->getImageEditor(), this);
	paramDlg.setCurrentSection(ICEGUI_NS::ParamDlg::GUIDES);
	if(paramDlg.exec() == QDialog::Accepted)
	{
		//TODO refac
		updateStateAction();
		//TODO refac сделано быстро (можно как-то красивее)
		activeMdiSubWindow()->updateState();
		Q_ASSERT(m_cursorImageInfoWidget);
		m_cursorImageInfoWidget->updateState();
	}
}

void MainWindow::addedParamAction_triggered()
{
	ICEGUI_NS::ParamDlg paramDlg(activeMdiSubWindow()->getImageEditor(), this);
	paramDlg.setCurrentSection(ICEGUI_NS::ParamDlg::ADDED);
	paramDlg.exec();
}

void MainWindow::gridAction_triggered()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(mdiSubWindow)
	{
		ICEQWT_NS::ImageEditor* imageEditor = mdiSubWindow->getImageEditor();
		Q_ASSERT(imageEditor);
		bool isGrid = imageEditor->gridVisible();
		imageEditor->setGridVisible(!isGrid);
	}
}

void MainWindow::zoomInAction_triggered()
{
	Q_ASSERT(m_scaleCB);
	m_scaleCB->zoomIn();
}

void MainWindow::zoomOutAction_triggered()
{
	Q_ASSERT(m_scaleCB);
	m_scaleCB->zoomOut();
}

void MainWindow::inSizeOfWindowAction_triggered()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	if(mdiSubWindow)
	{
		// Устанавливаем масштаб
		Q_ASSERT(m_scaleCB);
		m_scaleCB->setScaleType(ICEGUI_NS::ScaleComboBox::TO_SIZE_SCALE);
		m_scaleCB->setCurrentPercentType(mdiSubWindow->getImageEditor()->getPercentScaleInSize());
	}
}

void MainWindow::brightnessContrastAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	ICEGUI_NS::BrightnessContrastDialog dialog(imageBuilder, this);
	dialog.exec();
}

void MainWindow::balanceColorAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	ICEGUI_NS::ColorBalanceDialog dialog(imageBuilder, this);
	dialog.exec();
}

void MainWindow::grayAction_triggered(bool checked)
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setGray(checked);

	ui.grayRAction->setChecked(false);
	ui.grayGAction->setChecked(false);
	ui.grayBAction->setChecked(false);
}

void MainWindow::grayRAction_triggered(bool checked)
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setGray(checked, ICEIMAGE_NS::BaseImageBuilder::R_CHANNEL);

	ui.grayAction->setChecked(false);
	ui.grayGAction->setChecked(false);
	ui.grayBAction->setChecked(false);
}

void MainWindow::grayGAction_triggered(bool checked)
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setGray(checked, ICEIMAGE_NS::BaseImageBuilder::G_CHANNEL);

	ui.grayAction->setChecked(false);
	ui.grayRAction->setChecked(false);
	ui.grayBAction->setChecked(false);
}

void MainWindow::grayBAction_triggered(bool checked)
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setGray(checked, ICEIMAGE_NS::BaseImageBuilder::B_CHANNEL);

	ui.grayAction->setChecked(false);
	ui.grayRAction->setChecked(false);
	ui.grayGAction->setChecked(false);
}

void MainWindow::turn90ToRightAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setRotatedAngleDegree(imageBuilder->getRotatedAngleDegree() + 90);
}

void MainWindow::turn90ToLeftAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setRotatedAngleDegree(imageBuilder->getRotatedAngleDegree() - 90);
}

void MainWindow::turn180Action_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	imageBuilder->setRotatedAngleDegree(imageBuilder->getRotatedAngleDegree() + 180);
}

void MainWindow::turnAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	ICEGUI_NS::TurnDialog dialog(imageBuilder, this);
	dialog.exec();
}

void MainWindow::resolutionAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	ICEGUI_NS::ImageResizeDialog dialog(imageBuilder, this);
	if(dialog.exec() == QDialog::Accepted)
	{
		ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
		Q_ASSERT(mdiSubWindow);
		ICEQWT_NS::IceQwtPlot* plot = mdiSubWindow->getImageEditor()->icePlot();
		Q_ASSERT(plot);
		plot->rescale();
		Q_ASSERT(m_cursorImageInfoWidget);
		m_cursorImageInfoWidget->updateState();
		Q_ASSERT(m_scaleCB);
		onScaleChanged(m_scaleCB->getScaleType());
	}
}

void MainWindow::cutUpAction_triggered()
{
	QSharedPointer<ICEIMAGE_NS::GdalImageBuilder> imageBuilder =
		qSharedPointerDynamicCast<ICEIMAGE_NS::GdalImageBuilder>(getCurrentImageBuilder());
	Q_ASSERT(!imageBuilder.isNull());
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	Q_ASSERT(mdiSubWindow);
	ICEQWT_NS::IceQwtPlot* plot = mdiSubWindow->getImageEditor()->icePlot();
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	ICEGUI_NS::ImageCutUpDialog dialog(imageBuilder, plot, settings, this);
	dialog.exec();
}

void MainWindow::cutDownAction_triggered()
{
	createMdiWindow(true);
}

void MainWindow::batchProcessingAction_triggered()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICEGUI_NS::BatchProcessingDlg dialog(settings, this);
	dialog.setWindowIcon(QIcon(":/dvg/iceapp/images/batchprocessing.png"));
	dialog.exec();
}

void MainWindow::showFilterManageAction_triggered()
{
	ICECORE_NS::Core* core = ICECORE_NS::Core::Instance();
	Q_ASSERT(core);
	ICEGUI_NS::FilterManageDialog dialog(QStringList(), this);
	dialog.setWindowTitle(tr("Просмотр фильтров"));
	dialog.setSelectEnabled(false);
	dialog.setModel(core->getFilterItemModel());
	dialog.exec();
}

void MainWindow::tileHorizontalAction_triggered()
{
	QList<QMdiSubWindow*> subwindowList = ui.mdiArea->subWindowList();
	if(subwindowList.count() < 2)
	{
		ui.mdiArea->tileSubWindows();
		return;
	}
	int wHeight = ui.mdiArea->height() / subwindowList.count();
	int y = 0;
	foreach(QMdiSubWindow* item, subwindowList)
	{
		item->resize(ui.mdiArea->width(), wHeight);
		item->move(0, y);
		y += wHeight;
	}
}

void MainWindow::tileVerticalAction_triggered()
{
	QList<QMdiSubWindow*> subwindowList = ui.mdiArea->subWindowList();
	if(subwindowList.count() < 2)
	{
		ui.mdiArea->tileSubWindows();
		return;
	}
	int wWidth = ui.mdiArea->width() / subwindowList.count();
	int x = 0;
	foreach(QMdiSubWindow* item, subwindowList)
	{
		item->resize(wWidth, ui.mdiArea->height());
		item->move(x, 0);
		x += wWidth;
	}
}

void MainWindow::openRecentFile()
{
	bool isOpen = false;
	QString recentFileName = (qobject_cast<QAction*>(sender()))->data().toString();

	// Если файл уже открыт в программе, то делаем его окно активным
	QList<QMdiSubWindow*> subWindows = ui.mdiArea->subWindowList();
	foreach(QMdiSubWindow* subWindow, subWindows)
	{
		if(qobject_cast<ICEMDI_NS::MdiSubWindow*>(subWindow)->getFilename() == recentFileName)
		{
			ui.mdiArea->setActiveSubWindow(subWindow);
			isOpen = true;
			break;
		}
	}
	// Если файл в программе не открыт, от открываем его
	if(!isOpen)
	{
		createMdiWindow(false, recentFileName);
	}
}

void MainWindow::updateRecentFileActions()
{
	for(int i=0; i<MaxRecentFiles; i++)
	{
		if(i < m_recentFiles.count())
		{
			QString text = tr("%1 %2").arg(i + 1).arg(ICEMDI_NS::MdiSubWindow::strippedName(m_recentFiles[i]));
			m_recentFileActions[i]->setText(text);
			m_recentFileActions[i]->setData(m_recentFiles[i]);
			m_recentFileActions[i]->setVisible(true);
		}
		else
		{
			m_recentFileActions[i]->setVisible(false);
		}
	}
	m_recentFileSeparator->setVisible(!m_recentFiles.isEmpty());
}

void MainWindow::addRecentFile(const QString& filename)
{
	m_recentFiles.removeAll(filename);
	m_recentFiles.prepend(filename);
	updateRecentFileActions();
}

void MainWindow::initDb(const QString& user, const QString& password, const QString& dbName,
	const QString& host, unsigned short int port)
{
	ICECORE_NS::Core* core = ICECORE_NS::Core::Instance();
	Q_ASSERT(core);
	bool dbActionEnableFlag = false;
	try
	{
		ICECORE_NS::FilterItemModel* filterItemModel = core->getFilterItemModel();
		if(filterItemModel)
		{
			filterItemModel->initDb(user, password, dbName, host, port);
			const QStringList& procKeyList = core->getImageProcKey();
			filterItemModel->setLoadedFilterType(procKeyList);
			QStringList::const_iterator iterEnd = procKeyList.constEnd();
			for(QStringList::const_iterator iter = procKeyList.constBegin(); iter != iterEnd; iter++)
			{
				ICEPROC_NS::ImageProcInterface* proc = core->getImageProc(*iter);
				if(proc)
				{
					filterItemModel->setIcon(*iter, proc->getViewIcon());
				}
			}
		}
		updateFilterMenuActions(true);
	}
	catch(const ICECORE_NS::DataBaseException& /*exc*/)
	{
		updateFilterMenuActions(false);
		QMessageBox::warning(this, tr("Ошибка"), tr("Ошибка соединения с базой данных.\r\n") + 
			tr("Сохранение/Октрытие фильтров ограничено"));
	}
}

void MainWindow::fillFilterMenu()
{
	Q_ASSERT(ui.filterMenu);
	ICECORE_NS::Core* core = ICECORE_NS::Core::Instance();
	Q_ASSERT(core);
	QStringList keyList = core->getImageProcKey();
	ui.filterMenu->setEnabled(!keyList.isEmpty());
	QStringList::const_iterator iterEnd = keyList.constEnd();
	for(QStringList::const_iterator iter = keyList.constBegin(); iter != iterEnd; iter++)
	{
		QString dllName = *iter;
		ICEPROC_NS::ImageProcInterface* proc = core->getImageProc(dllName);
		if(proc)
		{
			QMenu* subMenu = ui.filterMenu->addMenu(proc->getViewIcon(), proc->getViewName(true));
			ICEGUI_NS::FilterEditDialog* editDialog = ICEGUI_NS::FilterEditDialog::createInstance(
				proc, this, core->getFilterItemModel(), true, 0, Qt::Dialog | Qt::WindowMinimizeButtonHint);

			connect(editDialog, SIGNAL(filterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>&)),
				this, SLOT(onFilterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>&)));
			connect(editDialog, SIGNAL(started(ICEGUI_NS::FilterEditDialog*)),
				this, SLOT(onEditFilterStarted(ICEGUI_NS::FilterEditDialog*)));
			connect(editDialog, SIGNAL(finished(ICEGUI_NS::FilterEditDialog*)),
				this, SLOT(onEditFilterFinished(ICEGUI_NS::FilterEditDialog*)));
			connect(this, SIGNAL(currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)),
				editDialog, SLOT(onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)));
			connect(this, SIGNAL(databaseConnectionChanged(bool)),
				editDialog, SLOT(onDatabaseConnectionChanged(bool)));

			QAction* openFilterAction = new QAction(tr("Открыть..."), this);
			openFilterAction->setIcon(QIcon(":/dvg/iceapp/images/open.png"));
			openFilterAction->setStatusTip(tr("Открыть фильтр"));
			openFilterAction->setData(dllName);
			m_openFilterActionList.append(openFilterAction);
			connect(openFilterAction, SIGNAL(triggered()), this, SLOT(openFilter()));

			QAction* createFilterAction = new QAction(tr("Создать..."), this);
			createFilterAction->setIcon(QIcon(":/dvg/iceapp/images/createfilter.png"));
			createFilterAction->setStatusTip(tr("Создать фильтр"));
			createFilterAction->setData(dllName);
			m_createFilterActionList.append(createFilterAction);
			connect(createFilterAction, SIGNAL(triggered()), this, SLOT(createFilter()));

			subMenu->addAction(openFilterAction);
			subMenu->addAction(createFilterAction);

			QMenu* subWinMenu = new QMenu(proc->getViewName(true));
			subWinMenu->setIcon(proc->getViewIcon());
			m_filterDialogMenuMap.insert(dllName, subWinMenu);
			ui.windowMenu->insertMenu(ui.closeAction, subWinMenu);
		}
	}

	ui.windowMenu->insertSeparator(ui.closeAction);
	updateActions();
}

void MainWindow::updateFilterMenuActions(bool databaseConnected)
{
	ui.showFilterManageAction->setEnabled(databaseConnected);
	for(int i=0; i<m_openFilterActionList.count(); i++)
	{
		QAction* action = m_openFilterActionList[i];
		if(action)
		{
			action->setEnabled(databaseConnected);
		}
	}
	emit databaseConnectionChanged(databaseConnected);
	updateActions();
}

void MainWindow::loadFiles()
{
	QStringList args = QApplication::arguments();
	args.removeFirst();
	if(!args.isEmpty())
	{
		foreach(QString arg, args)
		{
			ui.mdiArea->cascadeSubWindows();
		}
	}
	ui.mdiArea->activateNextSubWindow();

	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::DbSettings dbSettings = settings->getDbSettings();
	initDb(dbSettings.user, dbSettings.password, dbSettings.dbName, dbSettings.host,
		dbSettings.port);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	settings->setMainWindowGeometry(saveGeometry());
	settings->setMainWindowState(saveState());

	QList<QString> keyList = m_openFromFileFilterEditDialogMap.keys();
	QList<QString>::const_iterator iterEnd = keyList.constEnd();
	for(QList<QString>::const_iterator iter = keyList.constBegin(); iter != iterEnd; iter++)
	{
		ICEGUI_NS::FilterEditDialog* dialog = m_openFromFileFilterEditDialogMap.take(*iter);
		delete dialog;
	}

	ui.mdiArea->closeAllSubWindows();
	if(!ui.mdiArea->subWindowList().isEmpty())
	{
		event->ignore();
	}
	else
	{
		ICEGUI_NS::FilterEditDialog::clearFilterEditDialogMap();
		event->accept();
	}
}

ICEMDI_NS::MdiSubWindow* MainWindow::activeMdiSubWindow() const
{
	return qobject_cast<ICEMDI_NS::MdiSubWindow*>(ui.mdiArea->activeSubWindow());
}

void MainWindow::updateActions()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	bool hasEditor = (mdiSubWindow != 0);
	bool gdalBuilder = false;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getCurrentImageBuilder();
	if(!imageBuilder.isNull())
	{
		gdalBuilder = imageBuilder->isGdalUsed();
	}

	// Меню и тулбар "Файл"
	ui.saveAction->setEnabled(hasEditor && mdiSubWindow->isWindowModified());
	ui.saveAsAction->setEnabled(hasEditor);

	// Меню и тулбар "Правка"
	ui.copyAllToClipboardAction->setEnabled(hasEditor && !gdalBuilder);
	ui.copySelectedToClipboardAction->setEnabled(hasEditor);

	// Меню и тулбар "Вид"
	ui.viewToolBar->setEnabled(hasEditor);
	if(!hasEditor)
	{
		Q_ASSERT(m_scaleCB);
		ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
		Q_ASSERT(settings);
		m_scaleCB->setCurrentPercentType(settings->getImageScale());
	}
	ui.zoomInAction->setEnabled(hasEditor);
	ui.zoomOutAction->setEnabled(hasEditor);
	ui.inSizeOfWindowAction->setEnabled(hasEditor);
	// Восстановление состояния масштабов
	if(hasEditor)
	{
		int percentScale = mdiSubWindow->getImageEditor()->getCurrentPercentScale();
		// Устанавливаем масштаб
		m_scaleCB->setCurrentPercentType(percentScale);
	}
	ui.gridAction->setEnabled(hasEditor);	// Линейка
	ui.rulerAction->setEnabled(hasEditor);	// Сетка
	ui.lineParamAction->setEnabled(hasEditor);	// Настройка направляющих
	ui.gridParamAction->setEnabled(hasEditor);	// Настройка линейки и сетки
	ui.addedParamAction->setEnabled(hasEditor);	// TODO fix

	// Восстановление состояния линейки и сетки
	if(hasEditor)
	{
		ui.gridAction->setChecked(mdiSubWindow->getImageEditor()->gridVisible());
		ui.rulerAction->setChecked(mdiSubWindow->getImageEditor()->axisVisible());
	}
	else
	{
		ui.gridAction->setChecked(false);
		ui.rulerAction->setChecked(false);
	}

	// Меню и тулбар "Изображение"
	ui.imageToolBar->setEnabled(hasEditor && !gdalBuilder);
	ui.brightnessContrastAction->setEnabled(hasEditor && !gdalBuilder);
	ui.balanceColorAction->setEnabled(hasEditor && !gdalBuilder);
	ui.grayMenu->setEnabled(hasEditor && !gdalBuilder);
	ui.turnImageMenu->setEnabled(hasEditor && !gdalBuilder);
	ui.resolutionAction->setEnabled(hasEditor && !gdalBuilder);
	ui.cutUpAction->setEnabled(hasEditor);

	// Восстановление состояния кнопки "Оттенки серого"
	if(!imageBuilder.isNull())
	{
		ui.grayAction->setChecked(imageBuilder->isGray());
	}
	else
	{
		ui.grayAction->setChecked(false);
	}
	emit currentImageBuilderChanged(imageBuilder);

	// Меню и тулбар "Окно"
	ui.closeAction->setEnabled(hasEditor);
	ui.closeAllAction->setEnabled(hasEditor);
	ui.tileHorizontalAction->setEnabled(hasEditor);
	ui.tileVerticalAction->setEnabled(hasEditor);
	ui.cascadeAction->setEnabled(hasEditor);
	ui.nextAction->setEnabled(hasEditor);
	ui.previousAction->setEnabled(hasEditor);
	m_separatorWindowAction->setVisible(hasEditor);
	if(hasEditor)
	{
		QAction* action = m_mdiSubWindowActionMap.value(mdiSubWindow);
		if(action)
		{
			action->setChecked(true);
		}
	}
	toolsTlBr->actualActions();
}

void MainWindow::updateStateAction()
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = activeMdiSubWindow();
	Q_ASSERT(mdiSubWindow);
	ICEQWT_NS::ImageEditor* imageEditor = mdiSubWindow->getImageEditor();
	Q_ASSERT(imageEditor);

	blockSignals(true);
	ui.gridAction->setChecked(imageEditor->icePlot()->grid()->isVisible());
	blockSignals(false);
}

void MainWindow::clipboard_dataChanged()
{
	QClipboard* clipboard = QApplication::clipboard();
	if(clipboard)
	{
		if(!clipboard->image().isNull())
		{
			ui.createFromClipboardAction->setEnabled(true);
			return;
		}
	}
	ui.createFromClipboardAction->setEnabled(false);
}

void MainWindow::aboutAction_triggered()
{
	AboutDialog dialog(this);
	dialog.setVersion(VERSION);
	dialog.exec();
}

void MainWindow::openFilter()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if(action)
	{
		ICECORE_NS::Core* core = ICECORE_NS::Core::Instance();
		Q_ASSERT(core);
		QString key = action->data().toString();
		ICEPROC_NS::ImageProcInterface* proc = core->getImageProc(key);
		if(proc)
		{
			QStringList keyList;
			keyList << key;
			ICEGUI_NS::FilterManageDialog dialog(keyList, this);
			dialog.setWindowIcon(proc->getViewIcon());
			dialog.setModel(core->getFilterItemModel());
			if(dialog.exec() == QDialog::Accepted)
			{
				ICEGUI_NS::FilterEditDialog* editDialog = ICEGUI_NS::FilterEditDialog::createInstance(
					dialog.getCurrentFilterParamModelIndex(), 0, Qt::Dialog | Qt::WindowMinimizeButtonHint);
				if(editDialog)
				{
					editDialog->show();
				}
			}
		}
	}
}

void MainWindow::createFilter()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if(action)
	{
		QString key = action->data().toString();
		ICEGUI_NS::FilterEditDialog* editDialog = ICEGUI_NS::FilterEditDialog::getInstance(
			key, 0);
		if(editDialog)
		{
			editDialog->show();
		}
	}
}

void MainWindow::onOpenFilterDialog()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if(action)
	{
		QVariantMap dataMap = action->data().toMap();
		QString dllName = dataMap.value(dllNameKeyName, "").toString();
		if(!dllName.isEmpty())
		{
			unsigned int id = dataMap.value(filterIdKeyName, 0).toUInt();
			ICEGUI_NS::FilterEditDialog* editDialog = ICEGUI_NS::FilterEditDialog::getInstance(dllName, id);
			if(editDialog)
			{
				editDialog->showDialog();
			}
		}
	}
}

void MainWindow::onEditFilterStarted(ICEGUI_NS::FilterEditDialog* dialog)
{
	setFilerDialogActionVisible(dialog);
}

void MainWindow::onEditFilterFinished(ICEGUI_NS::FilterEditDialog* dialog)
{
	setFilerDialogActionVisible(dialog, false);
}

void MainWindow::onEditFilterFromFileFinished(ICEGUI_NS::FilterEditDialog* dialog)
{
	QString key = "";
	QMap<QString, ICEGUI_NS::FilterEditDialog*>::const_iterator iterEnd = m_openFromFileFilterEditDialogMap.constEnd();
	for(QMap<QString, ICEGUI_NS::FilterEditDialog*>::const_iterator iter = m_openFromFileFilterEditDialogMap.constBegin();
		iter != iterEnd; iter++)
	{
		if(dialog == (*iter))
		{
			key = iter.key();
		}
	}
	if(!key.isEmpty())
	{
		ICEGUI_NS::FilterEditDialog* dialog = m_openFromFileFilterEditDialogMap.take(key);
		delete dialog;
	}
}
