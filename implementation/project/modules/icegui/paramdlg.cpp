#include "paramdlg.h"

#include "ui_paramdlg.h"

#include <iceqwt/imageeditor.h>
#include <iceqwt/iceqwtplot.h>
#include <iceqwt/iceqwtplotgrid.h>
#include <iceqwt/iceunitratioqwttransform.h>
#include <iceqwt/iceqwtplotrescaler.h>
#include <iceqwt/iceqwtplotrasteritem.h>

#include <icecore/core.h>
#include <icecore/settings.h>

#include <qwt_scale_engine.h>
#include <qwt_interval.h>
#include <qwt_scale_widget.h>

#include <QtGui/QColorDialog>

namespace ICEGUI_NS
{

ParamDlg::ParamDlg(ICEQWT_NS::ImageEditor* imageEditor, QWidget* parent, Qt::WindowFlags f) :
	QDialog(parent, f), imageEditor(imageEditor), ui(new Ui::ParamDlg)
{
	ui->setupUi(this);

	QList<int> sizes;
	sizes << 180 << 300;
	ui->splitter->setSizes(sizes);

	// Инициализируем окно в стеквиджете по настройке сетки
	// GridFrm1 объявлен в ui_paramdlg.h и является подклассом данного класса (ParamDlg)
	// Вот такая вот запутанная вещь
	ui->GridFrm1->init(this); 

	// Инициализируем окно в стеквиджете по настройке линейки
	// LineFrm объявлен в ui_paramdlg.h и является подклассом данного класса (ParamDlg)
	// Вот такая вот запутанная вещь
	// Настройки линеек надо делать после настроек сетки, так как в сетке должны быть
	// установлены единицы измерения, которые определяются в окне настроек линеек
	ui->LineFrm1->init(this);

	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
	ui->objectTableFileCreateAutoChB->setChecked(imageAddedSettings.objectTableFileCreateAuto);
	ui->htmlFileCreateAutoChB->setChecked(imageAddedSettings.htmlFileCreateAuto);
	ui->halfArcLengthChB->setChecked(imageAddedSettings.halfArcLength);
	ui->jpegQualitySB->setValue(imageAddedSettings.jpegQuality);
	ui->useColorCursorChB->setChecked(imageAddedSettings.useColorCursor);
	QFrame* usedFrame = ui->colorCursorFrame;
	QPalette palette = usedFrame->palette();
	palette.setColor(usedFrame->backgroundRole(), QColor(imageAddedSettings.colorCursorR,
		imageAddedSettings.colorCursorG, imageAddedSettings.colorCursorB,
		imageAddedSettings.colorCursorA));
	usedFrame->setPalette(palette);
}

ParamDlg::~ParamDlg()
{
	ui->LineFrm1->~LineFrm();
	ui->GridFrm1->~GridFrm();

	delete ui;
	ui = 0;
}

GridFrm* ParamDlg::gridFrm() const
{
	return ui->GridFrm1;
}

// Установить текущую отображаемую секцию
// Как правило вызывается пользователем при создании данного окна
bool ParamDlg::setCurrentSection(SECTION section)
{
	QList<QTreeWidgetItem*> findItem;

	switch(section)
	{
		case IMAGE:
		{
			// Qt::MatchRecursive чтобы при поиске дерево рекурсивно проходило, иначе поиск
			// будет идти только по элементам первого уровня
			findItem = ui->treeWgt->findItems(tr("Изображение"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
		case GRID:
		{
			findItem = ui->treeWgt->findItems(tr("Сетка"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
		case GUIDES:
		{
			findItem = ui->treeWgt->findItems(tr("Линейка"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
		case ADDED:
		{
			findItem = ui->treeWgt->findItems(tr("Дополнительно"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
	}

	// Поиск должен либо найти единственный элемент, либо ничего.
	// У нас же нет в дереве два элемента с одним и тем же текстом
	Q_ASSERT(findItem.count() <= 1);
	if(!findItem.count())
	{
		return false;
	}
	ui->treeWgt->setCurrentItem(findItem[0]);
	return true;
}

void ParamDlg::accept()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageAddedSettings imageAddedSettings;
	imageAddedSettings.objectTableFileCreateAuto = ui->objectTableFileCreateAutoChB->isChecked();
	imageAddedSettings.htmlFileCreateAuto = ui->htmlFileCreateAutoChB->isChecked();
	imageAddedSettings.halfArcLength = ui->halfArcLengthChB->isChecked();
	imageAddedSettings.jpegQuality = ui->jpegQualitySB->value();
	imageAddedSettings.useColorCursor = ui->useColorCursorChB->isChecked();

	QFrame* usedFrame = ui->colorCursorFrame;
	Q_ASSERT(usedFrame);
	QPalette palette = usedFrame->palette();
	QColor colorCursor = palette.color(QPalette::Normal, usedFrame->backgroundRole());
	imageAddedSettings.colorCursorR = colorCursor.red();
	imageAddedSettings.colorCursorG = colorCursor.green();
	imageAddedSettings.colorCursorB = colorCursor.blue();
	imageAddedSettings.colorCursorA = colorCursor.alpha();
	settings->setImageAddedSettings(imageAddedSettings);
	QDialog::accept();
}

void ParamDlg::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/)
{
	if(current->text(0) == tr("Линейка"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("LineFrm"));
	}
	else if(current->text(0) == tr("Сетка"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("GridFrm"));
	}
	else if(current->text(0) == tr("Дополнительно"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("AddedFrm"));
	}
	else if(current->text(0) == tr("Изображение"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("ImageFrm"));
	}
}

void ParamDlg::useColorCursorChB_toggled(bool checked)
{
	ui->colorCursorL->setEnabled(checked);
	ui->colorCursorFrame->setEnabled(checked);
	ui->colorCursorPB->setEnabled(checked);
	Q_ASSERT(imageEditor);
	ICEQWT_NS::IceQwtPlot* plot = imageEditor->icePlot();
	Q_ASSERT(plot);
	if(checked)
	{
		QFrame* usedFrame = ui->colorCursorFrame;
		Q_ASSERT(usedFrame);
		QPalette palette = usedFrame->palette();
		plot->setCursorColor(palette.color(QPalette::Normal, usedFrame->backgroundRole()));
	}
	else
	{
		plot->setCursorColor();
	}
}

void ParamDlg::colorCursorPB_clicked()
{
	QFrame* usedFrame = ui->colorCursorFrame;
	Q_ASSERT(usedFrame);
	QPalette palette = usedFrame->palette();
	QColorDialog dialog(palette.color(QPalette::Normal, usedFrame->backgroundRole()), this);
	dialog.setOption(QColorDialog::ShowAlphaChannel);
	if(dialog.exec() == QDialog::Accepted)
	{
		palette.setColor(usedFrame->backgroundRole(), dialog.currentColor());
		usedFrame->setPalette(palette);
		Q_ASSERT(imageEditor);
		ICEQWT_NS::IceQwtPlot* plot = imageEditor->icePlot();
		Q_ASSERT(plot);
		plot->setCursorColor(palette.color(QPalette::Normal, usedFrame->backgroundRole()));
	}
}

ICEQWT_NS::IceQwtPlot* ParamDlg::plot() const
{
	return imageEditor->icePlot();
}

QWidget* ParamDlg::findWidget(const QString& objectName) const
{
	for(int i=0; i<ui->stackedWgt->count(); i++)
	{
		if(ui->stackedWgt->widget(i)->objectName() == objectName)
		{
			return ui->stackedWgt->widget(i);
		}
	}
	return 0;
}

//=============================================================================
LineFrm::LineFrm(QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f)
{
	updateAxes = true;
}

LineFrm::~LineFrm()
{
	if(!paramDlg->ui)
	{
		return;
	}
	// Запоминаем настройки 
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	imageLineSettings.isUseUserParam = isUseUserParam;
	settings->setImageLineSettings(imageLineSettings);
}

void LineFrm::init(ParamDlg* paramDlg)
{
	this->paramDlg = paramDlg;

	//REFAC очень запутано
	// Такой же выбор как и в IceQwtPlot::setupLine
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	if(!imageLineSettings.isUseUserParam)
	{
		imageLineSettings = settings->getImageLineSettingsDefault();
	}
	initData(imageLineSettings);
	initSlot();
}

void LineFrm::initSlot()
{
	// Установить единицы измерения
	connect(paramDlg->ui->unitCmBx, SIGNAL(currentIndexChanged(int)), this, SLOT(unitCmBxCurrentIndexChangedSlot(int)));
	// Показать/скрыть линейку
	connect(paramDlg->ui->showLineChkBx, SIGNAL(stateChanged(int)), this, SLOT(showLineSlot(int)));
	// Всегда использовать по умолчанию
	connect(paramDlg->ui->useDefaultUnitChBox, SIGNAL(stateChanged(int)), this, SLOT(useDefaultUnitSlot(int)));
	// Установить коэффициент масштаба на местности
	connect(paramDlg->ui->ratioDblSpBx, SIGNAL(valueChanged(double)), this, SLOT(ratioDblSpBxSlot(double)));
	// Установить начало координат
	connect(paramDlg->ui->vOriginDblSpBx, SIGNAL(valueChanged(double)), this, SLOT(vOriginDblSpBxSlot(double)));
	// Установить вид настроек линейки, которые должны загружаться при первой загрузке изображения
	connect(paramDlg->ui->useLineParamByDefaultCmBx, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(useLineParamByDefaultCmBxCurrentIndexChangedSlot(int)));
	// Сохранить текущие настройки линейки как Пользовательские
	connect(paramDlg->ui->saveUserLineParamPBtn, SIGNAL(pressed()), this, SLOT(saveUserLineParamPBtnPressedSlot()));
	// Загрузить настройки линейки По умолчанию
	connect(paramDlg->ui->loadDefaultLineParamPBtn, SIGNAL(pressed()), this, SLOT(loadDefaultLineParamPBtnSlot()));
	// Загрузить настройки линейки Пользовательские
	connect(paramDlg->ui->loadUserLineParamPBtn, SIGNAL(pressed()), this, SLOT(loadUserLineParamPBtnSlot()));
}

void LineFrm::initData(const ICECORE_NS::ImageLineSettings& imageLineSettings)
{
	blockSignals(true);

	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();

	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();

	// Установить единицы измерения
	QComboBox* &unitCmBx = paramDlg->ui->unitCmBx;
	ICEIMAGE_NS::UNIT unit = transform->unit();
	switch(unit)
	{
		case ICEIMAGE_NS::NONE: // Пиксели
		{
			//TODO vvzh магические цмфры 
			unitCmBx->setCurrentIndex(0);
			break;
		}
		case ICEIMAGE_NS::M:
		{
			unitCmBx->setCurrentIndex(1);
			break;
		}
		case ICEIMAGE_NS::KM: default:
		{
			unitCmBx->setCurrentIndex(2);
			break;
		}
	};

	// Отображаем единицу измерения для коэффициента масштаба на местности
	if(ICEIMAGE_NS::NONE != unit)
	{
		paramDlg->ui->KUnitlbl->setText(unitCmBx->currentText());
		// Отображаем единицу измерения для сетки
		//paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->currentText());
		//paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->currentText());
	}
	else 
	{
		ICEIMAGE_NS::UNIT prevUnit = transform->metricUnit();
		if(ICEIMAGE_NS::M == prevUnit)
		{
			paramDlg->ui->KUnitlbl->setText(unitCmBx->itemText(1));
			// Отображаем единицу измерения для сетки
			//paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->itemText(1));
			//paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->itemText(1));
		}
		else
		{
			paramDlg->ui->KUnitlbl->setText(unitCmBx->itemText(2));
			// Отображаем единицу измерения для сетки
			//paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->itemText(2));
			//paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->itemText(2));
		}
	}
	// Отображаем единицу измерения для смещения начало координат
	//paramDlg->ui->hUnitBeginCoordTxt->setText(unitCmBx->currentText());
	//paramDlg->ui->vUnitBeginCoordTxt->setText(unitCmBx->currentText());

	// Показать/скрыть линейку
	QCheckBox* &showLineChkBx = paramDlg->ui->showLineChkBx;
	showLineChkBx->setChecked(imageLineSettings.isShow);

	// Установить коэффициент масштаба на местности
	QDoubleSpinBox* &ratioDblSpBx = paramDlg->ui->ratioDblSpBx;
	ratioDblSpBx->setValue(1.0 / transform->metricRatio()); // Всегда отбражаем только коэффициент метрической системы
	if(ICEIMAGE_NS::NONE == unit)
	{
		ratioDblSpBx->setEnabled(false);
	}

	// Установить начало координат
	QDoubleSpinBox* &hOriginDblSpBx = paramDlg->ui->hOriginDblSpBx;
	hOriginDblSpBx->setValue(plot->rasterItem()->origin().x()); 
	connect(hOriginDblSpBx, SIGNAL(valueChanged(double)), this, SLOT(hOriginDblSpBxSlot(double)));
	QDoubleSpinBox* &vOriginDblSpBx = paramDlg->ui->vOriginDblSpBx;
	vOriginDblSpBx->setValue(plot->rasterItem()->origin().y()); 

	// Установить флаг для единиц измерения "Всегда использовать по умолчанию"
	//ICECORE_NS::Settings* settings = plot->getSettings();
	//ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	//if(!imageLineSettings.isUseUserParam)
	//{
	//	imageLineSettings = settings->getImageLineSettingsDefault();
	//}
	paramDlg->ui->useDefaultUnitChBox->setChecked(imageLineSettings.isUseAlwaysUnit);

	// Установить флаг "Использовать пользовательские параметры по умолчанию"
	// paramDlg->ui->useDefaultAllLineParamsChBx->setChecked(imageLineSettings.isUseUserParam);

	blockSignals(false);

	// Установить вид настроек линейки, которые должны загружаться при первой загрузке изображения
	QComboBox* &useLineParamByDefaultCmBx = paramDlg->ui->useLineParamByDefaultCmBx;
	isUseUserParam = imageLineSettings.isUseUserParam;
	if(!isUseUserParam)
	{
		useLineParamByDefaultCmBx->setCurrentIndex(0); // По умолчанию
	}
	else
	{
		useLineParamByDefaultCmBx->setCurrentIndex(1); // Пользовательские
	}
	// Подчеркиваем текущий выбранный начальный набор настроек
	QFont font = paramDlg->ui->loadUserLineParamPBtn->font();
	font.setUnderline(isUseUserParam);
	paramDlg->ui->loadUserLineParamPBtn->setFont(font);
	font.setUnderline(!isUseUserParam);
	paramDlg->ui->loadDefaultLineParamPBtn->setFont(font);

	// Приводим пользовательский интерфейс в актуальное состояние
	unitCmBxCurrentIndexChangedSlot(unitCmBx->currentIndex());
}

// Показать скрыть линейки
void LineFrm::showLineSlot(int state)
{
	switch(state)
	{
		case Qt::Unchecked :
		{
			Q_ASSERT(paramDlg);
			paramDlg->imageEditor->icePlot()->setAxisVisible(false);
			break;
		}
		case Qt::Checked :
		{
			Q_ASSERT(paramDlg);
			paramDlg->imageEditor->icePlot()->setAxisVisible(true);
			break;
		}
	};
	settingsChange();
}

// Всегда использовать по умолчанию
void LineFrm::useDefaultUnitSlot(int)
{
	settingsChange();
}

// Привести пользовательский интерфейс в актуальное состояние в зависисмости от
// текущих настроек
void LineFrm::settingsChange()
{
	Q_ASSERT(paramDlg);
	// Берем набор текущих настроек
	ICECORE_NS::ImageLineSettings currentImageLineSettings = getCurrentImageLineSettings();
	// Берем набор начальных настроек по умолчанию
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettingsDefault = settings->getImageLineSettingsDefault();
	// Берем набор начальных пользовательских настроек
	ICECORE_NS::ImageLineSettings imageLineSettingsUser = settings->getImageLineSettings();

	// Если набор настроек совпадает с начальными настройками по умолчанию
	paramDlg->ui->loadDefaultLineParamPBtn->setEnabled(!(currentImageLineSettings == imageLineSettingsDefault));

	// Если набор настроек совпадает с начальными пользовательскими настройками
	paramDlg->ui->loadUserLineParamPBtn->setEnabled(!(currentImageLineSettings == imageLineSettingsUser));
	paramDlg->ui->saveUserLineParamPBtn->setEnabled(paramDlg->ui->loadUserLineParamPBtn->isEnabled());
}

// Получить текущие настройки
ICECORE_NS::ImageLineSettings LineFrm::getCurrentImageLineSettings()
{
	ICECORE_NS::ImageLineSettings imageLineSettings;
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	imageLineSettings.origin = plot->rasterItem()->origin();

	// Уповаем на то, что для оси Х и Y параметры трансформации один и те же
	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();
	imageLineSettings.ratio = transform->ratio();
	imageLineSettings.unit = transform->unit();
	imageLineSettings.isShow = paramDlg->ui->showLineChkBx->isChecked();

	imageLineSettings.isUseAlwaysUnit = paramDlg->ui->useDefaultUnitChBox->isChecked();
	imageLineSettings.isUseUserParam = isUseUserParam;
	return imageLineSettings;
}

// Изменить вид настроек линейки, используемые при загрузке изображения
void LineFrm::useLineParamByDefaultCmBxCurrentIndexChangedSlot(int index)
{
	switch(index)
	{
		case 0 :
		{
			isUseUserParam = false;
			break;
		}
		case 1:
		default:
		{
			isUseUserParam = true;
			break;
		}
	}
	settingsChange();
}

// Сохранить текущие настройки линейки как Пользовательские
void LineFrm::saveUserLineParamPBtnPressedSlot()
{
	ICECORE_NS::ImageLineSettings imageLineSettings = getCurrentImageLineSettings();
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	settings->setImageLineSettings(imageLineSettings);
	settingsChange();
}

// Загрузить настройки линейки По умолчанию
void LineFrm::loadDefaultLineParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettingsDefault = settings->getImageLineSettingsDefault();
	paramDlg->plot()->setupLine(imageLineSettingsDefault);
	initData(imageLineSettingsDefault);

	// Подчеркиваем текущий выбранный начальный набор настроек
	QFont font = paramDlg->ui->loadDefaultLineParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadDefaultLineParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadUserLineParamPBtn->setFont(font);
	//settingsChange();
}

// Загрузить настройки линейки Пользовательские
void LineFrm::loadUserLineParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	paramDlg->plot()->setupLine(imageLineSettings);
	initData(imageLineSettings);

	// Подчеркиваем текущий выбранный начальный набор настроек
	QFont font = paramDlg->ui->loadUserLineParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadUserLineParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadDefaultLineParamPBtn->setFont(font);
	//settingsChange();
}

// Обновить трансформацию
void LineFrm::updateTransform(const ICEQWT_NS::IceUnitRatioQwtTransform* transform)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();

	// Обновление трансформации говорит о том, что поменялись единицы измерений. Для того,
	// чтобы plot->rescaler()->rescale() он должен работать с одинаковыми размерностями осей
	// как до изменения размера так и после (это касается oldInterval). После установки новой
	// трансформации оси до изменения размера будут в других единицах измерения - поэтому здесь
	// мы их приводим к нужной единице измерения
	QwtScaleDiv scaleDivX = plot->axisScaleDiv(QwtPlot::xTop);
	QwtInterval intervalBaseImageX = plot->transform(QwtPlot::xTop, scaleDivX.interval());
	intervalBaseImageX.setMinValue(transform->invTransform(intervalBaseImageX.minValue()));
	intervalBaseImageX.setMaxValue(transform->invTransform(intervalBaseImageX.maxValue()));
	scaleDivX.setInterval(intervalBaseImageX);
	plot->setAxisScaleDiv(QwtPlot::xTop, scaleDivX);

	QwtScaleDiv scaleDivY = plot->axisScaleDiv(QwtPlot::yLeft);
	QwtInterval intervalBaseImageY = plot->transform(QwtPlot::yLeft, scaleDivY.interval());
	intervalBaseImageY.setMinValue(transform->invTransform(intervalBaseImageY.minValue()));
	intervalBaseImageY.setMaxValue(transform->invTransform(intervalBaseImageY.maxValue()));
	scaleDivY.setInterval(intervalBaseImageY);
	plot->setAxisScaleDiv(QwtPlot::yLeft, scaleDivY);

	ICEQWT_NS::IceUnitRatioQwtTransform* transformNewX = (ICEQWT_NS::IceUnitRatioQwtTransform*)transform->copy();
	ICEQWT_NS::IceUnitRatioQwtTransform* transformNewY = (ICEQWT_NS::IceUnitRatioQwtTransform*)transform->copy();
	plot->axisScaleEngine(QwtPlot::xTop)->setTransformation(transformNewX);
	plot->axisScaleEngine(QwtPlot::yLeft)->setTransformation(transformNewY);
}

// Обновить оси
void LineFrm::updateAxis()
{
	if(!updateAxes)
	{
		return;
	}
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	Q_ASSERT(plot);
	plot->rescale();
	settingsChange();
}

// Установить коэффициент масштаба на местности
void LineFrm::ratioDblSpBxSlot(double value)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	Q_ASSERT(plot);
	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();

	transform->setRatio(1.0 / value);
	updateTransform(transform);
	updateAxis(); 
	plot->replot();
}

// Установить начало координат по оси Х
void LineFrm::hOriginDblSpBxSlot(double value)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	QPointF origin = plot->rasterItem()->origin();
	origin.setX(value);
	plot->rasterItem()->setOrigin(origin); 
	updateAxis();
}

// Установить начало координат по оси Y
void LineFrm::vOriginDblSpBxSlot(double value)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	QPointF origin = plot->rasterItem()->origin();
	origin.setY(value);
	plot->rasterItem()->setOrigin(origin); 
	updateAxis();
}

// Установить единицы измерения
void LineFrm::unitCmBxCurrentIndexChangedSlot(int /*index*/)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();

	// Коэффициент масштаба на местности
	QDoubleSpinBox* &ratioDblSpBx = paramDlg->ui->ratioDblSpBx;
	// Шаг сетки по горизонтали и вертикали в пикселях
	QDoubleSpinBox* &hStepGridPixelDblSpBx = paramDlg->ui->hStepGridPixelDblSpBx;
	QDoubleSpinBox* &vStepGridPixelDblSpBx = paramDlg->ui->vStepGridPixelDblSpBx;
	// Шаг сетки по горизонтали и вертикали в метрической системе
	QDoubleSpinBox* &hStepGridMetricDblSpBx = paramDlg->ui->hStepGridMetricDblSpBx;
	QDoubleSpinBox* &vStepGridMetricDblSpBx = paramDlg->ui->vStepGridMetricDblSpBx;
	// Единица измерения коэффициента масштаба на местности
	QLabel* &KUnitlbl = paramDlg->ui->KUnitlbl;

	QComboBox* &unitCmBx = paramDlg->ui->unitCmBx;

	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();

	// Смещение преобразовываем в пиксели изображения
	QPointF origin = transform->transform(plot->rasterItem()->origin());
	//// Шаг сетки преобразовываем в пиксели изображения
	//double hStepMetric = transform->transform(paramDlg->ui->hStepGridMetricDblSpBx->value());
	//double vStepMetric = transform->transform(paramDlg->ui->vStepGridMetricDblSpBx->value());

	switch(unitCmBx->currentIndex())
	{
		case 0: // Пиксели
		{
			transform->setUnit(ICEIMAGE_NS::NONE);
			ratioDblSpBx->setEnabled(false);
			hStepGridMetricDblSpBx->setEnabled(false);
			vStepGridMetricDblSpBx->setEnabled(false);
			break;
		}
		case 1: // Метры
		{
			transform->setUnit(ICEIMAGE_NS::M);
			ratioDblSpBx->blockSignals(true);
			ratioDblSpBx->setValue(1.0 / transform->ratio());
			ratioDblSpBx->blockSignals(false);
			ratioDblSpBx->setEnabled(true);
			KUnitlbl->setText(unitCmBx->currentText());
			break;
		}
		case 2: default: // Километры
		{
			transform->setUnit(ICEIMAGE_NS::KM);
			ratioDblSpBx->blockSignals(true);
			ratioDblSpBx->setValue(1.0 / transform->ratio());
			ratioDblSpBx->blockSignals(false);
			ratioDblSpBx->setEnabled(true);
			KUnitlbl->setText(unitCmBx->currentText());
			break;
		}
	}

	// А теперь смещение преобразовываем к новой установленной единице измерения
	origin = transform->invTransform(origin);
	// Для смещения начала координат также устанавливаем измененные единицы
	// измерения
	paramDlg->ui->hUnitBeginCoordTxt->setText(unitCmBx->currentText());
	paramDlg->ui->vUnitBeginCoordTxt->setText(unitCmBx->currentText());
	updateAxes = false;
	paramDlg->ui->vOriginDblSpBx->setValue(origin.x());
	paramDlg->ui->hOriginDblSpBx->setValue(origin.y());
	updateAxes = true;

	updateTransform(transform);
	updateAxis();

	// Преобразовываем к новой установленной единице и шаг сетки в метрической системе
	switch(unitCmBx->currentIndex())
	{
		case 0:
		{
			hStepGridPixelDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked()); // Включать надо, только если "не по линейке" (3)
			vStepGridPixelDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked());
			break;
		}
		case 1:
		case 2:
		default: // Метрическая система
		{
			double mnj;
			if(unitCmBx->currentText() != paramDlg->ui->vUnitMetricGridTxt->text())
			{
				if(unitCmBx->currentIndex() == 1) // Из КМ в М
				{
					mnj = 1000;
				}
				else
				{
					mnj = 0.001;
				}
				paramDlg->gridFrm()->setUpdateUserUnterface(false);
				paramDlg->ui->hStepGridMetricDblSpBx->setValue(paramDlg->ui->hStepGridMetricDblSpBx->value() * mnj);
				paramDlg->ui->vStepGridMetricDblSpBx->setValue(paramDlg->ui->vStepGridMetricDblSpBx->value() * mnj);
				paramDlg->gridFrm()->setUpdateUserUnterface(true);
			}

			hStepGridPixelDblSpBx->setEnabled(false);
			vStepGridPixelDblSpBx->setEnabled(false);
			hStepGridMetricDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked());  // см. (3)
			vStepGridMetricDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked());  // см. (3)
			paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->currentText());
			paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->currentText());
			break;
		}
	}
	// Адаптируем интерфейс вкладки "Сетка"
	paramDlg->gridFrm()->onLineSlot(paramDlg->ui->onLineChkBx->checkState());
}

