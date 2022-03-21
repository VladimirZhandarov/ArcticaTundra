#ifndef MASKVIEWWIDGET_H
#define MASKVIEWWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>
#include "ui_maskviewwidget.h"

#include <boost/tr1/memory.hpp>

namespace ICEODB_NS
{
	class MaskView;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT MaskViewWidget : public QWidget
{
	Q_OBJECT
public:
	MaskViewWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~MaskViewWidget();

	void setMaskName(const QString& maskName);
	void setLabelText(const QString& text);

	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& getMaskView();
	void setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);

	void setColor(const QColor& color);
	QColor getColor() const;
	void setUseColorState(Qt::CheckState state);
	void setAlpha(int alpha);
	void setUseChBVisible(bool visible);
	void setReadOnly(bool readOnly);

signals:
	void colorChanged(const QColor& color);

protected slots:
	void colorPB_clicked();
	void useChB_stateChanged(int state);
	void alphaSB_valueChanged(int value);

private:
	Ui::MaskViewWidget ui;	///< Ui класс

	QString m_maskName;
	std::tr1::shared_ptr<ICEODB_NS::MaskView> m_maskView;
};

} // namespace ICEGUI_NS

#endif // MASKVIEWWIDGET_H
