#include "topitemwidget.h"

#include <QtGui/QMessageBox>
#include <QtGui/QColorDialog>

#include <iceproc/imageprocinterface.h>
#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

namespace ICEAVGLUMPROC_NS
{

TopItemWidget::TopItemWidget(QWidget* parent) : QWidget(parent), m_proc(0),
	m_imageWindowInterface(0), m_redColor(Qt::red), m_greenColor(Qt::green), m_blueColor(Qt::blue),
	m_channelType(Param::Y_TYPE), m_baseImageCalced(false), m_filterParamId(0)
{
	ui.setupUi(this);

	rb_toggled(true);

	setParamObject(Param::FILTER_BASE_MASK_TYPE, QSharedPointer<Param>(new Param()));
	setParamObject(Param::IMAGE_BASE_MASK_TYPE, QSharedPointer<Param>(new Param()));
}

TopItemWidget::~TopItemWidget()
{

}

void TopItemWidget::init(const ICEPROC_NS::ImageProcInterface* proc,
	ICEPROC_NS::ImageWindowInterface* imageWindowInterface)
{
	m_proc = proc;
	m_imageWindowInterface = imageWindowInterface;
	QPalette paletteR = ui.rFilterBaseL->palette();
	m_redColor = paletteR.color(QPalette::Normal, ui.rFilterBaseL->foregroundRole());
	QPalette paletteG = ui.gFilterBaseL->palette();
	m_greenColor = paletteG.color(QPalette::Normal, ui.gFilterBaseL->foregroundRole());
	QPalette paletteB = ui.bFilterBaseL->palette();
	m_blueColor = paletteB.color(QPalette::Normal, ui.bFilterBaseL->foregroundRole());
}

void TopItemWidget::clear()
{
	baseCancelPB_clicked();
}

void TopItemWidget::setParamObject(int index, const QSharedPointer<QObject>& param,
	bool fromDb)
{
	if(index == Param::FILTER_BASE_MASK_TYPE)
	{
		m_paramFilterBase = param;
		emit paramFilterBaseChanged(param);
	}
	else if(index == Param::IMAGE_BASE_MASK_TYPE)
	{
		m_paramImageBase = param;
	}
	Param* item = qobject_cast<Param*>(param.data());
	if(item)
	{
		if(index == Param::FILTER_BASE_MASK_TYPE)
		{
			ui.yFilterBaseSB->setValue(item->getAvgAbsoluteLum());
			ui.rFilterBaseSB->setValue(item->getAvgAbsoluteRed());
			ui.gFilterBaseSB->setValue(item->getAvgAbsoluteGreen());
			ui.bFilterBaseSB->setValue(item->getAvgAbsoluteBlue());

			ui.dontUseBlackChB->setChecked(item->isDontUseBlack());
			ui.dontUseBlackSB->setValue(item->getDontUseBlackValue());
			ui.dontUseWhiteChB->setChecked(item->isDontUseWhite());
			ui.dontUseWhiteSB->setValue(item->getDontUseWhiteValue());
			dontUseBlackChB_stateChanged(ui.dontUseBlackChB->checkState());
			dontUseWhiteChB_stateChanged(ui.dontUseWhiteChB->checkState());

			ui.filterBaseSizeValueL->setText(item->getBaseSizeStringValue());
			if(!fromDb)
			{
				ui.yImageBaseSB->setValue(item->getAvgAbsoluteLum());
				ui.rImageBaseSB->setValue(item->getAvgAbsoluteRed());
				ui.gImageBaseSB->setValue(item->getAvgAbsoluteGreen());
				ui.bImageBaseSB->setValue(item->getAvgAbsoluteBlue());
				ui.imageBaseSizeValueL->setText(item->getBaseSizeStringValue());
			}
			ui.useRectForBaseChB->setChecked(item->isUseRectForBase());
			ui.useRectForBaseChB->setEnabled(!fromDb);
			m_channelType = item->getChannelType();
			setImageBaseValue();
			m_baseImageCalced = true;
		}
		else if(index == Param::IMAGE_BASE_MASK_TYPE)
		{
			if(!fromDb)
			{
				ui.yImageBaseSB->setValue(item->getAvgAbsoluteLum());
				ui.rImageBaseSB->setValue(item->getAvgAbsoluteRed());
				ui.gImageBaseSB->setValue(item->getAvgAbsoluteGreen());
				ui.bImageBaseSB->setValue(item->getAvgAbsoluteBlue());
				ui.imageBaseSizeValueL->setText(item->getBaseSizeStringValue());
			}
		}
	}
}

QSharedPointer<QObject> TopItemWidget::getParamObject(int index) const
{
	QSharedPointer<QObject> result;
	if(index == Param::FILTER_BASE_MASK_TYPE)
	{
		result = m_paramFilterBase;
	}
	else if(index == Param::IMAGE_BASE_MASK_TYPE)
	{
		result = m_paramImageBase;
	}
	if(result.isNull())
	{
		result = QSharedPointer<QObject>(new Param());
	}
	Param* item = qobject_cast<Param*>(result.data());
	Q_ASSERT(item);
	if(index == Param::FILTER_BASE_MASK_TYPE)
	{
		item->setAvgAbsoluteLum(ui.yFilterBaseSB->value());
		item->setAvgAbsoluteRed(ui.rFilterBaseSB->value());
		item->setAvgAbsoluteGreen(ui.gFilterBaseSB->value());
		item->setAvgAbsoluteBlue(ui.bFilterBaseSB->value());

		item->setDontUseBlack(ui.dontUseBlackChB->isChecked());
		item->setDontUseBlackValue(ui.dontUseBlackSB->value());
		item->setDontUseWhite(ui.dontUseWhiteChB->isChecked());
		item->setDontUseWhiteValue(ui.dontUseWhiteSB->value());

		item->setUseRectForBase(ui.useRectForBaseChB->isChecked());
		item->setChannelType(m_channelType);
	}
	else if(index == Param::IMAGE_BASE_MASK_TYPE)
	{
		item->setAvgAbsoluteLum(ui.yImageBaseSB->value());
		item->setAvgAbsoluteRed(ui.rImageBaseSB->value());
		item->setAvgAbsoluteGreen(ui.gImageBaseSB->value());
		item->setAvgAbsoluteBlue(ui.bImageBaseSB->value());
	}
	return result;
}

void TopItemWidget::setFilterParamId(unsigned int filterParamId)
{
	m_filterParamId = filterParamId;

	bool flag = (filterParamId == 0);
	bool prevFlag = (m_filterParamId == 0);

	if(flag)
	{
		if(!prevFlag)
		{
			connect(ui.yFilterBaseSB, SIGNAL(valueChanged(int)), ui.yImageBaseSB, SLOT(setValue(int)));
			connect(ui.rFilterBaseSB, SIGNAL(valueChanged(int)), ui.rImageBaseSB, SLOT(setValue(int)));
			connect(ui.gFilterBaseSB, SIGNAL(valueChanged(int)), ui.gImageBaseSB, SLOT(setValue(int)));
			connect(ui.bFilterBaseSB, SIGNAL(valueChanged(int)), ui.bImageBaseSB, SLOT(setValue(int)));
		}
	}
	else
	{
		if(prevFlag)
		{
			disconnect(ui.yFilterBaseSB, SIGNAL(valueChanged(int)), ui.yImageBaseSB, SLOT(setValue(int)));
			disconnect(ui.rFilterBaseSB, SIGNAL(valueChanged(int)), ui.rImageBaseSB, SLOT(setValue(int)));
			disconnect(ui.gFilterBaseSB, SIGNAL(valueChanged(int)), ui.gImageBaseSB, SLOT(setValue(int)));
			disconnect(ui.bFilterBaseSB, SIGNAL(valueChanged(int)), ui.bImageBaseSB, SLOT(setValue(int)));
		}
	}
}

const QColor& TopItemWidget::getRedColor() const
{
	return m_redColor;
}

const QColor& TopItemWidget::getGreenColor() const
{
	return m_greenColor;
}

const QColor& TopItemWidget::getBlueColor() const
{
	return m_blueColor;
}

void TopItemWidget::setImageBaseValue()
{
	int imageBaseValue = ui.yFilterBaseSB->value();
	if(m_channelType == Param::R_TYPE)
	{
		imageBaseValue =  ui.rFilterBaseSB->value();
	}
	else if(m_channelType == Param::G_TYPE)
	{
		imageBaseValue = ui.gFilterBaseSB->value();
	}
	else if(m_channelType == Param::B_TYPE)
	{
		imageBaseValue = ui.bFilterBaseSB->value();
	}
	emit imageBaseValueChanged(imageBaseValue);
}

bool TopItemWidget::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	if(m_filterParamId == 0)
	{
		if(!m_baseImageCalced)
		{
			return baseCalc(Param::FILTER_BASE_MASK_TYPE, imageBuilder);
		}
		else
		{
			return baseCalc(Param::IMAGE_BASE_MASK_TYPE, imageBuilder);
		}
	}
	else
	{
		return baseCalc(Param::IMAGE_BASE_MASK_TYPE, imageBuilder);
	}
}