GridFrm::GridFrm(QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f)
{
	updateUserinterface = true;
}

GridFrm::~GridFrm()
{
	if(!paramDlg->ui)
	{
		return;
	}

	// Запоминаем настройки 
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->plot();
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();
	imageGridSettings.isUseUserParam = isUseUserParam;
	settings->setImageGridSettings(imageGridSettings);
}

void GridFrm::setUpdateUserUnterface(bool flag)
{
	updateUserinterface = flag;
}

ICEQWT_NS::IceQwtPlotGrid* GridFrm::grid()
{
	return paramDlg->imageEditor->icePlot()->grid();
}

void GridFrm::initSlot()
{
	this->paramDlg = paramDlg;

	// Показать/скрыть сетку
	connect(paramDlg->ui->showGridChkBx, SIGNAL(stateChanged(int)), this, SLOT(showGridSlot(int)));
	// Установить/снять привязку сетки "По линейке"
	connect(paramDlg->ui->onLineChkBx, SIGNAL(stateChanged(int)), this, SLOT(onLineSlot(int)));
	// Установить шаг сетки в пикселях по горизонтали в пикселях 
	connect(paramDlg->ui->hStepGridPixelDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(hStepPixelValueChangedSlot(double)));
	// Установить шаг сетки в пикселях по вертикали в пикселях
	connect(paramDlg->ui->vStepGridPixelDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(vStepPixelValueChangedSlot(double)));
	// Установить шаг сетки в метрической системе по горизонтали 
	connect(paramDlg->ui->hStepGridMetricDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(hStepMetricValueChangedSlot(double)));
	// Установить шаг сетки в метрической системе по вертикали 
	connect(paramDlg->ui->vStepGridMetricDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(vStepMetricValueChangedSlot(double)));
	// Выбор цвета линии сетки
	connect(paramDlg->ui->colorPenGridPBtn, SIGNAL(clicked()), this, SLOT(colorGridPBtnSlot()));
	// Установка прозрачности сетки
	connect(paramDlg->ui->transparencySldr, SIGNAL(valueChanged(int)), this, SLOT(transparencySldrSlot(int)));
	// Установить вид настроек сетки, которые должны загружаться при первой загрузке изображения
	connect(paramDlg->ui->useGridParamByDefaultCmBx, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(useGridParamByDefaultCmBxCurrentIndexChangedSlot(int)));
	// Сохранить текущие настройки сетки как Пользовательские
	connect(paramDlg->ui->saveUserGridParamPBtn, SIGNAL(pressed()), this, SLOT(saveUserGridParamPBtnPressedSlot()));
	// Загрузить настройки сетки По умолчанию
	connect(paramDlg->ui->loadDefaultGridParamPBtn, SIGNAL(pressed()), this, SLOT(loadDefaultGridParamPBtnSlot()));
	// Загрузить настройки сетки Пользовательские
	connect(paramDlg->ui->loadUserGridParamPBtn, SIGNAL(pressed()), this, SLOT(loadUserGridParamPBtnSlot()));
}

