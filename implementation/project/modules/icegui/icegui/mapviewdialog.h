#ifndef MAPVIEWDIALOG_H
#define MAPVIEWDIALOG_H

#include <QtGui/QDialog>
#include "ui_mapviewdialog.h"

#include <QtCore/QMutex>

namespace ICEGUI_NS
{

class MapViewDialog : public QDialog
{
	Q_OBJECT
public:
	MapViewDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~MapViewDialog();

	void addPos(const QString& title, const QString& text, const QColor& markerColor,
		const QString& originalImageFilename, const QString& maskedImageFilename,
		double latitude, double longitude);

protected slots:
	void webView_loadFinished(bool ok);

private:
	void drawPos(const QString& title, const QString& text, const QColor& markerColor,
		const QString& originalImageFilename, const QString& maskedImageFilename,
		double latitude, double longitude);

	struct Data
	{
		QString title;
		QString text;
		QColor markerColor;
		QString originalImageFilename;
		QString maskedImageFilename;
		double latitude;
		double longitude;
	};

	Ui::MapViewDialog ui;
	bool m_mapLoaded;
	QList<Data> m_posList;
	QMutex m_mutex;
};

} // namespace ICEGUI_NS

#endif // MAPVIEWDIALOG_H
