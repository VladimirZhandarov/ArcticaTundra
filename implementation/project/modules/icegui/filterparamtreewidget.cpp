#include "filterparamtreewidget.h"

#include <QtGui/QMenu>
#include <QtGui/QColorDialog>

#include <icecore/core.h>
#include <icecore/filteritemmodel.h>

#include <iceproc/imageprocinterface.h>

#include <iceodb/odb/filterparam.hxx>
#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

#include "filtermanagedialog.h"
#include "filtereditdialog.h"

namespace ICEGUI_NS
{

FilterParamTreeWidgetItem::FilterParamTreeWidgetItem(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
	int emptyMaskNumber, int index, QTreeWidget* parent) : QTreeWidgetItem(parent), m_filterParam(filterParam),
	m_index(index), m_defaultBackgroundBrush(background(0))
{
	Q_ASSERT(m_filterParam.get());
	ICECORE_NS::FilterItemModel* filterModel = ICECORE_NS::Core::Instance()->getFilterItemModel();
	Q_ASSERT(filterModel);
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterModel->getMaskParam(
		filterParam, index);
	if(maskParam.get())
	{
		std::tr1::shared_ptr<ICEODB_NS::LineView> lineView = maskParam->getLineView();
		if(!lineView.get())
		{
			m_maskView = maskParam->getMaskView();
			Q_ASSERT(m_maskView.get());
			setText(MASK_NAME_COL, m_maskView->getName());
			setContent(MASK_COLOR_COL, m_maskView.get());
		}
	}
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> lineParam = filterModel->getMaskParam(
		filterParam, index + 1);
	if(lineParam.get())
	{
		m_objectView = lineParam->getMaskView();
		m_lineView = lineParam->getLineView();
		if(m_objectView.get() && m_lineView.get())
		{
			setText(LINE_NAME_COL, m_objectView->getName());
			setContent(OBJECT_COLOR_COL, m_objectView.get());
			setContent(LINE_COLOR_COL, m_lineView.get());
		}
	}
	ICEPROC_NS::ImageProcInterface* proc = ICECORE_NS::Core::Instance()->getImageProc(
		m_filterParam->getFilterType()->getDllName());
	QString typeText = "";
	if(proc)
	{
		typeText = "(" + proc->getShortName();
		if(!isUsedMask() || !isUsedLine())
		{
			typeText += " " + QString::number(index - emptyMaskNumber + 1);
		}
		typeText += ")";
	}
	setText(NAME_COL, m_filterParam->getName() + " " + typeText);
}

FilterParamTreeWidgetItem::~FilterParamTreeWidgetItem()
{

}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterParamTreeWidgetItem::getFilterParam() const
{
	return m_filterParam;
}

int FilterParamTreeWidgetItem::getIndex() const
{
	return m_index;
}

bool FilterParamTreeWidgetItem::isUsedMask() const
{
	return (m_maskView.get() != 0);
}

bool FilterParamTreeWidgetItem::isUsedLine() const
{
	return (m_objectView.get() != 0 && m_lineView.get() != 0);
}

void FilterParamTreeWidgetItem::setColor(int columnNumber, const QColor& color)
{
	ICEODB_NS::MaskView* view = getMaskViewByColumn(columnNumber);
	if(view)
	{
		view->setColor(color);
		setContent(columnNumber, view);
	}
}

QColor FilterParamTreeWidgetItem::getColor(int columnNumber) const
{
	ICEODB_NS::MaskView* view = getMaskViewByColumn(columnNumber);
	if(view)
	{
		return view->getColor();
	}
	return ICEODB_NS::MaskView().getColor();
}

void FilterParamTreeWidgetItem::setUseImage(int columnNumber, bool useImage)
{
	ICEODB_NS::MaskView* view = getMaskViewByColumn(columnNumber);
	if(view)
	{
		view->setUseImage(useImage);
		setContent(columnNumber, view);
	}
}

bool FilterParamTreeWidgetItem::isUseImage(int columnNumber) const
{
	ICEODB_NS::MaskView* view = getMaskViewByColumn(columnNumber);
	if(view)
	{
		return view->isUseImage();
	}
	return ICEODB_NS::MaskView().isUseImage();
}

ICEODB_NS::MaskView* FilterParamTreeWidgetItem::getMaskViewByColumn(int columnNumber) const
{
	if(columnNumber == MASK_COLOR_COL)
	{
		return m_maskView.get();
	}
	if(columnNumber == OBJECT_COLOR_COL)
	{
		return m_objectView.get();
	}
	if(columnNumber == LINE_COLOR_COL)
	{
		return m_lineView.get();
	}
	return 0;
}

void FilterParamTreeWidgetItem::setContent(int columnNumber, ICEODB_NS::MaskView* view)
{
	Q_ASSERT(view);
	if(!view->isUseImage())
	{
		QBrush brush(view->getColor());
		setBackground(columnNumber, brush);
		setText(columnNumber, QObject::tr(""));
	}
	else
	{
		setBackground(columnNumber, m_defaultBackgroundBrush);
		setText(columnNumber, QObject::tr("Нет"));
	}
}

FilterParamTreeWidget::FilterParamTreeWidget(QWidget* parent) : QTreeWidget(parent),
	m_currentItem(0)
{
	m_contextMenu = new QMenu(this);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(onCustomContextMenuRequested(const QPoint&)));