void GridFrm::initData(const ICECORE_NS::ImageGridSettings& imageGridSettings)
{
	blockSignals(true);

	ICEQWT_NS::IceQwtPlotGrid* grid = this->grid();

	// Показать/скрыть сетку
	QCheckBox* &showGridChkBx = paramDlg->ui->showGridChkBx;
	showGridChkBx->setChecked(((ICEQWT_NS::IceQwtPlot*)(grid->plot()))->gridVisible());

	// Установить/снять привязку сетки "По линейке"
	QCheckBox* &onLineChkBx = paramDlg->ui->onLineChkBx;
	onLineChkBx->setChecked(grid->isOnLine());

	// Установить шаг сетки в пикселях по горизонтали 
	QDoubleSpinBox* &hStepGridPixelDblSpBx = paramDlg->ui->hStepGridPixelDblSpBx;
	hStepGridPixelDblSpBx->setValue(grid->hStepPixel());

	// Установить шаг сетки в пикселях по вертикали 
	QDoubleSpinBox* &vStepGridPixelDblSpBx = paramDlg->ui->vStepGridPixelDblSpBx;
	vStepGridPixelDblSpBx->setValue(grid->vStepPixel());

	// Установить шаг сетки в метрической системе по горизонтали 
	QDoubleSpinBox* &hStepGridMetricDblSpBx = paramDlg->ui->hStepGridMetricDblSpBx;
	hStepGridMetricDblSpBx->setValue(grid->hStepMetric());

	// Установить шаг сетки в метрической системе по вертикали 
	QDoubleSpinBox* &vStepGridMetricDblSpBx = paramDlg->ui->vStepGridMetricDblSpBx;
	vStepGridMetricDblSpBx->setValue(grid->vStepMetric());

	// Установить/снять Показать сетку пикселей с масштабом 800% и выше
	QCheckBox* &showGrid800ChBx = paramDlg->ui->showGrid800ChBx;
	showGrid800ChBx->setChecked(grid->isShow800());

	// Выбор цвета линии сетки
	// Устанавливаем текущий цвет сетки в индикатор цвета сетки
	QFrame* &colorPenGridClrFrm = paramDlg->ui->colorPenGridClrFrm;
	QPalette palette = colorPenGridClrFrm->palette();
	QPen pen = grid->majorPen();
	palette.setColor(colorPenGridClrFrm->backgroundRole(), pen.color());
	colorPenGridClrFrm->setPalette(palette);

	// Установка прозрачности сетки
	QSlider* &transparencySldr = paramDlg->ui->transparencySldr;
	transparencySldr->setValue(100 - grid->majorPen().color().alpha() / 255.0 * 100);

	blockSignals(false);

	// Установить вид настроек сетки, которые должны загружаться при первой загрузке изображения
	// Получаем пользовательские параметры сетки
	//ICECORE_NS::Settings* settings = paramDlg->plot()->getSettings(); 
	//ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();

	// Устанавливаем, какие настройки сетки использовать при загрузке
	QComboBox* &useGridParamByDefaultCmBx = paramDlg->ui->useGridParamByDefaultCmBx;
	isUseUserParam = imageGridSettings.isUseUserParam;
	if(!isUseUserParam)
	{
		useGridParamByDefaultCmBx->setCurrentIndex(0); // По умолчанию
	}
	else
	{
		useGridParamByDefaultCmBx->setCurrentIndex(1); // Пользовательские
	}
	// Подчеркиваем текущий выбранный начальный набор настроек
	QFont font = paramDlg->ui->loadUserGridParamPBtn->font();
	font.setUnderline(isUseUserParam);
	paramDlg->ui->loadUserGridParamPBtn->setFont(font);
	font.setUnderline(!isUseUserParam);
	paramDlg->ui->loadDefaultGridParamPBtn->setFont(font);

	// Приводим пользовательский интерфейс в актуальное состояние
	onLineSlot(onLineChkBx->checkState());
}

