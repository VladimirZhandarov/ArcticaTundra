//==============================================================================
// Название:        Главное окно приложения
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: Vladimir Zhandarov $
// Создан:    2014.09.26
// Изменен:   $Date:$
// Модуль:    iceapp
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: mainwindow.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iceproc/imagewindowinterface.h>

#include "ui_mainwindow.h"

#include "iceappconst.h"

#include <QtCore/QMap>

#include <boost/tr1/memory.hpp>
#include <iceimage/imagebuilder.h>

class QMdiArea;
class QActionGroup;
class QToolButton;
class QButtonGroup;

class IceToolsTlBr;

namespace ICEODB_NS
{
	class FilterParam;
}

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEGUI_NS
{
	class FilterEditDialog;
	class ScaleComboBox;
	class CursorImageInfoWidget;
}

namespace ICEMDI_NS
{
	class MdiSubWindow;
}

class MainWindow : public ICEPROC_NS::ImageWindowInterface
{
	Q_OBJECT
	static QString dllNameKeyName;
	static QString filterIdKeyName;
public:
	//TODO refac убрать
	friend IceToolsTlBr;

	MainWindow(QWidget* parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

	virtual QString getCurrentFilename() const;
	virtual QImage getCurrentSelectedImage(const QRect& rect = QRect()) const;
	virtual QRect getCurrentSelectedRect() const;
	virtual QImage getCurrentImage() const;
	virtual QSharedPointer<ICEIMAGE_NS::ImageBuilder> getCurrentImageBuilder() const;

	ICEMDI_NS::MdiSubWindow* activeMdiSubWindow() const;

signals:
	void currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);
	void databaseConnectionChanged(bool connected);

protected:
	void closeEvent(QCloseEvent* event);

protected slots:
	void openAction_triggered();
	void openFilterFromFileAction_triggered();
	void createFromClipboardAction_triggered();
	void saveAction_triggered();
	void saveAsAction_triggered();
	void dbSettingAction_triggered();

	void copyAllToClipboardAction_triggered();
	void copySelectedToClipboardAction_triggered();

	void rulerAction_triggered();
	void defaultScaleParamAction_triggered();
	void gridParamAction_triggered();
	void lineParamAction_triggered();
	void addedParamAction_triggered();
	void gridAction_triggered();
	void zoomInAction_triggered();
	void zoomOutAction_triggered();
	void inSizeOfWindowAction_triggered();

	void brightnessContrastAction_triggered();
	void balanceColorAction_triggered();
	void grayAction_triggered(bool checked);
	void grayRAction_triggered(bool checked);
	void grayGAction_triggered(bool checked);
	void grayBAction_triggered(bool checked);

	void turn90ToRightAction_triggered();
	void turn90ToLeftAction_triggered();
	void turn180Action_triggered();
	void turnAction_triggered();
	void resolutionAction_triggered();

	void cutUpAction_triggered();
	void cutDownAction_triggered();
	void batchProcessingAction_triggered();

	void showFilterManageAction_triggered();

	void tileHorizontalAction_triggered();
	void tileVerticalAction_triggered();

	void openRecentFile();
	void loadFiles();
	void updateActions();
	void updateStateAction();
	void clipboard_dataChanged();

	void aboutAction_triggered();

	void openFilter();
	void createFilter();
	void onOpenFilterDialog();
	void onEditFilterStarted(ICEGUI_NS::FilterEditDialog* dialog);
	void onEditFilterFinished(ICEGUI_NS::FilterEditDialog* dialog);
	void onEditFilterFromFileFinished(ICEGUI_NS::FilterEditDialog* dialog);

	void onFilterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);

	void onErrorProcessed(QString errorMsg);
	void onMdiSubWindowClosed();
	void onScaleChanged(int scalePercentType);
	void onSelectedAreaChanged();
	void onSubWindowActivated(QMdiSubWindow* subWindow);

private:
	ICEMDI_NS::MdiSubWindow* createMdiWindow(bool cutDown = false, const QString& filename = "",
		const QImage& image = QImage(), const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView =
		std::tr1::shared_ptr<ICEODB_NS::MaskView>(), const QString& title = tr("Созданное из буфера обмена"));
	void setFilerDialogActionVisible(ICEGUI_NS::FilterEditDialog* dialog, bool visible = true);
	void createActions();
	void createMenus();
	void createToolBars();
	bool loadFile(const QString& filename);
	bool saveFile(const QString& filename);
	void updateRecentFileActions();

	void addRecentFile(const QString& filename);

	/**
	* Установить соединение с базой данных
	* @param user имя пользователя
	* @param password пароль пользователя
	* @param dbName имя базы данных
	* @param host имя или IP-адрес хоста, где находится СУБД
	* @param port порт СУБД (5432)
	*/
	void initDb(const QString& user, const QString& password, const QString& dbName,
		const QString& host, unsigned short int port);
	
	/**
	* Заполнить меню "Фильтры"
	*/
	void fillFilterMenu();
	void updateFilterMenuActions(bool databaseConnected);

	Ui::MainWindow ui;	///< Ui класс

	QStringList m_recentFiles;
	enum {MaxRecentFiles = 5};
	QAction* m_recentFileSeparator;
	QAction* m_recentFileActions[MaxRecentFiles];
	QAction* m_exitAction;

	IceToolsTlBr* toolsTlBr;

	// Действия меню "Фильтры"
	QList<QAction*> m_openFilterActionList;
	QList<QAction*> m_createFilterActionList;
	QMap<QString, QMenu*> m_filterDialogMenuMap;

	// Действия меню "Окно"
	QAction* m_separatorWindowAction;
	QActionGroup* m_openWindowsAction;

	ICEGUI_NS::ScaleComboBox* m_scaleCB;
	ICEGUI_NS::CursorImageInfoWidget* m_cursorImageInfoWidget;
	QMap<ICEMDI_NS::MdiSubWindow*, QAction*> m_mdiSubWindowActionMap;
	QMap<QString, ICEGUI_NS::FilterEditDialog*> m_openFromFileFilterEditDialogMap;
};

#endif // MAINWINDOW_H
