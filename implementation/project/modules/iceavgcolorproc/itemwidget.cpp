#include "itemwidget.h"

#include <QtGui/QMessageBox>
#include <QtGui/QColorDialog>

#include <iceproc/imageprocinterface.h>
#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/maskparam.hxx>
#include <iceodb/odb/maskview.hxx>

namespace ICEAVGCOLORPROC_NS
{

ItemWidget::ItemWidget(QWidget* parent) : QWidget(parent), m_index(0), m_proc(0),
	m_imageWindowInterface(0), m_redColor(Qt::red), m_greenColor(Qt::green),
	m_blueColor(Qt::blue), m_upRDeltaValue(0.0), m_downRDeltaValue(0.0),
	m_upGDeltaValue(0.0), m_downGDeltaValue(0.0), m_upBDeltaValue(0.0),
	m_downBDeltaValue(0.0)
{
	ui.setupUi(this);
	rgbKMaskChB_stateChanged(Qt::Unchecked);
	setColor(Qt::red);
	setParamObject(QSharedPointer<Param>(new Param()));
}

ItemWidget::~ItemWidget()
{

}

void ItemWidget::init(int index, const ICEPROC_NS::ImageProcInterface* proc,
	ICEPROC_NS::ImageWindowInterface* imageWindowInterface, const QColor& color)
{
	m_index = index;
	m_proc = proc;
	m_imageWindowInterface = imageWindowInterface;
	setColor(color);
	ui.maskGB->setTitle(getMaskName());
	ui.maskViewWidget->setMaskName(getMaskName());
}

void ItemWidget::setTitle(const QString& name)
{
	ui.maskGB->setTitle(name);
}

void ItemWidget::setCheckable(bool checkable)
{
	ui.maskGB->setCheckable(checkable);
}

void ItemWidget::clear()
{
	maskCancelPB_clicked();
	maskClearKPB_clicked();
}

void ItemWidget::setMaskPBEnabled(bool flag)
{
	ui.maskCalcPB->setEnabled(flag);
	ui.maskCancelPB->setEnabled(flag);
}

void ItemWidget::setParamObject(const QSharedPointer<QObject>& param, bool fromDb)
{
	m_param = param;
	Param* item = qobject_cast<Param*>(param.data());
	if(item)
	{
		ui.maskGB->setChecked(item->isMaskUsed());

		ui.redRelativeDSB->setValue(item->getAvgRelativeRed());
		ui.greenRelativeDSB->setValue(item->getAvgRelativeGreen());
		ui.blueRelativeDSB->setValue(item->getAvgRelativeBlue());

		ui.redAbsSB->setValue(item->getAvgAbsoluteRed());
		ui.greenAbsSB->setValue(item->getAvgAbsoluteGreen());
		ui.blueAbsSB->setValue(item->getAvgAbsoluteBlue());

		ui.rgbKMaskDSB->setValue(item->getKRgb());
		ui.rgbKMaskChB->setChecked(item->isKRgbUsed());

		ui.dontUseBlackChB->setChecked(item->isDontUseBlack());
		ui.dontUseBlackSB->setValue(item->getDontUseBlackValue());
		ui.dontUseWhiteChB->setChecked(item->isDontUseWhite());
		ui.dontUseWhiteSB->setValue(item->getDontUseWhiteValue());
		dontUseBlackChB_stateChanged(ui.dontUseBlackChB->checkState());
		dontUseWhiteChB_stateChanged(ui.dontUseWhiteChB->checkState());

		if(fromDb)
		{
			ui.redDeltaDownDSB->setValue(item->getDeltaMinusRelativeRed());
			ui.greenDeltaDownDSB->setValue(item->getDeltaMinusRelativeGreen());
			ui.blueDeltaDownDSB->setValue(item->getDeltaMinusRelativeBlue());

			ui.redDeltaUpDSB->setValue(item->getDeltaPlusRelativeRed());
			ui.greenDeltaUpDSB->setValue(item->getDeltaPlusRelativeGreen());
			ui.blueDeltaUpDSB->setValue(item->getDeltaPlusRelativeBlue());
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

	item->setAvgRelativeRed(ui.redRelativeDSB->value());
	item->setAvgRelativeGreen(ui.greenRelativeDSB->value());
	item->setAvgRelativeBlue(ui.blueRelativeDSB->value());

	item->setAvgAbsoluteRed(ui.redAbsSB->value());
	item->setAvgAbsoluteGreen(ui.greenAbsSB->value());
	item->setAvgAbsoluteBlue(ui.blueAbsSB->value());

	item->setDeltaMinusRelativeRed(ui.redDeltaDownDSB->value());
	item->setDeltaMinusRelativeGreen(ui.greenDeltaDownDSB->value());
	item->setDeltaMinusRelativeBlue(ui.blueDeltaDownDSB->value());

	item->setDeltaPlusRelativeRed(ui.redDeltaUpDSB->value());
	item->setDeltaPlusRelativeGreen(ui.greenDeltaUpDSB->value());
	item->setDeltaPlusRelativeBlue(ui.blueDeltaUpDSB->value());

	item->setKRgb(ui.rgbKMaskDSB->value());
	item->setKRgbUsed(ui.rgbKMaskChB->isChecked());

	item->setDontUseBlack(ui.dontUseBlackChB->isChecked());
	item->setDontUseBlackValue(ui.dontUseBlackSB->value());
	item->setDontUseWhite(ui.dontUseWhiteChB->isChecked());
	item->setDontUseWhiteValue(ui.dontUseWhiteSB->value());

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

void ItemWidget::maskCalcPB_clicked()
{
	Q_ASSERT(m_proc && m_imageWindowInterface);
	QImage image = m_imageWindowInterface->getCurrentSelectedImage();
	QRect rect = m_imageWindowInterface->getCurrentSelectedRect();
	if(image.isNull())
	{
		QMessageBox::warning(this, tr(""), tr("Выберите область на изображении"));
		return;
	}
	Param* param = new Param();
	param->setDontUseBlack(ui.dontUseBlackChB->isChecked());
	param->setDontUseBlackValue(ui.dontUseBlackSB->value());
	param->setDontUseWhite(ui.dontUseWhiteChB->isChecked());
	param->setDontUseWhiteValue(ui.dontUseWhiteSB->value());

	setParamObject(m_proc->calcParamObject(m_index, QSharedPointer<QObject>(param),
		image, rect));
	emit maskCreated(m_index, getMaskName());
}

void ItemWidget::maskCancelPB_clicked()
{
	Param param;
	ui.redRelativeDSB->setValue(param.getAvgRelativeRed());
	ui.greenRelativeDSB->setValue(param.getAvgRelativeGreen());
	ui.blueRelativeDSB->setValue(param.getAvgRelativeBlue());
	ui.redAbsSB->setValue(param.getAvgAbsoluteRed());
	ui.greenAbsSB->setValue(param.getAvgAbsoluteGreen());
	ui.blueAbsSB->setValue(param.getAvgAbsoluteBlue());
	//ui.redDeltaUpDSB->setValue(param.getDeltaPlusRelativeRed());
	//ui.greenDeltaUpDSB->setValue(param.getDeltaPlusRelativeGreen());
	//ui.blueDeltaUpDSB->setValue(param.getDeltaPlusRelativeBlue());
	//ui.redDeltaDownDSB->setValue(param.getDeltaMinusRelativeRed());
	//ui.greenDeltaDownDSB->setValue(param.getDeltaMinusRelativeGreen());
	//ui.blueDeltaDownDSB->setValue(param.getDeltaMinusRelativeBlue());
	//ui.rgbKMaskChB->setChecked(param.isKRgbUsed());
	//rgbKMaskChB_stateChanged(param.isKRgbUsed());

	emit maskRemoved(m_index);
}

void ItemWidget::maskAutoKPB_clicked()
{
	Param* item = qobject_cast<Param*>(m_param.data());
	if(item)
	{
		double deltaR = item->getAvgAbsoluteAutoDeltaRed();
		double deltaG = item->getAvgAbsoluteAutoDeltaGreen();
		double deltaB = item->getAvgAbsoluteAutoDeltaBlue();
		ui.redDeltaUpDSB->setValue(deltaR);
		ui.greenDeltaUpDSB->setValue(deltaG);
		ui.blueDeltaUpDSB->setValue(deltaB);
		ui.redDeltaDownDSB->setValue(deltaR);
		ui.greenDeltaDownDSB->setValue(deltaG);
		ui.blueDeltaDownDSB->setValue(deltaB);
	}
}

void ItemWidget::maskClearKPB_clicked()
{
	Param param;
	ui.rgbKMaskDSB->setValue(param.getKRgb());
	ui.redDeltaUpDSB->setValue(param.getDeltaPlusRelativeRed());
	ui.greenDeltaUpDSB->setValue(param.getDeltaPlusRelativeGreen());
	ui.blueDeltaUpDSB->setValue(param.getDeltaPlusRelativeBlue());
	ui.redDeltaDownDSB->setValue(param.getDeltaMinusRelativeRed());
	ui.greenDeltaDownDSB->setValue(param.getDeltaMinusRelativeGreen());
	ui.blueDeltaDownDSB->setValue(param.getDeltaMinusRelativeBlue());
	ui.rgbKMaskChB->setChecked(param.isKRgbUsed());
	rgbKMaskChB_stateChanged(param.isKRgbUsed());
}

void ItemWidget::rgbKMaskDSB_valueChanged(double value)
{
	ui.redDeltaUpDSB->setValue(value * m_upRDeltaValue);
	ui.redDeltaDownDSB->setValue(value * m_downRDeltaValue);
	ui.greenDeltaUpDSB->setValue(value * m_upGDeltaValue);
	ui.greenDeltaDownDSB->setValue(value * m_downGDeltaValue);
	ui.blueDeltaUpDSB->setValue(value * m_upBDeltaValue);
	ui.blueDeltaDownDSB->setValue(value * m_downBDeltaValue);
}

void ItemWidget::rgbKMaskChB_stateChanged(int state)
{
	ui.rgbKMaskL->setEnabled(state != Qt::Unchecked);
	ui.rgbKMaskDSB->setEnabled(state != Qt::Unchecked);
	ui.maskAutoKPB->setEnabled(state == Qt::Unchecked);
	if(state != Qt::Unchecked)
	{
		m_upRDeltaValue = ui.redDeltaUpDSB->value() / ui.rgbKMaskDSB->value();
		m_downRDeltaValue = ui.redDeltaDownDSB->value() / ui.rgbKMaskDSB->value();
		m_upGDeltaValue = ui.greenDeltaUpDSB->value() / ui.rgbKMaskDSB->value();
		m_downGDeltaValue = ui.greenDeltaDownDSB->value() / ui.rgbKMaskDSB->value();
		m_upBDeltaValue = ui.blueDeltaUpDSB->value() / ui.rgbKMaskDSB->value();
		m_downBDeltaValue = ui.blueDeltaDownDSB->value() / ui.rgbKMaskDSB->value();
		rgbKMaskDSB_valueChanged(ui.rgbKMaskDSB->value());
	}
}

void ItemWidget::dontUseBlackChB_stateChanged(int state)
{
	ui.dontUseBlackSB->setEnabled(state == Qt::Checked);
}

void ItemWidget::dontUseWhiteChB_stateChanged(int state)
{
	ui.dontUseWhiteSB->setEnabled(state == Qt::Checked);
}

void ItemWidget::setColor(const QColor& color)
{
	ui.maskViewWidget->setColor(color);
}

} // namespace ICEAVGCOLORPROC_NS
