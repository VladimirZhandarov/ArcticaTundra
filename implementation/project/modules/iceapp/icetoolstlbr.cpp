#include "icetoolstlbr.h"

#include "mainwindow.h"
#include <icegui/cursorimageinfowidget.h>

#include <icemdi/mdisubwindow.h>
#include <iceqwt/imageeditor.h>

#include <QAction>
#include <QToolButton>
#include <QMenu>

class IceToolsTlBr::PrivateData
{
public:
	QAction* moveAct;
	QAction* pickerRectAct;
	QAction* pickerPolygonAct;
	QAction* measureAct;
	QAction* clearSelectedRectAct;

	QToolButton* navigationTlBtn;
	QToolButton* pickerTlBtn;

	void createNavigationToolButton(IceToolsTlBr*);
	void createPickerToolButton(IceToolsTlBr*);

	QToolButton* createMenu(const QList<QAction*>&, IceToolsTlBr*);
};

QToolButton* IceToolsTlBr::PrivateData::createMenu(const QList<QAction*>& acts, IceToolsTlBr* tlBr)
{
	QMenu* menu = new QMenu(tlBr);
	menu->addActions(acts);

	QToolButton* tlBtn = new QToolButton(tlBr);
	tlBtn->setMenu(menu);
	connect(menu, SIGNAL(triggered(QAction*)), tlBtn, SLOT(setDefaultAction(QAction*)));

	tlBtn->setDefaultAction(acts[0]);
	tlBr->addWidget(tlBtn);

	return tlBtn;
}

void IceToolsTlBr::PrivateData::createNavigationToolButton(IceToolsTlBr* tlBr)
{
	moveAct = new QAction(tr("Перемещение"), tlBr);
	moveAct->setIcon(QIcon(":/dvg/iceapp/images/move.png"));
	moveAct->setStatusTip(tr("Перемещение"));
	moveAct->setCheckable(true);

	QList<QAction*> acts; 
	acts << moveAct;
	QToolButton* tlBtn = createMenu(acts, tlBr);
	connect(tlBtn, SIGNAL(triggered(QAction*)), tlBr, SLOT(navigationToolTriggered(QAction*)));
	pickerTlBtn = tlBtn;
}

void IceToolsTlBr::PrivateData::createPickerToolButton(IceToolsTlBr* tlBr)
{
	pickerRectAct = new QAction(tr("Прямоугольник"), tlBr);
	pickerRectAct->setIcon(QIcon(":/dvg/iceapp/images/selectrect.png"));
	pickerRectAct->setStatusTip(tr("Прямоугольник"));
	pickerRectAct->setCheckable(true);

	pickerPolygonAct = new QAction(tr("Полигон"), tlBr);
	pickerPolygonAct->setIcon(QIcon(":/dvg/iceapp/images/selectpolygon.png"));
	pickerPolygonAct->setStatusTip(tr("Полигон"));
	pickerPolygonAct->setCheckable(true);

	QList<QAction*> acts; 
	acts << pickerRectAct << pickerPolygonAct;
	QToolButton* tlBtn = createMenu(acts, tlBr);
	connect(tlBtn, SIGNAL(triggered(QAction*)), tlBr, SLOT(pickerToolTriggered(QAction*)));
	navigationTlBtn = tlBtn;
}

IceToolsTlBr::IceToolsTlBr(const QString& title, MainWindow* mainWindow) :
	QToolBar(title, mainWindow)
{
	d_data = new PrivateData;

	d_data->createNavigationToolButton(this);
	d_data->createPickerToolButton(this);

	d_data->measureAct = new QAction(tr("Измерение расстояний"), this);
	d_data->measureAct->setIcon(QIcon(":/dvg/iceapp/images/measure.png"));
	d_data->measureAct->setStatusTip(tr("Измерение расстояний"));
	d_data->measureAct->setCheckable(true);
	addAction(d_data->measureAct);
	connect(d_data->measureAct, SIGNAL(triggered()), this, SLOT(measureActTriggeredSlot()));

	d_data->clearSelectedRectAct = new QAction(tr("Удалить выделенные области"), this);
	d_data->clearSelectedRectAct->setIcon(QIcon(":/dvg/iceapp/images/clearselectrect.png"));
	d_data->clearSelectedRectAct->setStatusTip(tr("Удалить выделенные области"));
	connect(d_data->clearSelectedRectAct, SIGNAL(triggered()), this, SLOT(onClearSelectedRect()));
	addAction(d_data->clearSelectedRectAct);

	// Сюда надо добавить все action-ы которые должны на всем тулбаре совместно
	// работать как переключатели
	QActionGroup* globalActGrp = new QActionGroup(this);
	globalActGrp->addAction(d_data->moveAct);
	globalActGrp->addAction(d_data->pickerRectAct);
	globalActGrp->addAction(d_data->pickerPolygonAct);
	globalActGrp->addAction(d_data->measureAct);
}

