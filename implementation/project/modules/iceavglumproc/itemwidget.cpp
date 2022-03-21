#include "itemwidget.h"

#include <QtGui/QMessageBox>
#include <QtGui/QColorDialog>

#include <iceproc/imageprocinterface.h>
#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

namespace ICEAVGLUMPROC_NS
{

ItemWidget::ItemWidget(QWidget* parent) : QWidget(parent), m_index(0), m_proc(0),
	m_imageWindowInterface(0), m_redColor(Qt::red), m_greenColor(Qt::green),
	m_blueColor(Qt::blue), m_maxKDeltaValue(0.0), m_minKDeltaValue(0.0),
	m_imageBaseValue(0), m_channelType(Param::Y_TYPE), m_fromDb(false),
	m_maskCalced(false)
{
	ui.setupUi(this);
	yKMaskChB_stateChanged(Qt::Unchecked);
	setColor(Qt::red);
	setParamObject(QSharedPointer<Param>(new Param()));
}

ItemWidget::~ItemWidget()
{

}

void ItemWidget::init(int index, const ICEPROC_NS::ImageProcInterface* proc,
	ICEPROC_NS::ImageWindowInterface* imageWindowInterface, const QColor& color,
	const QColor& redColor, const QColor& greenColor, const QColor& blueColor)
{
	m_index = index;
	m_proc = proc;
	m_imageWindowInterface = imageWindowInterface;
	setColor(color);
	m_redColor = redColor;
	m_greenColor = greenColor;
	m_blueColor = blueColor;
	ui.maskGB->setTitle(getMaskName());
	ui.maskViewWidget->setMaskName(getMaskName());
}

void ItemWidget::setCheckable(bool checkable)
{
	ui.maskGB->setCheckable(checkable);
}

void ItemWidget::setTitle(const QString& name)
{
	ui.maskGB->setTitle(name);
}

void ItemWidget::clear()
{
	maskCancelPB_clicked();
	maskClearKPB_clicked();
	onMaskPBEnabled(false);
}

void ItemWidget::setMaskPBEnabled(bool enabled)
{
	onMaskPBEnabled(enabled);
}

void ItemWidget::setParamObject(const QSharedPointer<QObject>& param, bool fromDb)
{
	m_param = param;
	m_fromDb = fromDb;
	Param* item = qobject_cast<Param*>(param.data());
	if(item)
	{
		ui.maskGB->setChecked(item->isMaskUsed());
		ui.yMaskDSB->setValue(item->getLum());
		
		// Порядок имеет значение
		ui.yKMaskChB->setChecked(item->isKLumUsed());
		ui.yKMaskDSB->setValue(item->getKLum());
		ui.yKMinMaskDSB->setValue(item->getKMinLum());
		ui.yKMaxMaskDSB->setValue(item->getKMaxLum());

		if(!fromDb)
		{
			calcY();
		}
	}
}

QSharedPointer<QObject> ItemWidget::getParamObject() const
{
	QSharedPointer<QObject> result = m_param;
	if(result.isNull())
	{
		result = QSharedPointer<QObject>(new Param());
	}
	Param* item = qobject_cast<Param*>(result.data());
	Q_ASSERT(item);
	item->setMaskUsed(!ui.maskGB->isCheckable() || ui.maskGB->isChecked());
	item->setLum(ui.yMaskDSB->value());
	item->setKMinLum(ui.yKMinMaskDSB->value());
	item->setKMaxLum(ui.yKMaxMaskDSB->value());
	item->setKLum(ui.yKMaskDSB->value());
	item->setKLumUsed(ui.yKMaskChB->isChecked());
	if(!m_paramFilterBase.isNull())
	{
		Param* baseItem = qobject_cast<Param*>(m_paramFilterBase.data());
		Q_ASSERT(baseItem);
		item->setDontUseBlack(baseItem->isDontUseBlack());
		item->setDontUseBlackValue(baseItem->getDontUseBlackValue());
		item->setDontUseWhite(baseItem->isDontUseWhite());
		item->setDontUseWhiteValue(baseItem->isDontUseWhite());
	}
	return result;
}

void ItemWidget::setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	Q_ASSERT(maskView.get());
	ui.maskGB->setTitle(maskView->getName());
	ui.maskViewWidget->setMaskView(maskView);
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> ItemWidget::getMaskView() const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskView> result = ui.maskViewWidget->getMaskView();
	Q_ASSERT(result.get());
	result->setName(ui.maskGB->title());
	return result;
}