bool TopItemWidget::isCanUsed() const
{
	return m_baseImageCalced;
}

void TopItemWidget::baseCalcPB_clicked()
{
	Param::MaskType maskType = Param::FILTER_BASE_MASK_TYPE;
	if(m_filterParamId != 0)
	{
		maskType = Param::IMAGE_BASE_MASK_TYPE;
	}
	if(baseCalc(maskType))
	{
		m_baseImageCalced = true;
		ui.baseCalcPB->setEnabled(false);
		emit baseChanged();
	}
}

void TopItemWidget::baseCancelPB_clicked()
{
	QSharedPointer<QObject> param(new Param());
	bool useRect = ui.useRectForBaseChB->isChecked();
	if(m_filterParamId == 0)
	{
		setParamObject(Param::FILTER_BASE_MASK_TYPE, param);
		emit maskPBEnabled(false);
	}
	else
	{
		setParamObject(Param::IMAGE_BASE_MASK_TYPE, param);
	}
	ui.useRectForBaseChB->setChecked(useRect);
	if(m_filterParamId == 0)
	{
		m_baseImageCalced = false;
	}
	ui.baseCalcPB->setEnabled(true);
	emit baseChanged();
}

void TopItemWidget::rb_toggled(bool checked)
{
	if(checked)
	{
		QObject* senderObject = sender();

		ui.yFilterBaseL->setEnabled(false);
		ui.yFilterBaseSB->setEnabled(false);
		ui.rFilterBaseL->setEnabled(false);
		ui.rFilterBaseSB->setEnabled(false);
		ui.gFilterBaseL->setEnabled(false);
		ui.gFilterBaseSB->setEnabled(false);
		ui.bFilterBaseL->setEnabled(false);
		ui.bFilterBaseSB->setEnabled(false);

		ui.yImageBaseL->setEnabled(false);
		ui.yImageBaseSB->setEnabled(false);
		ui.rImageBaseL->setEnabled(false);
		ui.rImageBaseSB->setEnabled(false);
		ui.gImageBaseL->setEnabled(false);
		ui.gImageBaseSB->setEnabled(false);
		ui.bImageBaseL->setEnabled(false);
		ui.bImageBaseSB->setEnabled(false);

		if(senderObject == ui.yRB || !senderObject)
		{
			ui.yFilterBaseL->setEnabled(true);
			ui.yFilterBaseSB->setEnabled(true);
			ui.yImageBaseL->setEnabled(true);
			ui.yImageBaseSB->setEnabled(true);
			m_channelType = Param::Y_TYPE;
		}

		if(senderObject == ui.rRB)
		{
			ui.rFilterBaseL->setEnabled(true);
			ui.rFilterBaseSB->setEnabled(true);
			ui.rImageBaseL->setEnabled(true);
			ui.rImageBaseSB->setEnabled(true);
			m_channelType = Param::R_TYPE;
		}

		if(senderObject == ui.gRB)
		{
			ui.gFilterBaseL->setEnabled(true);
			ui.gFilterBaseSB->setEnabled(true);
			ui.gImageBaseL->setEnabled(true);
			ui.gImageBaseSB->setEnabled(true);
			m_channelType = Param::G_TYPE;
		}

		if(senderObject == ui.bRB)
		{
			ui.bFilterBaseL->setEnabled(true);
			ui.bFilterBaseSB->setEnabled(true);
			ui.bImageBaseL->setEnabled(true);
			ui.bImageBaseSB->setEnabled(true);
			m_channelType = Param::B_TYPE;
		}
		emit channelTypeChanged(m_channelType);
		setImageBaseValue();
	}
}

