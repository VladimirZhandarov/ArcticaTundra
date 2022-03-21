#include "processingwidget.h"
#include "ui_processingwidget.h"

#include <iceodb/odb/filterparam.hxx>

namespace ICEGUI_NS
{

class ProcessingWidget::PrivateData
{
public:
	PrivateData() : isGroupFilter(false)
	{
		ui = new Ui::ProcessingWidget;
	}
	~PrivateData() 
	{
		delete ui;
	}

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam;			///< Фильтр
	int imageCount;														///< Общее кол-во обрабатываемых снимков
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> currentFilterParam;	///< Текущий фильтр
																		///< Если filterParam не групповой,
																		///< то равен filterParam
	bool isGroupFilter;
	Ui::ProcessingWidget* ui;
};

ProcessingWidget::ProcessingWidget(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
	QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	d_data = new PrivateData;
	d_data->ui->setupUi(this);

	setFilterCount(0);
	setImageCount(0);

	d_data->filterParam = filterParam;
	int subFiltersNum = filterParam->getChildParamCount();
	if(d_data->isGroupFilter = (0 != subFiltersNum))
	{
		setFilterCount(subFiltersNum);
	}
	else
	{
		setFilterCount(1); // Если это не групповой фильтр, то он сам является фильтром
		d_data->currentFilterParam = d_data->filterParam;
	}
}

ProcessingWidget::~ProcessingWidget()
{
	delete d_data;
}

/** Установка общего кол-ва подфильтров фильтра.
    Для не групповых фильтров filterCount должен быть 1
*/
void ProcessingWidget::setFilterCount(int filterCount)
{
	d_data->ui->filterPrBr->setRange(0, filterCount);
	d_data->ui->filterPrBr->setValue(0);
	d_data->ui->filterNameLbl->setText(QString::null);
}

void ProcessingWidget::onMaskProgressRangeChanged(int minimum, int maximum)
{
	d_data->ui->filtersSectionPrBr->setRange(minimum, maximum);
	d_data->ui->filtersSectionPrBr->repaint();
	d_data->ui->filtersSectionPrBr->update();
	QApplication::processEvents();
}

void ProcessingWidget::onMaskProgressValueChanged(int progressValue)
{
	d_data->ui->filtersSectionPrBr->setValue(progressValue);
	d_data->ui->filtersSectionPrBr->repaint();
	d_data->ui->filtersSectionPrBr->update();
	QApplication::processEvents();
}

void ProcessingWidget::onMaskProgressTextChanged(const QString& progressText)
{
	d_data->ui->filterSectionNameLbl->setText(progressText);
	d_data->ui->filtersSectionPrBr->repaint();
	d_data->ui->filtersSectionPrBr->update();
	QApplication::processEvents();
}

/** Установка текущего номера фильтра.
    Вызывать необходимо для любого типа фильтра
    Для не групповых фильтров filterNum должен быть 0
*/
void ProcessingWidget::onFilterCalculated(int filterNum, const QString& filterType)
{
	d_data->ui->filterPrBr->setValue(filterNum + 1);
	if(d_data->isGroupFilter)
	{
		d_data->currentFilterParam = d_data->filterParam->getChildParam(filterNum)->shared_from_this();
	}
	// Установка названия фильтра
	d_data->ui->filterNameLbl->setText(filterType + ": \"" +
		d_data->currentFilterParam->getName() + "\"");
	d_data->ui->filterPrBr->repaint();
	d_data->ui->filterPrBr->update();
	QApplication::processEvents();
}

/** Установка общего кол-ва обрабатываемых снимков.*/
void ProcessingWidget::setImageCount(int imageCount)
{
	// Нет смысла отображать прогресс-бар по кол-ву файлов, если общее кол-во файлов не больше 1
	bool state = imageCount > 1;
	d_data->ui->imageCountPrBr->setVisible(state);
	d_data->ui->imageNameLbl->setVisible(state);
	d_data->ui->imageCountLn->setVisible(state);

	d_data->ui->imageCountPrBr->setRange(0, imageCount);
	setImageNum(0);
}

/** Установка текущего номера обрабатываемого снимка.*/
void ProcessingWidget::setImageNum(int imageNum, const QString& imageName)
{
	d_data->ui->imageNameLbl->setText(imageName);
	d_data->ui->imageCountPrBr->setValue(imageNum);
}

} // namespace ICEGUI_NS
