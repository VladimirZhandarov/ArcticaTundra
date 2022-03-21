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
// Имя файла: mdisubwindow.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include "icemdidef.h"

#include <QtGui/QMdiSubWindow>

#include <boost/tr1/memory.hpp>

class QCloseEvent;

namespace ICEODB_NS
{
	class MaskView;
}

namespace ICEQWT_NS
{
	class ImageEditor;
}

namespace ICEIMAGE_NS
{
	class ImageBuilder;
	class ImageCutDownThread;
}

namespace ICEGUI_NS
{
	class ObjectViewDialog;
}

namespace ICEMDI_NS
{
	class CommonWidget;

class ICEMDI_EXPORT MdiSubWindow: public QMdiSubWindow
{
	Q_OBJECT
public:
	MdiSubWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~MdiSubWindow();

	static QString strippedName(const QString& filename);

	//TODO refac
	void fillContextMenu();

	bool open(bool cutDown, QWidget* parent = 0);
	// Для открытия при известном имени файла (например из списка
	// недавно открытых файлов)
	bool open(const QString& filename);
	bool open(const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView =
		std::tr1::shared_ptr<ICEODB_NS::MaskView>(), const QString& title = tr("Созданное из буфера обмена"));

	bool save();
	bool saveAs(QWidget* parent = 0);
	bool save(const QString& filename, int quality);

	const QString& getFilename() const;

	bool isWindowModified() const;

	ICEQWT_NS::ImageEditor* getImageEditor() const;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> getImageBuilder() const;

	virtual QSize sizeHint() const;
	void updateState();

signals:
	void errorProcessed(QString errorMsg);
	void closed();

protected slots:
	void onCustomContextMenuRequested(const QPoint& pos);
	void showMask_triggered(bool checked);
	void showContours_triggered(bool checked);
	void showContoursObjectsAction_triggered(bool checked);
	void objectViewDialog_finished(int result);
	void showLines_triggered(bool checked);
	void imageCutDownThread_finished();

protected:
	virtual void closeEvent(QCloseEvent* event);

private:
	CommonWidget* getCommonWidget() const;
	bool okToContinue();

	QMenu* m_contextMenu;
	QAction* m_showContoursObjectsAction;
	ICEGUI_NS::ObjectViewDialog* m_objectViewDialog;
	ICEIMAGE_NS::ImageCutDownThread* m_imageCutDownThread;
	bool m_mergeIsOk;
};

} // namespace ICEMDI_NS

#endif // MDISUBWINDOW_H