void TopItemWidget::dontUseBlackChB_stateChanged(int state)
{
	ui.dontUseBlackSB->setEnabled(state == Qt::Checked);
}

void TopItemWidget::dontUseWhiteChB_stateChanged(int state)
{
	ui.dontUseWhiteSB->setEnabled(state == Qt::Checked);
}

bool TopItemWidget::baseCalc(int maskType, const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	Q_ASSERT(m_proc && m_imageWindowInterface);
	QImage image;

	if(m_paramFilterBase.isNull())
	{
		m_paramFilterBase = QSharedPointer<QObject>(new Param()); 
	}
	Param* item = qobject_cast<Param*>(m_paramFilterBase.data());
	item->setDontUseBlack(ui.dontUseBlackChB->isChecked());
	item->setDontUseBlackValue(ui.dontUseBlackSB->value());
	item->setDontUseWhite(ui.dontUseWhiteChB->isChecked());
	item->setDontUseWhiteValue(ui.dontUseWhiteSB->value());

	if(ui.useRectForBaseChB->checkState() != Qt::Unchecked)
	{
		QRect rect;
		if(maskType == Param::FILTER_BASE_MASK_TYPE)
		{
			if(!imageBuilder.isNull())
			{
				image = imageBuilder->getUserCreatedObjectQImage();
				rect = imageBuilder->getUserCreatedObjectRect();
			}
			else
			{
				image = m_imageWindowInterface->getCurrentSelectedImage();
				rect = m_imageWindowInterface->getCurrentSelectedRect();
			}
			if(image.isNull())
			{
				QMessageBox::warning(this, tr(""), tr("Выберите область на изображении"));
				return false;
			}
		}
		else if(maskType == Param::IMAGE_BASE_MASK_TYPE)
		{
			Param* item = qobject_cast<Param*>(m_paramFilterBase.data());
			if(item && !item->getBaseImageRect().isNull())
			{
				rect = item->getBaseImageRect();
				if(!imageBuilder.isNull())
				{
					imageBuilder->setUserCreatedObject(rect);
					image = imageBuilder->getUserCreatedObjectQImage();
				}
				else
				{
					image = m_imageWindowInterface->getCurrentSelectedImage(rect);
				}
			}
			else
			{
				return false;
			}
		}
		QSharedPointer<QObject> param = m_proc->calcParamObject(maskType, m_paramFilterBase,
			image, rect);
		Param* item = qobject_cast<Param*>(param.data());
		item->setUseRectForBase(true);
		setParamObject(maskType, param);
	}
	else
	{
		if(!imageBuilder.isNull())
		{
			image = imageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::RESULT_IMAGE_AND_MASK,
				QRect(), true);
		}
		else
		{
			image = m_imageWindowInterface->getCurrentImage();
		}
		if(image.isNull())
		{
			QMessageBox::warning(this, tr(""), tr("Выберите изображение"));
			return false;
		}
		QSharedPointer<QObject> param = m_proc->calcParamObject(maskType, m_paramFilterBase,
			image, QRect());
		Param* item = qobject_cast<Param*>(param.data());
		item->setUseRectForBase(false);
		setParamObject(maskType, param);
	}
	if(m_filterParamId == 0)
	{
		emit maskPBEnabled(true);
	}
	return true;
}

} // namespace ICEAVGLUMPROC_NS