void GridFrm::init(ParamDlg* paramDlg)
{
	this->paramDlg = paramDlg;

	initSlot();

	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();
	if(!imageGridSettings.isUseUserParam)
	{
		imageGridSettings = settings->getImageGridSettingsDefault();
	}

	//Установить единицу измерения в метрической системе
	if(imageGridSettings.unit == ICEIMAGE_NS::M)
	{
		paramDlg->ui->hUnitMetricGridTxt->setText("Метр");
		paramDlg->ui->vUnitMetricGridTxt->setText("Метр");
	}
	else
	{
		paramDlg->ui->hUnitMetricGridTxt->setText("Километр");
		paramDlg->ui->vUnitMetricGridTxt->setText("Километр");
	}
	initData(imageGridSettings);
}

// Привести пользовательский интерфейс в актуальное состояние в зависисмости от
// текущих настроек
void GridFrm::settingsChange()
{
	// Берем набор текущих настроек
	ICECORE_NS::ImageGridSettings currentImageGridSettings = getCurrentImageGridSettings();
	// Берем набор начальных настроек по умолчанию
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettingsDefault = settings->getImageGridSettingsDefault();
	// Берем набор начальных пользовательских настроек
	ICECORE_NS::ImageGridSettings imageGridSettingsUser = settings->getImageGridSettings();

	// Если набор настроек совпадает с начальными настройками по умолчанию
	paramDlg->ui->loadDefaultGridParamPBtn->setEnabled(!(currentImageGridSettings == imageGridSettingsDefault));

	// Если набор настроек совпадает с начальными пользовательскими настройками
	paramDlg->ui->loadUserGridParamPBtn->setEnabled(!(currentImageGridSettings == imageGridSettingsUser));
	paramDlg->ui->saveUserGridParamPBtn->setEnabled(paramDlg->ui->loadUserGridParamPBtn->isEnabled());
}

