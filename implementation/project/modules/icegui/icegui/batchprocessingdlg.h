#ifndef BATCHPROCESSINGDLG_H
#define BATCHPROCESSINGDLG_H

#include "iceguidef.h"

#include <QtGui/QDialog>

#include <QtCore/QMap>

#include <iceimage/imagebuilder.h>

class QDir;

namespace ICECORE_NS
{
	class Settings;
	struct FilterSettings;
}

namespace ICEODB_NS
{
	class FilterParam;
}

namespace Ui
{
	class BatchProcessingDlg;
	class SetFilterWgt;
}

namespace ICEGUI_NS
{
	class SetFilterWidget;
	class MapViewDialog;

class ICEGUI_EXPORT BatchProcessingDlg : public QDialog
{
	Q_OBJECT
public:
	BatchProcessingDlg(ICECORE_NS::Settings* settings, QWidget* parent = 0,
		Qt::WindowFlags f = 0);

	~BatchProcessingDlg();

private Q_SLOTS:
	void sourceTlBtnSlot();
	void sourceGrpBxSlot(bool);
	void distTlBtnSlot();
	void distGrpBxSlot(bool);
	void addFilterTlBtnSlot();
	void delFilterTlBtnSlot();
	void useAllChBxSlot(bool);
	void defaultPBtnSlot();
	void beginPBtnSlot();
	void mapPBtnSlot();

private:
	void init();
	bool createDistDirs();
	bool setSourceDirs();
	bool checkOnValidates();
	void addSetFilterWidget(int indexAfter, SetFilterWidget* widget);
	void reconfFilterNum();
	void showResult();

	Ui::BatchProcessingDlg* ui;	///< Ui класс
	ICECORE_NS::Settings* m_settings;

	QVector<SetFilterWidget*> m_filterWidgetVector;
	MapViewDialog* m_mapViewDialog;
};

} // namespace ICEGUI_NS

#endif // BATCHPROCESSINGDLG_H