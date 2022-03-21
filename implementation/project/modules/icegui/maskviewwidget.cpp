#include "maskviewwidget.h"

#include <QtGui/QColorDialog>

#include <iceodb/odb/maskview.hxx>

namespace ICEGUI_NS
{

MaskViewWidget::MaskViewWidget(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	ui.setupUi(this);
	setColor(Qt::red);
}

MaskViewWidget::~MaskViewWidget()
{

}

void MaskViewWidget::setMaskName(const QString& maskName)
{
	m_maskName = maskName;
}

void MaskViewWidget::setLabelText(const QString& text)
{
	ui.nameL->setText(text);
}

const std::tr1::shared_ptr<ICEODB_NS::MaskView>& MaskViewWidget::getMaskView()
{
	if(!m_maskView.get())
	{
		m_maskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(new ICEODB_NS::MaskView(
			m_maskName, getColor(), !ui.useChB->isChecked(),
			255 - ((double)ui.alphaSB->value()) / 100 * 255));
	}
	else
	{
		m_maskView->setColor(getColor());
		m_maskView->setUseImage(!ui.useChB->isChecked());
		m_maskView->setAlphaImageChannel(255 - ((double)ui.alphaSB->value()) / 100 * 255);
	}
	return m_maskView;
}

void MaskViewWidget::setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	m_maskView = maskView;
	if(m_maskView.get())
	{
		setColor(m_maskView->getColor());
		ui.useChB->setChecked(!m_maskView->isUseImage());
		useChB_stateChanged(!m_maskView->isUseImage());
		ui.alphaSB->setValue(100 - ((double)m_maskView->getAlphaImageChannel()) / 255 * 100);
	}
}

void MaskViewWidget::setColor(const QColor& color)
{
	QPalette palette = ui.colorFrame->palette();
	palette.setColor(ui.colorFrame->backgroundRole(), color);
	ui.colorFrame->setPalette(palette);
}

QColor MaskViewWidget::getColor() const
{
	QPalette palette = ui.colorFrame->palette();
	return palette.color(ui.colorFrame->backgroundRole());
}

void MaskViewWidget::setUseColorState(Qt::CheckState state)
{
	ui.useChB->setCheckState(state);
}

void MaskViewWidget::setAlpha(int alpha)
{
	ui.alphaSB->setValue(alpha);
}

void MaskViewWidget::setUseChBVisible(bool visible)
{
	ui.useChB->setVisible(visible);
}

void MaskViewWidget::setReadOnly(bool readOnly)
{
	ui.useChB->setEnabled(!readOnly);
	ui.colorPB->setVisible(!readOnly);
	ui.alphaL->setVisible(!readOnly);
	ui.alphaSB->setVisible(!readOnly);
}

void MaskViewWidget::colorPB_clicked()
{
	QPalette palette = ui.colorFrame->palette();
	QColorDialog dialog(palette.color(QPalette::Normal, ui.colorFrame->backgroundRole()),
		this);
	if(dialog.exec() == QDialog::Accepted)
	{
		setColor(dialog.currentColor());
		alphaSB_valueChanged(ui.alphaSB->value());
		emit colorChanged(getColor());
	}
}

void MaskViewWidget::useChB_stateChanged(int state)
{
	ui.colorFrame->setEnabled(state != Qt::Unchecked);
	ui.colorFrame->setAutoFillBackground(state != Qt::Unchecked);
	ui.colorPB->setEnabled(state != Qt::Unchecked);
}

void MaskViewWidget::alphaSB_valueChanged(int /*value*/)
{
	QColor color = getColor();
	color.setAlpha(255 - ((double)ui.alphaSB->value()) / 100 * 255);
	setColor(color);
	emit colorChanged(getColor());
}

} // namespace ICEGUI_NS