// Получить текущие настройки
ICECORE_NS::ImageGridSettings GridFrm::getCurrentImageGridSettings()
{
	ICECORE_NS::ImageGridSettings imageGridSettings;

	imageGridSettings.hStepPixel = paramDlg->ui->hStepGridPixelDblSpBx->value();
	imageGridSettings.vStepPixel = paramDlg->ui->vStepGridPixelDblSpBx->value();
	imageGridSettings.hStepMetric = paramDlg->ui->hStepGridMetricDblSpBx->value();
	imageGridSettings.vStepMetric = paramDlg->ui->vStepGridMetricDblSpBx->value();

	QString unitStr = paramDlg->ui->hUnitMetricGridTxt->text();
	if(unitStr == "Метр")
	{
		imageGridSettings.unit = ICEIMAGE_NS::M;
	}
	else
	{
		imageGridSettings.unit = ICEIMAGE_NS::KM;
	}
	imageGridSettings.isOnLine = grid()->isOnLine();
	imageGridSettings.isShow = paramDlg->ui->showGridChkBx->isChecked();
	imageGridSettings.isShow800 = paramDlg->ui->showGrid800ChBx->isChecked();
	imageGridSettings.color = grid()->majorPen().color();
	imageGridSettings.isUseUserParam = isUseUserParam;

	return imageGridSettings;
}

