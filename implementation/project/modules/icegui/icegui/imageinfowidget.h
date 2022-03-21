#ifndef IMAGEINFOWIDGET_H
#define IMAGEINFOWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>
#include "ui_imageinfowidget.h"

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ImageInfoWidget: public QWidget
{
	Q_OBJECT
public:
	ImageInfoWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ImageInfoWidget();

	void setScaledType(ImageWidget::ScaledType scaledType);
	void setAspectRatioMode(Qt::AspectRatioMode aspectRatioMode);

	void setImage(int index, const QString& name, const QString& filename, const QImage& image);

	const QImage& getImage() const;
	const QString& getFilename() const;
	int getIndex() const;

protected:
	int m_index;
	QString m_filename;

private:
	Ui::ImageInfoWidget ui;	///< Ui класс
};

} // namespace ICEGUI_NS

#endif // IMAGEINFOWIDGET_H
