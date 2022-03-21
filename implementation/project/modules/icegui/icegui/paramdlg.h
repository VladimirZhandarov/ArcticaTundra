#ifndef PARAMDLG_H
#define PARAMDLG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QTreeWidgetItem>

//#include "ui_paramdlg.h"

namespace ICEQWT_NS
{
	class ImageEditor;
	class IceQwtPlot;
	class IceQwtPlotGrid;
	class IceUnitRatioQwtTransform;
}

namespace ICECORE_NS
{
	struct ImageGridSettings;
	struct ImageLineSettings;
}

namespace Ui
{
	class ParamDlg;
}

class QwtInterval;

namespace ICEGUI_NS
{
class GridFrm;
class LineFrm;

/**
* @class ParamDlg
* Диалоговое окно настройки параметров программы
*/
class ICEGUI_EXPORT ParamDlg : public QDialog
{
	Q_OBJECT

	friend GridFrm;
	friend LineFrm;

public:

	enum SECTION
	{
		GUIDES,	///< Линейка
		GRID,	///< Сетка
		ADDED,	///< Дополнительно
		IMAGE	///< Изображение
	};

	/**
	* Конструктор
	* @param 
	* @param parent родительский объект
	* @param f флаги окна
	*/
	ParamDlg(ICEQWT_NS::ImageEditor* imageEditor, QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* Деструктор
	*/
	~ParamDlg();

	/**
	* Установить текущий раздел для отображения
	* @param section выбранный раздел
	*/
	bool setCurrentSection(SECTION section);

	GridFrm* gridFrm() const;

protected slots:
	void accept();
	void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void useColorCursorChB_toggled(bool checked);
	void colorCursorPB_clicked();

private:
	ICEQWT_NS::IceQwtPlot* plot() const;
	QWidget* findWidget(const QString& textItem) const;

	Ui::ParamDlg* ui;	///< Ui класс
	ICEQWT_NS::ImageEditor* imageEditor;
};

//=============================================================================
class GridFrm : public QFrame
{
	Q_OBJECT

public:
	GridFrm(QWidget* parent = 0, Qt::WindowFlags f = 0);
	void init(ParamDlg* paramDlg);
	void initData(const ICECORE_NS::ImageGridSettings& imageGridSettings);
	void initSlot();
	~GridFrm();

	void setUpdateUserUnterface(bool flag);

public slots:
	void onLineSlot(int);

private slots:
	void showGridSlot(int state);
	void hStepPixelValueChangedSlot(double);
	void vStepPixelValueChangedSlot(double);
	void hStepMetricValueChangedSlot(double);
	void vStepMetricValueChangedSlot(double);
	void colorGridPBtnSlot();
	void transparencySldrSlot(int);
	void useGridParamByDefaultCmBxCurrentIndexChangedSlot(int);
	void saveUserGridParamPBtnPressedSlot();
	void loadDefaultGridParamPBtnSlot();
	void loadUserGridParamPBtnSlot();

private:
	ICEQWT_NS::IceQwtPlotGrid* grid();
	void settingsChange();
	ICECORE_NS::ImageGridSettings getCurrentImageGridSettings();

	// ICEQWT_NS::IceQwtPlot* plot;
	ParamDlg* paramDlg;
	//QwtInterval* xInterval;
	//QwtInterval* yInterval;
	bool updateUserinterface;
	bool isUseUserParam;
};

//=============================================================================
class LineFrm : public QFrame
{
	Q_OBJECT
public:
	LineFrm(QWidget* parent = 0, Qt::WindowFlags f = 0);
	void init(ParamDlg* paramDlg);
	void initData(const ICECORE_NS::ImageLineSettings& imageLineSettings);
	void initSlot();
	~LineFrm();

private slots:
	void showLineSlot(int);
	void useDefaultUnitSlot(int);
	void unitCmBxCurrentIndexChangedSlot(int);
	void ratioDblSpBxSlot(double);
	void vOriginDblSpBxSlot(double);
	void hOriginDblSpBxSlot(double);
	void useLineParamByDefaultCmBxCurrentIndexChangedSlot(int);
	void saveUserLineParamPBtnPressedSlot();
	void loadDefaultLineParamPBtnSlot();
	void loadUserLineParamPBtnSlot();

private:
	void updateTransform(const ICEQWT_NS::IceUnitRatioQwtTransform* transform);
	void updateAxis();
	void settingsChange();
	ICECORE_NS::ImageLineSettings getCurrentImageLineSettings();

	ParamDlg* paramDlg;
	bool updateAxes;
	bool isUseUserParam;
};

} // namespace ICEGUI_NS

#endif // PARAMDLG_H