// Загрузить настройки сетки По умолчанию
void GridFrm::loadDefaultGridParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettingsDefault = settings->getImageGridSettingsDefault();
	paramDlg->plot()->setupGrid(imageGridSettingsDefault);
	initData(imageGridSettingsDefault);

	// Подчеркиваем текущий выбранный начальный набор настроек
	QFont font = paramDlg->ui->loadDefaultGridParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadDefaultGridParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadUserGridParamPBtn->setFont(font);

	//settingsChange();
}

// Загрузить настройки сетки Пользовательские
void GridFrm::loadUserGridParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();
	paramDlg->plot()->setupGrid(imageGridSettings);
	initData(imageGridSettings);

	// Подчеркиваем текущий выбранный начальный набор настроек
	QFont font = paramDlg->ui->loadUserGridParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadUserGridParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadDefaultGridParamPBtn->setFont(font);

	//settingsChange();
}

// Сохранить текущие настройки сетки как Пользовательские
void GridFrm::saveUserGridParamPBtnPressedSlot()
{
	ICECORE_NS::ImageGridSettings imageGridSettings = getCurrentImageGridSettings();
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	settings->setImageGridSettings(imageGridSettings);
	settingsChange();
}

// Изменить вид настроек сетки, используемые при загрузке изображения
void GridFrm::useGridParamByDefaultCmBxCurrentIndexChangedSlot(int index)
{
	switch(index)
	{
		case 0 :
		{
			isUseUserParam = false;
			break;
		}
		case 1:
		default:
		{
			isUseUserParam = true;
			break;
		}
	}
	settingsChange();
}