	m_addAction = m_contextMenu->addAction(tr("Добавить дочерний фильтр"));
	connect(m_addAction, SIGNAL(triggered()), this, SLOT(addAction_triggered()));
	m_editAction = m_contextMenu->addAction(tr("Просмотреть дочерний фильтр"));
	connect(m_editAction, SIGNAL(triggered()), this, SLOT(editAction_triggered()));
	m_removeAction = m_contextMenu->addAction(tr("Удалить дочерний фильтр"));
	connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeAction_triggered()));
	m_contextMenu->addSeparator();

	m_editMaskColorAction = m_contextMenu->addAction(tr("Цвет маски"));
	connect(m_editMaskColorAction, SIGNAL(triggered()), this, SLOT(editMaskColorAction_triggered()));
	m_disableMaskColorAction = m_contextMenu->addAction(tr("Выкл. маску"));
	m_disableMaskColorAction->setCheckable(true);
	connect(m_disableMaskColorAction, SIGNAL(triggered(bool)), this, SLOT(disableMaskColorAction_triggered(bool)));

	m_editObjectColorAction = m_contextMenu->addAction(tr("Цвет заливки объектов"));
	connect(m_editObjectColorAction, SIGNAL(triggered()), this, SLOT(editObjectColorAction_triggered()));
	m_disableObjectColorAction = m_contextMenu->addAction(tr("Выкл. заливку объектов"));
	m_disableObjectColorAction->setCheckable(true);
	connect(m_disableObjectColorAction, SIGNAL(triggered(bool)), this, SLOT(disableObjectColorAction_triggered(bool)));

	m_editLineColorAction = m_contextMenu->addAction(tr("Цвет контуров объектов"));
	connect(m_editLineColorAction, SIGNAL(triggered()), this, SLOT(editLineColorAction_triggered()));
	m_disableLineColorAction = m_contextMenu->addAction(tr("Выкл. контуры объектов"));
	m_disableLineColorAction->setCheckable(true);
	connect(m_disableLineColorAction, SIGNAL(triggered(bool)), this, SLOT(disableLineColorAction_triggered(bool)));
}

FilterParamTreeWidget::~FilterParamTreeWidget()
{

}

int FilterParamTreeWidget::resizeHeaderToContent(int addedWidth)
{
	setColumnCount(NUM_COLS);
	QTreeWidgetItem* headerWidgetItem = headerItem();
	QHeaderView* headerView = header();
	Q_ASSERT(headerWidgetItem && headerView);
	QFontMetrics metrics = fontMetrics();
	int result = 0;
	for(int i=0; i<columnCount(); i++)
	{
		int width = metrics.width(headerWidgetItem->text(i)) + addedWidth;
		result += width;
		setColumnWidth(i, width);
	}
	return result;
}

