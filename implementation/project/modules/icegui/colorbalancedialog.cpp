#include "colorbalancedialog.h"

#include <iceimage/imagebuilder.h>

namespace ICEGUI_NS
{

ColorBalanceDialog::ColorBalanceDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_imageBuilder(imageBuilder)
{
	ui.setupUi(this);
	Q_ASSERT(!m_imageBuilder.isNull());
	m_startRedBalance = m_imageBuilder->getRedBalance();
	m_startGreenBalance = m_imageBuilder->getGreenBalance();
	m_startBlueBalance = m_imageBuilder->getBlueBalance();
	ui.redBalanceSlider->setValue(m_startRedBalance);
	ui.greenBalanceSlider->setValue(m_startGreenBalance);
	ui.blueBalanceSlider->setValue(m_startBlueBalance);
}

ColorBalanceDialog::~ColorBalanceDialog()
{

}

void ColorBalanceDialog::onRedBalanceChanged(int value)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setRedBalance(value);
}

void ColorBalanceDialog::onGreenBalanceChanged(int value)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setGreenBalance(value);
}

void ColorBalanceDialog::onBlueBalanceChanged(int value)
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setBlueBalance(value);
}

void ColorBalanceDialog::reject()
{
	Q_ASSERT(!m_imageBuilder.isNull());
	m_imageBuilder->setRedBalance(m_startRedBalance);
	m_imageBuilder->setGreenBalance(m_startGreenBalance);
	m_imageBuilder->setBlueBalance(m_startBlueBalance);
	QDialog::reject();
}

} // namespace ICEGUI_NS