// Показать/скрыть сетку
void GridFrm::showGridSlot(int state)
{
	ICEQWT_NS::IceQwtPlot* plt = (ICEQWT_NS::IceQwtPlot*)this->grid()->plot();

	switch(state)
	{
		case Qt::Unchecked :
		{
			plt->setGridVisible(false);
			break;
		}
		case Qt::Checked :
		{
			plt->setGridVisible(true); 
			break;
		}
	}
	plt->replot();
	settingsChange();
}

// Установить/снять привязку сетки "По линейке"
void GridFrm::onLineSlot(int state)
{
	if(!updateUserinterface)
	{
		return;
	}
	ICEQWT_NS::IceQwtPlotGrid* grid = this->grid();

	switch(state)
	{
		case Qt::Unchecked :
		{
			// Сетка не будет по линейке
			grid->setIsOnLine(false);

			//Отсериваем только то, какие единицы измерения установлены на вкладке "Линейка"
			switch(paramDlg->ui->unitCmBx->currentIndex())
			{
				case 0: // Пиксели
				{
					paramDlg->ui->hStepGridPixelDblSpBx->setEnabled(true);
					paramDlg->ui->hUnitGridTxt->setEnabled(true);
					paramDlg->ui->vStepGridPixelDblSpBx->setEnabled(true);
					paramDlg->ui->vUnitGridTxt->setEnabled(true);
					break;
				}
				case 1:
				case 2:
				default: // Метрические системы
				{
					paramDlg->ui->hStepGridMetricDblSpBx->setEnabled(true);
					paramDlg->ui->vStepGridMetricDblSpBx->setEnabled(true);
					paramDlg->ui->hUnitMetricGridTxt->setEnabled(true);
					paramDlg->ui->vUnitMetricGridTxt->setEnabled(true);
					break;
				}
			}
			break;
		}
		case Qt::Checked :
		{
			// Сетка будет по линейке
			grid->setIsOnLine(true);

			// Все засериваем
			paramDlg->ui->hStepGridPixelDblSpBx->setEnabled(false);
			paramDlg->ui->hUnitGridTxt->setEnabled(false);
			paramDlg->ui->vStepGridPixelDblSpBx->setEnabled(false);
			paramDlg->ui->vUnitGridTxt->setEnabled(false);
			paramDlg->ui->hStepGridMetricDblSpBx->setEnabled(false);
			paramDlg->ui->vStepGridMetricDblSpBx->setEnabled(false);
			paramDlg->ui->hUnitMetricGridTxt->setEnabled(false);
			paramDlg->ui->vUnitMetricGridTxt->setEnabled(false);
			break;
		}
	}
	grid->plot()->replot();
	settingsChange();
}

