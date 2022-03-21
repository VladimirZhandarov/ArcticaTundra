#ifndef ICETOOLSTLBR_H
#define ICETOOLSTLBR_H

#include <QtGui/QToolBar>

namespace ICEQWT_NS
{
	class ImageEditor;
}

class QAction;

class MainWindow;

class IceToolsTlBr : public QToolBar
{
	Q_OBJECT
public:
	IceToolsTlBr(const QString& title, MainWindow* mainWindow = 0);
	~IceToolsTlBr();

	void setInitial();
	void actualActions();

private slots:
	void navigationToolTriggered(QAction*);
	void pickerToolTriggered(QAction*);
	void onClearSelectedRect();
	void measureActTriggeredSlot();

private:
	ICEQWT_NS::ImageEditor* getImageEditor() const;

	class PrivateData;
	PrivateData* d_data;
};

#endif // ICETOOLSTLBR_H