QString ItemWidget::getMaskName() const
{
	return ui.maskGB->title();
}

void ItemWidget::calcY()
{
	Param* item = qobject_cast<Param*>(m_param.data());
	if(!item)
	{
		return;
	}
	double yValue = 1.0;
	if(m_imageBaseValue > 0)
	{
		if(m_channelType == Param::Y_TYPE)
		{
			yValue = item->getAvgAbsoluteLum();
		}
		else if(m_channelType == Param::R_TYPE)
		{
			yValue = item->getAvgAbsoluteRed();
		}
		else if(m_channelType == Param::G_TYPE)
		{
			yValue = item->getAvgAbsoluteGreen();
		}
		else if(m_channelType == Param::B_TYPE)
		{
			yValue = item->getAvgAbsoluteBlue();
		}
		yValue = yValue / m_imageBaseValue;
	}
	ui.yMaskDSB->setValue(yValue);
}

bool ItemWidget::isCanUsed() const
{
	if(m_fromDb)
	{
		return true;
	}
	return !ui.maskGB->isChecked() || m_maskCalced;
}

void ItemWidget::maskGB_toggled(bool /*on*/)
{
	if(!m_fromDb)
	{
		emit maskChanged();
	}
}

void ItemWidget::maskCalcPB_clicked()
{
	Q_ASSERT(m_proc && m_imageWindowInterface);
	int maskType = Param::MASK_TYPE + m_index;
	QImage image = m_imageWindowInterface->getCurrentSelectedImage();
	QRect rect = m_imageWindowInterface->getCurrentSelectedRect();
	if(image.isNull())
	{
		QMessageBox::warning(this, tr(""), tr("Выберите область на изображении"));
		return;
	}
	setParamObject(m_proc->calcParamObject(maskType, m_paramFilterBase, image, rect));
	emit maskCreated(maskType, getMaskName());
	if(!m_fromDb)
	{
		m_maskCalced = true;
		ui.maskCalcPB->setEnabled(false);
		emit maskChanged();
	}
}

void ItemWidget::maskCancelPB_clicked()
{
	int maskType = Param::MASK_TYPE;
	Param param;
	ui.yMaskDSB->setValue(param.getLum());
	//ui.yKMaxMaskDSB->setValue(param.getKMaxLum());
	//ui.yKMinMaskDSB->setValue(param.getKMinLum());
	//ui.yKMaskChB->setChecked(param.isKLumUsed());
	//yKMaskChB_stateChanged(param.isKLumUsed());
	emit maskRemoved(maskType + m_index);
	if(!m_fromDb)
	{
		m_maskCalced = false;
		ui.maskCalcPB->setEnabled(true);
		emit maskChanged();
	}
}

void ItemWidget::maskAutoKPB_clicked()
{
	Param* item = qobject_cast<Param*>(m_param.data());
	if(item)
	{
		double delta = item->getAvgAbsoluteAutoDeltaLum();
		if(m_channelType == Param::R_TYPE)
		{
			delta = item->getAvgAbsoluteAutoDeltaRed();
		}
		else if(m_channelType == Param::G_TYPE)
		{
			delta = item->getAvgAbsoluteAutoDeltaGreen();
		}
		else if(m_channelType == Param::B_TYPE)
		{
			delta = item->getAvgAbsoluteAutoDeltaBlue();
		}
		double yValue = ui.yMaskDSB->value();
		ui.yKMaxMaskDSB->setValue((yValue + delta) / yValue);
		ui.yKMinMaskDSB->setValue((yValue - delta) / yValue);
	}
}