// Установить шаг сетки в пикселях по горизонтали 
void GridFrm::hStepPixelValueChangedSlot(double /*value*/)
{
	grid()->setHStepPixel(paramDlg->ui->hStepGridPixelDblSpBx->value());
	// Вот так вот косвенно применяем изменение шага сетки
	onLineSlot(Qt::Unchecked);
}

// Установить шаг сетки в пикселях по вертикали 
void GridFrm::vStepPixelValueChangedSlot(double /*value*/)
{
	grid()->setVStepPixel(paramDlg->ui->vStepGridPixelDblSpBx->value());
	// Вот так вот косвенно применяем изменение шага сетки
	onLineSlot(Qt::Unchecked);
}

// Установить шаг сетки в метрической системе по горизонтали 
void GridFrm::hStepMetricValueChangedSlot(double /*value*/)
{
	grid()->setHStepMetric(paramDlg->ui->hStepGridMetricDblSpBx->value());
	// Вот так вот косвенно применяем изменение шага сетки
	onLineSlot(Qt::Unchecked);
}

// Установить шаг сетки в метрической системе по вертикали 
void GridFrm::vStepMetricValueChangedSlot(double /*value*/)
{
	grid()->setVStepMetric(paramDlg->ui->vStepGridMetricDblSpBx->value());
	// Вот так вот косвенно применяем изменение шага сетки
	onLineSlot(Qt::Unchecked);
}

// Выбор цвета линии сетки
void GridFrm::colorGridPBtnSlot()
{
	QFrame* colorPenGridClrFrm = paramDlg->ui->colorPenGridClrFrm;
	QPalette palette = colorPenGridClrFrm->palette();
	QColorDialog dialog(palette.color(QPalette::Normal, colorPenGridClrFrm->backgroundRole()), this);
	if(dialog.exec() == QDialog::Accepted)
	{
		//TODO refac
		//palette.setColor(colorPenGridClrFrm->backgroundRole(), dialog.currentColor());
		//colorPenGridClrFrm->setPalette(palette);

		ICEQWT_NS::IceQwtPlotGrid* grid = this->grid();

		QPen pen = grid->majorPen();
		pen.setColor(dialog.currentColor());
		grid->setMajorPen(pen);

		// Косвенно устанавливаем прозрачность для выбранного цвета
		transparencySldrSlot(0);
		//grid->plot()->replot();
		//SettingsChange();
	}
}

// Установка прозрачности сетки
void GridFrm::transparencySldrSlot(int)
{
	ICEQWT_NS::IceQwtPlotGrid* grid = this->grid();

	QSlider* &transparencySldr = paramDlg->ui->transparencySldr;

	QColor color = grid->majorPen().color();
	color.setAlpha(255 - ((double)transparencySldr->value()) / 100.0 * 255);

	QPen pen = grid->majorPen();
	pen.setColor(color);
	grid->setMajorPen(pen); 

	QFrame* &colorPenGridClrFrm = paramDlg->ui->colorPenGridClrFrm;
	QPalette palette = colorPenGridClrFrm->palette();
	palette.setColor(colorPenGridClrFrm->backgroundRole(), color);
	colorPenGridClrFrm->setPalette(palette);

	grid->plot()->replot();

	settingsChange();
}

} // namespace ICEGUI_NS
