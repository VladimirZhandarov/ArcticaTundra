#ifndef OBJECTVIEWDIALOG_H
#define OBJECTVIEWDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_objectviewdialog.h"

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEQWT_NS
{
	class IceUnitRatioQwtTransform;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ObjectViewDialog : public QDialog
{
	Q_OBJECT
public:
	ObjectViewDialog(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder, 
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ObjectViewDialog();

	void updateState(ICEQWT_NS::IceUnitRatioQwtTransform* transformation, bool halfArcLength);

protected:
	virtual void closeEvent(QCloseEvent* event);

public slots:
	void show();

protected slots:
	void tabWidget_currentChanged(int index);
	void onColorChanged(const QColor& color);
	void onMouseDoubleClicked(); //TODO refac

private:
	Ui::ObjectViewDialog ui;	///< Ui класс

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
};

} // namespace ICEGUI_NS

#endif // OBJECTVIEWDIALOG_H
