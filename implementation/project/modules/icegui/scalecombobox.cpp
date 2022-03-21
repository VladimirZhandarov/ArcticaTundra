#include "scalecombobox.h"

#include <QtGui/QLineEdit>

#include <QDebug>

namespace ICEGUI_NS
{

ScaleComboBox::ScaleComboBox(QWidget* parent) : QComboBox(parent),
	m_scaleType(CURRENT_SCALE)
{
	addPercent(100); // Последний выбранный масштаб
	addItem(getString(TO_SIZE_SCALE), TO_SIZE_SCALE);
	addItem(getString(TO_HEIGHT_SCALE), TO_HEIGHT_SCALE);
	addItem(getString(TO_WIDTH_SCALE), TO_WIDTH_SCALE);
	addPercent(2);
	addPercent(5);
	addPercent(10);
	addPercent(25);
	addPercent(33);
	addPercent(50);
	addPercent(75);
	addPercent(100);
	addPercent(200);
	addPercent(300);
	addPercent(400);
	addPercent(500);
	addPercent(600);
	addPercent(800);
	addPercent(1000);
	addPercent(1200);
	addPercent(1600);
	addPercent(3200);

	setCurrentIndex(0);
	setMaxVisibleItems(count());
	setEditable(true);
	setValidator(new QRegExpValidator(QRegExp("^\\d{,4}%{,1}"), this));
	setInsertPolicy(QComboBox::NoInsert);
	connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
	connect(lineEdit(), SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

ScaleComboBox::~ScaleComboBox()
{

}

void ScaleComboBox::addPercent(unsigned int percent)
{
	addItem(QString::number(percent) + "%", percent);
}

void ScaleComboBox::setCurrentPercentType(int percentType)
{
	if(percentType > 0)
	{
		if(percentType < 2)
		{
			percentType = 2;
		}
		else if(percentType > 3200)
		{
			percentType = 3200;
		}
		setItemText(0, QString("%1%").arg(percentType));
	}
	else
	{
		setItemText(0, getString(percentType));
	}
	setItemData(0, percentType);
	setCurrentIndex(0);
	emit scaleChanged(percentType);
}

int ScaleComboBox::getCurrentPercentType() const
{
	return itemData(0).toInt();
}

void ScaleComboBox::setScaleType(int scaleType)
{
	m_scaleType = scaleType;
}

int ScaleComboBox::getScaleType() const
{
	return m_scaleType;
}

void ScaleComboBox::zoomIn()
{
	int currentScale = getCurrentPercentType();
  qDebug() << "Получили: " << currentScale;
	if(currentScale <= 0)
	{
		return;
	}
	for(int i=1; i<count(); i++)
	{
		int dataValue = itemData(i).toInt();
		if(dataValue <= 0)
		{
			continue;
		}
		if(dataValue > currentScale)
		{
			setCurrentPercentType(dataValue);
			break;
		}
	}
}

void ScaleComboBox::zoomOut()
{
	int currentScale = getCurrentPercentType();
	if(currentScale <= 0)
	{
		return;
	}
	for(int i=count()-1; i>0; i--)
	{
		int dataValue = itemData(i).toInt();
		if(dataValue <= 0)
		{
			continue;
		}
		if(dataValue < currentScale)
		{
			setCurrentPercentType(dataValue);
			break;
		}
	}
}

void ScaleComboBox::onActivated(int index)
{
	m_scaleType = itemData(index).toInt();
	setCurrentPercentType(m_scaleType);
}

void ScaleComboBox::onEditingFinished()
{
	QString text = lineEdit()->text();
	if(text.endsWith("%"))
	{
		text = text.remove(text.length()-1, 1);
	}
	setCurrentPercentType(text.toInt());
}

QString ScaleComboBox::getString(int type)
{
	if(type == TO_SIZE_SCALE)
	{
		return tr("Уместить всю");
	}
	else if(type == TO_HEIGHT_SCALE)
	{
		return tr("По высоте");
	}
	else if(type == TO_WIDTH_SCALE)
	{
		return tr("По ширине");
	}
	return "";
}

} // namespace ICEGUI_NS
