#include "mapviewdialog.h"

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>

#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120

namespace ICEGUI_NS
{

MapViewDialog::MapViewDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f),
	m_mapLoaded(false)
{
	ui.setupUi(this);

	connect(ui.webView, SIGNAL(loadFinished(bool)), this, SLOT(webView_loadFinished(bool)));

	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
	ui.webView->setUrl(QUrl("qrc:/dvg/icegui/google_maps.html"));
}

MapViewDialog::~MapViewDialog()
{

}

void MapViewDialog::addPos(const QString& title, const QString& text, const QColor& markerColor,
	const QString& originalImageFilename, const QString& maskedImageFilename,
	double latitude, double longitude)
{
	m_mutex.lock();
	if(m_mapLoaded)
	{
		drawPos(title, text, markerColor, originalImageFilename, maskedImageFilename,
			latitude, longitude);
	}
	else
	{
		Data data;
		data.title = title;
		data.text = text;
		data.markerColor = markerColor;
		data.originalImageFilename = originalImageFilename;
		data.maskedImageFilename = maskedImageFilename;
		data.latitude = latitude;
		data.longitude = longitude;
		m_posList.append(data);
	}
	m_mutex.unlock();
}

void MapViewDialog::webView_loadFinished(bool /*ok*/)
{
	m_mutex.lock();
	m_mapLoaded = true;
	for(int i=0; i<m_posList.size(); i++)
	{
		Data data = m_posList[i];
		drawPos(data.title, data.text, data.markerColor, data.originalImageFilename,
			data.maskedImageFilename, data.latitude, data.longitude);
	}
	m_posList.clear();
	m_mutex.unlock();
}

void MapViewDialog::drawPos(const QString& title, const QString& text, const QColor& /*markerColor*/,
	const QString& originalImageFilename, const QString& maskedImageFilename,
	double latitude, double longitude)
{
	QString contentString = QString("<div style=\"overflow: auto; width: 350px; height: 315px;\">") +
		QString("<div class=\"\" style=\"overflow: auto;\">") +
		QString("<div class=\"infowindow\">") +
		QString("<h3>") + title + QString("</h3>") +
		QString("<p>") + text + QString("</p>") +
		QString("<img src=\"file:/" + originalImageFilename + "\" height=\"200\" width=\"300\"></img>") +
		QString("<br></br>") +
		QString("<img src=\"file:/" + maskedImageFilename + "\" height=\"200\" width=\"300\"></img>") +
		QString("<br></br>") +
		QString("</div>") +
		QString("</div>") +
		QString("</div>");

	QString str = QString("placeMarker(") + QString::number(latitude) + QString(", ") +
		QString::number(longitude) + QString(", '") + contentString + QString("')");
	ui.webView->page()->mainFrame()->evaluateJavaScript(str);
}

} // namespace ICEGUI_NS