void ItemWidget::maskClearKPB_clicked()
{
	Param param;
	//ui.yKMaskDSB->setValue(param.getKLum());
	ui.yKMaxMaskDSB->setValue(param.getKMaxLum());
	ui.yKMinMaskDSB->setValue(param.getKMinLum());
	ui.yKMaskChB->setChecked(param.isKLumUsed());
	yKMaskChB_stateChanged(param.isKLumUsed());
}

void ItemWidget::yKMaskDSB_valueChanged(double value)
{
	ui.yKMaxMaskDSB->setValue(1 + value * m_maxKDeltaValue);
	ui.yKMinMaskDSB->setValue(1 - value * m_minKDeltaValue);
}

void ItemWidget::yKMaskChB_stateChanged(int state)
{
	ui.yKMaskL->setEnabled(state != Qt::Unchecked);
	ui.yKMaskDSB->setEnabled(state != Qt::Unchecked);
	ui.yKMaxMaskDSB->setReadOnly(state != Qt::Unchecked);
	ui.yKMinMaskDSB->setReadOnly(state != Qt::Unchecked);
	ui.maskAutoKPB->setEnabled(state == Qt::Unchecked);
	if(state != Qt::Unchecked)
	{
		m_maxKDeltaValue = (ui.yKMaxMaskDSB->value() - 1) / ui.yKMaskDSB->value();
		m_minKDeltaValue = (1 - ui.yKMinMaskDSB->value()) / ui.yKMaskDSB->value();
		yKMaskDSB_valueChanged(ui.yKMaskDSB->value());
	}
}

void ItemWidget::onImageBaseValueChanged(int imageBaseValue)
{
	m_imageBaseValue = imageBaseValue;
	if(m_maskCalced)
	{
		calcY();
	}
}

void ItemWidget::onMaskPBEnabled(bool enabled)
{
	if(!m_maskCalced)
	{
		ui.maskCalcPB->setEnabled(enabled);
	}
	else
	{
		ui.maskCalcPB->setEnabled(false);
	}
	ui.maskCancelPB->setEnabled(enabled);
}

void ItemWidget::onChannelTypeChanged(int channelType)
{
	m_channelType = (Param::YRGBChannelType)channelType;
	QColor colorText = Qt::black;
	if(m_channelType == Param::Y_TYPE)
	{
		ui.yMaskL->setText(tr("Отношение y:"));
	}
	else if(m_channelType == Param::R_TYPE)
	{
		colorText = m_redColor;
		ui.yMaskL->setText(tr("Отношение r:"));
	}
	else if(m_channelType == Param::G_TYPE)
	{
		colorText = m_greenColor;
		ui.yMaskL->setText(tr("Отношение g:"));
	}
	else if(channelType == Param::B_TYPE)
	{
		colorText = m_blueColor;
		ui.yMaskL->setText(tr("Отношение b:"));
	}
	QList<QLabel*> labelList;
	labelList.append(ui.yKMaxMaskL);
	labelList.append(ui.yKMinMaskL);
	labelList.append(ui.yMaskL);
	for(int i=0; i<labelList.size(); i++)
	{
		QPalette palette = labelList[i]->palette();
		palette.setColor(QPalette::Normal, labelList[i]->foregroundRole(), colorText);
		labelList[i]->setPalette(palette);
	}
	if(m_maskCalced)
	{
		calcY();
	}
}

void ItemWidget::onParamFilterBaseChanged(QSharedPointer<QObject> paramFilterBase)
{
	m_paramFilterBase = paramFilterBase;
}

void ItemWidget::setColor(const QColor& color)
{
	ui.maskViewWidget->setColor(color);
}

} // namespace ICEAVGLUMPROC_NS