IceToolsTlBr::~IceToolsTlBr()
{
	delete d_data;
}

ICEQWT_NS::ImageEditor* IceToolsTlBr::getImageEditor() const
{
	ICEMDI_NS::MdiSubWindow* mdiSubWindow = ((MainWindow*)(parent()))->activeMdiSubWindow();
	if(mdiSubWindow)
	{
		return mdiSubWindow->getImageEditor();
	}
	return 0;
}

void IceToolsTlBr::navigationToolTriggered(QAction* action)
{
	if(!action->isChecked())
	{
		return;
	}
	ICEQWT_NS::ImageEditor* imageEditor = getImageEditor();
	if(action == d_data->moveAct)
	{
		Q_ASSERT(imageEditor);
		imageEditor->setMode(ICEQWT_NS::ImageEditor::MOVE);
	}
}

void IceToolsTlBr::pickerToolTriggered(QAction* action)
{
	if(!action->isChecked())
	{
		return;
	}
	ICEQWT_NS::ImageEditor* imageEditor = getImageEditor();
	if(action == d_data->pickerRectAct)
	{
		Q_ASSERT(imageEditor);
		imageEditor->setMode(ICEQWT_NS::ImageEditor::PICKER_RECT);
	}
	else if(action == d_data->pickerPolygonAct)
	{
		Q_ASSERT(imageEditor);
		imageEditor->setMode(ICEQWT_NS::ImageEditor::PICKER_POLYGON);
	}
}

void IceToolsTlBr::onClearSelectedRect()
{
	ICEQWT_NS::ImageEditor* imageEditor = getImageEditor();
	Q_ASSERT(imageEditor);
	imageEditor->cleanSelectedArea();
	MainWindow* mainWnd = (MainWindow*)parent();
	mainWnd->m_cursorImageInfoWidget->updateState();
}

void IceToolsTlBr::measureActTriggeredSlot()
{
	if(!d_data->measureAct->isChecked())
	{
		return;
	}
	ICEQWT_NS::ImageEditor* imageEditor = getImageEditor();
	Q_ASSERT(imageEditor);
	imageEditor->setMode(ICEQWT_NS::ImageEditor::PICKER_POLYLINE);
}

void IceToolsTlBr::actualActions()
{
	ICEQWT_NS::ImageEditor* imageEditor = getImageEditor();
	setEnabled(imageEditor != 0);

	// Восстановление состояния режимов
	if(imageEditor)
	{
		ICEQWT_NS::ImageEditor::MODE mode = imageEditor->getMode();
		switch(mode)
		{
			case ICEQWT_NS::ImageEditor::MOVE:
			{
				QMenu* menu = d_data->navigationTlBtn->menu();
				menu->setActiveAction(d_data->moveAct);
				d_data->moveAct->setChecked(true);
				break;
			}
			case ICEQWT_NS::ImageEditor::PICKER_RECT:
			{
				QMenu* menu = d_data->pickerTlBtn->menu();
				menu->setActiveAction(d_data->pickerRectAct);
				d_data->pickerRectAct->setChecked(true);
				break;
			}
			case ICEQWT_NS::ImageEditor::PICKER_POLYGON:
			{
				QMenu* menu = d_data->pickerTlBtn->menu();
				menu->setActiveAction(d_data->pickerPolygonAct);
				d_data->pickerPolygonAct->setChecked(true);
				break;
			}
			case ICEQWT_NS::ImageEditor::PICKER_POLYLINE:
			{
				d_data->measureAct->setChecked(true);
				break;
			}
		}
	}
}

void IceToolsTlBr::setInitial()
{
	d_data->moveAct->trigger();
}
