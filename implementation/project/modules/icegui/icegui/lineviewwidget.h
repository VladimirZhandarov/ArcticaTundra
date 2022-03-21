#ifndef LINEVIEWWIDGET_H
#define LINEVIEWWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>
#include "ui_lineviewwidget.h"

#include <boost/tr1/memory.hpp>

namespace ICEODB_NS
{
	class LineView;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT LineViewWidget : public QWidget
{
	Q_OBJECT
public:
	LineViewWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~LineViewWidget();

	void setMaskName(const QString& maskName);
	void setLabelText(const QString& text);

	const std::tr1::shared_ptr<ICEODB_NS::LineView>& getLineView();
	void setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);

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
	Ui::LineViewWidget ui;	///< Ui класс

	QString m_maskName;
	std::tr1::shared_ptr<ICEODB_NS::LineView> m_lineView;
};

} // namespace ICEGUI_NS

#endif // LINEVIEWWIDGET_H
