#include "lineviewwidget.h"

#include <QtGui/QColorDialog>

#include <iceodb/odb/lineview.hxx>

namespace ICEGUI_NS
{

LineViewWidget::LineViewWidget(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	ui.setupUi(this);

	ui.alphaL->setVisible(false);
	ui.alphaSB->setVisible(false);

	setColor(Qt::red);
	for(int i=1; i<=4; i++)
	{
		ui.lineWidthCB->addItem(QString::number(i), i);
	}
}

LineViewWidget::~LineViewWidget()
{

}

void LineViewWidget::setMaskName(const QString& maskName)
{
	m_maskName = maskName;
}

void LineViewWidget::setLabelText(const QString& text)
{
	ui.nameL->setText(text);
}

const std::tr1::shared_ptr<ICEODB_NS::LineView>& LineViewWidget::getLineView()
{
	if(!m_lineView.get())
	{
		m_lineView = std::tr1::shared_ptr<ICEODB_NS::LineView>(new ICEODB_NS::LineView(
			m_maskName, getColor(), !ui.useChB->isChecked(),
			255 - ((double)ui.alphaSB->value()) / 100 * 255, QStringList(),
			ui.lineWidthCB->itemData(ui.lineWidthCB->currentIndex()).toInt()));
	}
	else
	{
		m_lineView->setColor(getColor());
		m_lineView->setUseImage(!ui.useChB->isChecked());
		m_lineView->setAlphaImageChannel(255 - ((double)ui.alphaSB->value()) / 100 * 255);
		m_lineView->setLineWidth(ui.lineWidthCB->itemData(ui.lineWidthCB->currentIndex()).toInt());
	}
	return m_lineView;
}

void LineViewWidget::setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView)
{
	m_lineView = lineView;
	if(m_lineView.get())
	{
		setColor(m_lineView->getColor());
		ui.useChB->setChecked(!m_lineView->isUseImage());
		useChB_stateChanged(!m_lineView->isUseImage());
		ui.alphaSB->setValue(100 - ((double)m_lineView->getAlphaImageChannel()) / 255 * 100);
		int lineWidthIndex = ui.lineWidthCB->findData(m_lineView->getLineWidth());
		if(lineWidthIndex < 0)
		{
			lineWidthIndex = 0;
		}
		ui.lineWidthCB->setCurrentIndex(lineWidthIndex);
	}
}

void LineViewWidget::setColor(const QColor& color)
{
	QPalette palette = ui.colorFrame->palette();
	palette.setColor(ui.colorFrame->backgroundRole(), color);
	ui.colorFrame->setPalette(palette);
}

QColor LineViewWidget::getColor() const
{
	QPalette palette = ui.colorFrame->palette();
	return palette.color(ui.colorFrame->backgroundRole());
}

void LineViewWidget::setUseColorState(Qt::CheckState state)
{
	ui.useChB->setCheckState(state);
}

void LineViewWidget::setAlpha(int alpha)
{
	ui.alphaSB->setValue(alpha);
}

void LineViewWidget::setUseChBVisible(bool visible)
{
	ui.useChB->setVisible(visible);
}

void LineViewWidget::setReadOnly(bool readOnly)
{
	ui.useChB->setEnabled(!readOnly);
	ui.colorPB->setVisible(!readOnly);
	ui.alphaSB->setReadOnly(readOnly);
	ui.lineWidthCB->setEditable(readOnly);
}

void LineViewWidget::colorPB_clicked()
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

void LineViewWidget::useChB_stateChanged(int state)
{
	bool flag = (state != Qt::Unchecked);
	ui.colorFrame->setEnabled(flag);
	ui.colorFrame->setAutoFillBackground(flag);
	ui.colorPB->setEnabled(flag);
	ui.lineWidthL->setEnabled(flag);
	ui.lineWidthCB->setEnabled(flag);
}

void LineViewWidget::alphaSB_valueChanged(int /*value*/)
{
	QColor color = getColor();
	color.setAlpha(255 - ((double)ui.alphaSB->value()) / 100 * 255);
	setColor(color);
	emit colorChanged(getColor());
}

} // namespace ICEGUI_NS