int FilterParamTreeWidget::setChildFilterParam(int index,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam)
{
	Q_ASSERT(filterParam);
	ICECORE_NS::FilterItemModel* filterModel = ICECORE_NS::Core::Instance()->getFilterItemModel();
	Q_ASSERT(filterModel);
	int result = 0;
	int emptyMaskNumber = 0;
	for(int i=0; i<filterParam->getMaskParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterModel->getMaskParam(
			filterParam, i);
		if(maskParam.get())
		{
			std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
			if(maskView.get())
			{
				emptyMaskNumber = i;
				break;
			}
		}
	}
	for(int i=emptyMaskNumber; i<filterParam->getMaskParamCount(); i++)
	{
		FilterParamTreeWidgetItem* item = new FilterParamTreeWidgetItem(filterParam,
			emptyMaskNumber, i, this);
		if(item->isUsedMask() && item->isUsedLine())
		{
			i++;
		}
		insertTopLevelItem(index + result, item);
		result++;
	}
	return result;
}

int FilterParamTreeWidget::getChildFilterParamCount() const
{
	return topLevelItemCount();
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterParamTreeWidget::getChildFilterParam(int index) const
{
	FilterParamTreeWidgetItem* item = dynamic_cast<FilterParamTreeWidgetItem*>(
		topLevelItem(index));
	if(item)
	{
		return item->getFilterParam();
	}
	else
	{
		return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
}

void FilterParamTreeWidget::setContextMenuEnabled(bool enabled)
{
	if(enabled)
	{
		setContextMenuPolicy(Qt::CustomContextMenu);
	}
	else
	{
		setContextMenuPolicy(Qt::DefaultContextMenu);
	}
}

void FilterParamTreeWidget::onCustomContextMenuRequested(const QPoint& pos)
{
	m_currentItem = itemAt(pos);
	bool flag = false;
	if(m_currentItem)
	{
		flag = true;
	}
	m_editAction->setEnabled(flag);
	m_removeAction->setEnabled(flag);
	FilterParamTreeWidgetItem* item = dynamic_cast<FilterParamTreeWidgetItem*>(m_currentItem);
	if(!item)
	{
		m_editMaskColorAction->setEnabled(false);
		m_disableMaskColorAction->setChecked(false);
		m_disableMaskColorAction->setEnabled(false);
		m_editObjectColorAction->setEnabled(false);
		m_disableObjectColorAction->setChecked(false);
		m_disableObjectColorAction->setEnabled(false);
		m_editLineColorAction->setEnabled(false);
		m_disableLineColorAction->setChecked(false);
		m_disableLineColorAction->setEnabled(false);
	}
	else
	{
		if(!item->isUsedMask())
		{
			m_editMaskColorAction->setEnabled(false);
			m_disableMaskColorAction->setChecked(false);
			m_disableMaskColorAction->setEnabled(false);
		}
		else
		{
			m_editMaskColorAction->setEnabled(true);
			m_disableMaskColorAction->setChecked(item->isUseImage(MASK_COLOR_COL));
			m_disableMaskColorAction->setEnabled(true);
		}

		if(!item->isUsedLine())
		{
			m_editObjectColorAction->setEnabled(false);
			m_disableObjectColorAction->setChecked(false);
			m_disableObjectColorAction->setEnabled(false);
			m_editLineColorAction->setEnabled(false);
			m_disableLineColorAction->setChecked(false);
			m_disableLineColorAction->setEnabled(false);
		}
		else
		{
			m_editObjectColorAction->setEnabled(true);
			m_disableObjectColorAction->setChecked(item->isUseImage(OBJECT_COLOR_COL));
			m_disableObjectColorAction->setEnabled(true);
			m_editLineColorAction->setEnabled(true);
			m_disableLineColorAction->setChecked(item->isUseImage(LINE_COLOR_COL));
			m_disableLineColorAction->setEnabled(true);
		}
	}
	m_contextMenu->popup(mapToGlobal(pos));
}

void FilterParamTreeWidget::addAction_triggered()
{
	QStringList keyList = ICECORE_NS::Core::Instance()->getImageProcKey();
	//TODO refac
	keyList.removeAll("icegroupproc_d");
	FilterManageDialog dialog(keyList, this);
	dialog.setModel(ICECORE_NS::Core::Instance()->getFilterItemModel());
	if(dialog.exec() == QDialog::Accepted)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam = dialog.getCurrentFilterParam();
		Q_ASSERT(filterParam);
		ICECORE_NS::FilterItemModel* filterModel = ICECORE_NS::Core::Instance()->getFilterItemModel();
		Q_ASSERT(filterModel);
		int emptyMaskNumber = 0;
		for(int i=0; i<filterParam->getMaskParamCount(); i++)
		{
			std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterModel->getMaskParam(
				filterParam, i);
			if(maskParam.get())
			{
				std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
				if(maskView.get())
				{
					emptyMaskNumber = i;
					break;
				}
			}
		}
		for(int i=emptyMaskNumber; i<filterParam->getMaskParamCount(); i++)
		{
			FilterParamTreeWidgetItem* item = new FilterParamTreeWidgetItem(filterParam,
				emptyMaskNumber, i, this);
			if(item->isUsedMask() && item->isUsedLine())
			{
				i++;
			}
			addTopLevelItem(item);
		}
	}
}

void FilterParamTreeWidget::editAction_triggered()
{
	if(!m_currentItem)
	{
		return;
	}
	FilterParamTreeWidgetItem* item = dynamic_cast<FilterParamTreeWidgetItem*>(
		m_currentItem);
	if(!item)
	{
		return;
	}
	ICEPROC_NS::ImageProcInterface* proc = ICECORE_NS::Core::Instance()->getImageProc(
		item->getFilterParam()->getFilterType()->getDllName());
	if(proc)
	{
		FilterEditDialog editDialog(item->getFilterParam(), proc, this);
		editDialog.setModel(ICECORE_NS::Core::Instance()->getFilterItemModel());
		editDialog.init();
		editDialog.exec();
	}
}

void FilterParamTreeWidget::removeAction_triggered()
{
	int index = indexOfTopLevelItem(m_currentItem);
	if(index < 0)
	{
		return;
	}
	QTreeWidgetItem* item = takeTopLevelItem(index);
	delete item;
	m_currentItem = 0;
}

void FilterParamTreeWidget::editMaskColorAction_triggered()
{
	showColorDialog(MASK_COLOR_COL);
}

void FilterParamTreeWidget::disableMaskColorAction_triggered(bool checked)
{
	setUseImage(MASK_COLOR_COL, checked);
}

void FilterParamTreeWidget::editObjectColorAction_triggered()
{
	showColorDialog(OBJECT_COLOR_COL);
}

void FilterParamTreeWidget::disableObjectColorAction_triggered(bool checked)
{
	setUseImage(OBJECT_COLOR_COL, checked);
}

void FilterParamTreeWidget::editLineColorAction_triggered()
{
	showColorDialog(LINE_COLOR_COL);
}

void FilterParamTreeWidget::disableLineColorAction_triggered(bool checked)
{
	setUseImage(LINE_COLOR_COL, checked);
}

void FilterParamTreeWidget::showColorDialog(int colunmNumber)
{
	FilterParamTreeWidgetItem* item = dynamic_cast<FilterParamTreeWidgetItem*>(m_currentItem);
	if(item)
	{
		QColorDialog dialog(item->getColor(colunmNumber), this);
		dialog.setOption(QColorDialog::ShowAlphaChannel);
		if(dialog.exec() == QDialog::Accepted)
		{
			item->setColor(colunmNumber, dialog.currentColor());
		}
	}
}

void FilterParamTreeWidget::setUseImage(int colunmNumber, bool checked)
{
	FilterParamTreeWidgetItem* item = dynamic_cast<FilterParamTreeWidgetItem*>(m_currentItem);
	if(item)
	{
		item->setUseImage(colunmNumber, checked);
	}
}

} // namespace ICEGUI_NS
