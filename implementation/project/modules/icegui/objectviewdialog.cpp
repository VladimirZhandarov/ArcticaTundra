#include "objectviewdialog.h"

#include "objecttablewidget.h"

#include <iceimage/imagebuilder.h>
#include <iceimage/contouritemmodel.h>

#include <QtGui/QCloseEvent>
#include <QtGui/QColorDialog>

namespace ICEGUI_NS
{

ObjectViewDialog::ObjectViewDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder, 
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_imageBuilder(imageBuilder)
{
	ui.setupUi(this);

	connect(ui.maskViewWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(onColorChanged(const QColor&)));
	ui.maskViewWidget->setLabelText(tr("Заливка выделенного объекта:"));
	ui.maskViewWidget->setColor(Qt::darkGreen);
	ui.maskViewWidget->setAlpha(20);
	ui.maskViewWidget->setUseChBVisible(false);
}

ObjectViewDialog::~ObjectViewDialog()
{

}

void ObjectViewDialog::updateState(ICEQWT_NS::IceUnitRatioQwtTransform* transformation,
	bool halfArcLength)
{
	for(int i=0; i<ui.tabWidget->count(); i++)
	{
		ObjectTableWidget* item = qobject_cast<ObjectTableWidget*>(ui.tabWidget->widget(i));
		if(item)
		{
			item->updateState(transformation, halfArcLength);
		}
	}
}

void ObjectViewDialog::closeEvent(QCloseEvent* event)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setCurrentContoursIndex(-1);
	event->accept();
	emit finished(0);
}

void ObjectViewDialog::show()
{
	Q_ASSERT(!m_imageBuilder.isNull());
	ui.tabWidget->clear();
	for(int i=0; i<m_imageBuilder->getContourItemModelListSize(); i++)
	{
		ObjectTableWidget* table = new ObjectTableWidget(m_imageBuilder, i, ui.tabWidget);
		if(!table->isNull())
		{
			ui.tabWidget->addTab(table, QString::number(i + 1) + ": "+ table->getName());
		}
		else
		{
			delete table;
		}
	}
	QDialog::show();
}

void ObjectViewDialog::tabWidget_currentChanged(int index)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setCurrentContoursIndex(index);
}

void ObjectViewDialog::onColorChanged(const QColor& color)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setCurrentContourColor(color);
}

void ObjectViewDialog::onMouseDoubleClicked()
{
	Q_ASSERT(!m_imageBuilder.isNull());
	int tabIndex = m_imageBuilder->getCurrentContoursIndex();
	if(isVisible() && tabIndex != -1)
	{
		ui.tabWidget->setCurrentIndex(tabIndex);
		QDialog::hide();
		QDialog::show();
		ObjectTableWidget* table = dynamic_cast<ObjectTableWidget*>(ui.tabWidget->currentWidget());
		if(table)
		{
			table->currentRowChange();
		}
	}
}
} // namespace ICEGUI_NS
