#ifndef CURSORIMAGEINFOWIDGET_H
#define CURSORIMAGEINFOWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>
#include "ui_cursorimageinfowidget.h"

namespace ICEQWT_NS
{
	class ImageEditor;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT CursorImageInfoWidget: public QWidget
{
	Q_OBJECT
public:
	CursorImageInfoWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~CursorImageInfoWidget();

	void setImageEditor(const QString& name, const ICEQWT_NS::ImageEditor* imageEditor);
	void updateState();

protected slots:
	void onCursorPositionChanged();
	void onSelectedAreaChanged();

private:
	void fillFilterParamTreeWidget();

	Ui::CursorImageInfoWidget ui;	///< Ui класс

	const ICEQWT_NS::ImageEditor* m_imageEditor; //TODO refac
};

} // namespace ICEGUI_NS

#endif // CURSORIMAGEINFOWIDGET_H
