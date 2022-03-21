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

	// �������������� ���� � ����������� �� ��������� �����
	// GridFrm1 �������� � ui_paramdlg.h � �������� ���������� ������� ������ (ParamDlg)
	// ��� ����� ��� ���������� ����
	ui->GridFrm1->init(this); 

	// �������������� ���� � ����������� �� ��������� �������
	// LineFrm �������� � ui_paramdlg.h � �������� ���������� ������� ������ (ParamDlg)
	// ��� ����� ��� ���������� ����
	// ��������� ������ ���� ������ ����� �������� �����, ��� ��� � ����� ������ ����
	// ����������� ������� ���������, ������� ������������ � ���� �������� ������
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

// ���������� ������� ������������ ������
// ��� ������� ���������� ������������� ��� �������� ������� ����
bool ParamDlg::setCurrentSection(SECTION section)
{
	QList<QTreeWidgetItem*> findItem;

	switch(section)
	{
		case IMAGE:
		{
			// Qt::MatchRecursive ����� ��� ������ ������ ���������� ���������, ����� �����
			// ����� ���� ������ �� ��������� ������� ������
			findItem = ui->treeWgt->findItems(tr("�����������"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
		case GRID:
		{
			findItem = ui->treeWgt->findItems(tr("�����"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
		case GUIDES:
		{
			findItem = ui->treeWgt->findItems(tr("�������"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
		case ADDED:
		{
			findItem = ui->treeWgt->findItems(tr("�������������"), Qt::MatchExactly | Qt::MatchRecursive);
			break;
		}
	}

	// ����� ������ ���� ����� ������������ �������, ���� ������.
	// � ��� �� ��� � ������ ��� �������� � ����� � ��� �� �������
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
	if(current->text(0) == tr("�������"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("LineFrm"));
	}
	else if(current->text(0) == tr("�����"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("GridFrm"));
	}
	else if(current->text(0) == tr("�������������"))
	{
		ui->stackedWgt->setCurrentWidget(findWidget("AddedFrm"));
	}
	else if(current->text(0) == tr("�����������"))
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
	// ���������� ��������� 
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	imageLineSettings.isUseUserParam = isUseUserParam;
	settings->setImageLineSettings(imageLineSettings);
}

void LineFrm::init(ParamDlg* paramDlg)
{
	this->paramDlg = paramDlg;

	//REFAC ����� ��������
	// ����� �� ����� ��� � � IceQwtPlot::setupLine
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
	// ���������� ������� ���������
	connect(paramDlg->ui->unitCmBx, SIGNAL(currentIndexChanged(int)), this, SLOT(unitCmBxCurrentIndexChangedSlot(int)));
	// ��������/������ �������
	connect(paramDlg->ui->showLineChkBx, SIGNAL(stateChanged(int)), this, SLOT(showLineSlot(int)));
	// ������ ������������ �� ���������
	connect(paramDlg->ui->useDefaultUnitChBox, SIGNAL(stateChanged(int)), this, SLOT(useDefaultUnitSlot(int)));
	// ���������� ����������� �������� �� ���������
	connect(paramDlg->ui->ratioDblSpBx, SIGNAL(valueChanged(double)), this, SLOT(ratioDblSpBxSlot(double)));
	// ���������� ������ ���������
	connect(paramDlg->ui->vOriginDblSpBx, SIGNAL(valueChanged(double)), this, SLOT(vOriginDblSpBxSlot(double)));
	// ���������� ��� �������� �������, ������� ������ ����������� ��� ������ �������� �����������
	connect(paramDlg->ui->useLineParamByDefaultCmBx, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(useLineParamByDefaultCmBxCurrentIndexChangedSlot(int)));
	// ��������� ������� ��������� ������� ��� ����������������
	connect(paramDlg->ui->saveUserLineParamPBtn, SIGNAL(pressed()), this, SLOT(saveUserLineParamPBtnPressedSlot()));
	// ��������� ��������� ������� �� ���������
	connect(paramDlg->ui->loadDefaultLineParamPBtn, SIGNAL(pressed()), this, SLOT(loadDefaultLineParamPBtnSlot()));
	// ��������� ��������� ������� ����������������
	connect(paramDlg->ui->loadUserLineParamPBtn, SIGNAL(pressed()), this, SLOT(loadUserLineParamPBtnSlot()));
}

void LineFrm::initData(const ICECORE_NS::ImageLineSettings& imageLineSettings)
{
	blockSignals(true);

	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();

	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();

	// ���������� ������� ���������
	QComboBox* &unitCmBx = paramDlg->ui->unitCmBx;
	ICEIMAGE_NS::UNIT unit = transform->unit();
	switch(unit)
	{
		case ICEIMAGE_NS::NONE: // �������
		{
			//TODO vvzh ���������� ����� 
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

	// ���������� ������� ��������� ��� ������������ �������� �� ���������
	if(ICEIMAGE_NS::NONE != unit)
	{
		paramDlg->ui->KUnitlbl->setText(unitCmBx->currentText());
		// ���������� ������� ��������� ��� �����
		//paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->currentText());
		//paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->currentText());
	}
	else 
	{
		ICEIMAGE_NS::UNIT prevUnit = transform->metricUnit();
		if(ICEIMAGE_NS::M == prevUnit)
		{
			paramDlg->ui->KUnitlbl->setText(unitCmBx->itemText(1));
			// ���������� ������� ��������� ��� �����
			//paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->itemText(1));
			//paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->itemText(1));
		}
		else
		{
			paramDlg->ui->KUnitlbl->setText(unitCmBx->itemText(2));
			// ���������� ������� ��������� ��� �����
			//paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->itemText(2));
			//paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->itemText(2));
		}
	}
	// ���������� ������� ��������� ��� �������� ������ ���������
	//paramDlg->ui->hUnitBeginCoordTxt->setText(unitCmBx->currentText());
	//paramDlg->ui->vUnitBeginCoordTxt->setText(unitCmBx->currentText());

	// ��������/������ �������
	QCheckBox* &showLineChkBx = paramDlg->ui->showLineChkBx;
	showLineChkBx->setChecked(imageLineSettings.isShow);

	// ���������� ����������� �������� �� ���������
	QDoubleSpinBox* &ratioDblSpBx = paramDlg->ui->ratioDblSpBx;
	ratioDblSpBx->setValue(1.0 / transform->metricRatio()); // ������ ��������� ������ ����������� ����������� �������
	if(ICEIMAGE_NS::NONE == unit)
	{
		ratioDblSpBx->setEnabled(false);
	}

	// ���������� ������ ���������
	QDoubleSpinBox* &hOriginDblSpBx = paramDlg->ui->hOriginDblSpBx;
	hOriginDblSpBx->setValue(plot->rasterItem()->origin().x()); 
	connect(hOriginDblSpBx, SIGNAL(valueChanged(double)), this, SLOT(hOriginDblSpBxSlot(double)));
	QDoubleSpinBox* &vOriginDblSpBx = paramDlg->ui->vOriginDblSpBx;
	vOriginDblSpBx->setValue(plot->rasterItem()->origin().y()); 

	// ���������� ���� ��� ������ ��������� "������ ������������ �� ���������"
	//ICECORE_NS::Settings* settings = plot->getSettings();
	//ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	//if(!imageLineSettings.isUseUserParam)
	//{
	//	imageLineSettings = settings->getImageLineSettingsDefault();
	//}
	paramDlg->ui->useDefaultUnitChBox->setChecked(imageLineSettings.isUseAlwaysUnit);

	// ���������� ���� "������������ ���������������� ��������� �� ���������"
	// paramDlg->ui->useDefaultAllLineParamsChBx->setChecked(imageLineSettings.isUseUserParam);

	blockSignals(false);

	// ���������� ��� �������� �������, ������� ������ ����������� ��� ������ �������� �����������
	QComboBox* &useLineParamByDefaultCmBx = paramDlg->ui->useLineParamByDefaultCmBx;
	isUseUserParam = imageLineSettings.isUseUserParam;
	if(!isUseUserParam)
	{
		useLineParamByDefaultCmBx->setCurrentIndex(0); // �� ���������
	}
	else
	{
		useLineParamByDefaultCmBx->setCurrentIndex(1); // ����������������
	}
	// ������������ ������� ��������� ��������� ����� ��������
	QFont font = paramDlg->ui->loadUserLineParamPBtn->font();
	font.setUnderline(isUseUserParam);
	paramDlg->ui->loadUserLineParamPBtn->setFont(font);
	font.setUnderline(!isUseUserParam);
	paramDlg->ui->loadDefaultLineParamPBtn->setFont(font);

	// �������� ���������������� ��������� � ���������� ���������
	unitCmBxCurrentIndexChangedSlot(unitCmBx->currentIndex());
}

// �������� ������ �������
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

// ������ ������������ �� ���������
void LineFrm::useDefaultUnitSlot(int)
{
	settingsChange();
}

// �������� ���������������� ��������� � ���������� ��������� � ������������ ��
// ������� ��������
void LineFrm::settingsChange()
{
	Q_ASSERT(paramDlg);
	// ����� ����� ������� ��������
	ICECORE_NS::ImageLineSettings currentImageLineSettings = getCurrentImageLineSettings();
	// ����� ����� ��������� �������� �� ���������
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettingsDefault = settings->getImageLineSettingsDefault();
	// ����� ����� ��������� ���������������� ��������
	ICECORE_NS::ImageLineSettings imageLineSettingsUser = settings->getImageLineSettings();

	// ���� ����� �������� ��������� � ���������� ����������� �� ���������
	paramDlg->ui->loadDefaultLineParamPBtn->setEnabled(!(currentImageLineSettings == imageLineSettingsDefault));

	// ���� ����� �������� ��������� � ���������� ����������������� �����������
	paramDlg->ui->loadUserLineParamPBtn->setEnabled(!(currentImageLineSettings == imageLineSettingsUser));
	paramDlg->ui->saveUserLineParamPBtn->setEnabled(paramDlg->ui->loadUserLineParamPBtn->isEnabled());
}

// �������� ������� ���������
ICECORE_NS::ImageLineSettings LineFrm::getCurrentImageLineSettings()
{
	ICECORE_NS::ImageLineSettings imageLineSettings;
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	imageLineSettings.origin = plot->rasterItem()->origin();

	// ������� �� ��, ��� ��� ��� � � Y ��������� ������������� ���� � �� ��
	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();
	imageLineSettings.ratio = transform->ratio();
	imageLineSettings.unit = transform->unit();
	imageLineSettings.isShow = paramDlg->ui->showLineChkBx->isChecked();

	imageLineSettings.isUseAlwaysUnit = paramDlg->ui->useDefaultUnitChBox->isChecked();
	imageLineSettings.isUseUserParam = isUseUserParam;
	return imageLineSettings;
}

// �������� ��� �������� �������, ������������ ��� �������� �����������
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

// ��������� ������� ��������� ������� ��� ����������������
void LineFrm::saveUserLineParamPBtnPressedSlot()
{
	ICECORE_NS::ImageLineSettings imageLineSettings = getCurrentImageLineSettings();
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	settings->setImageLineSettings(imageLineSettings);
	settingsChange();
}

// ��������� ��������� ������� �� ���������
void LineFrm::loadDefaultLineParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettingsDefault = settings->getImageLineSettingsDefault();
	paramDlg->plot()->setupLine(imageLineSettingsDefault);
	initData(imageLineSettingsDefault);

	// ������������ ������� ��������� ��������� ����� ��������
	QFont font = paramDlg->ui->loadDefaultLineParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadDefaultLineParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadUserLineParamPBtn->setFont(font);
	//settingsChange();
}

// ��������� ��������� ������� ����������������
void LineFrm::loadUserLineParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	paramDlg->plot()->setupLine(imageLineSettings);
	initData(imageLineSettings);

	// ������������ ������� ��������� ��������� ����� ��������
	QFont font = paramDlg->ui->loadUserLineParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadUserLineParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadDefaultLineParamPBtn->setFont(font);
	//settingsChange();
}

// �������� �������������
void LineFrm::updateTransform(const ICEQWT_NS::IceUnitRatioQwtTransform* transform)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();

	// ���������� ������������� ������� � ���, ��� ���������� ������� ���������. ��� ����,
	// ����� plot->rescaler()->rescale() �� ������ �������� � ����������� ������������� ����
	// ��� �� ��������� ������� ��� � ����� (��� �������� oldInterval). ����� ��������� �����
	// ������������� ��� �� ��������� ������� ����� � ������ �������� ��������� - ������� �����
	// �� �� �������� � ������ ������� ���������
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

// �������� ���
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

// ���������� ����������� �������� �� ���������
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

// ���������� ������ ��������� �� ��� �
void LineFrm::hOriginDblSpBxSlot(double value)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	QPointF origin = plot->rasterItem()->origin();
	origin.setX(value);
	plot->rasterItem()->setOrigin(origin); 
	updateAxis();
}

// ���������� ������ ��������� �� ��� Y
void LineFrm::vOriginDblSpBxSlot(double value)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();
	QPointF origin = plot->rasterItem()->origin();
	origin.setY(value);
	plot->rasterItem()->setOrigin(origin); 
	updateAxis();
}

// ���������� ������� ���������
void LineFrm::unitCmBxCurrentIndexChangedSlot(int /*index*/)
{
	ICEQWT_NS::IceQwtPlot* plot = paramDlg->imageEditor->icePlot();

	// ����������� �������� �� ���������
	QDoubleSpinBox* &ratioDblSpBx = paramDlg->ui->ratioDblSpBx;
	// ��� ����� �� ����������� � ��������� � ��������
	QDoubleSpinBox* &hStepGridPixelDblSpBx = paramDlg->ui->hStepGridPixelDblSpBx;
	QDoubleSpinBox* &vStepGridPixelDblSpBx = paramDlg->ui->vStepGridPixelDblSpBx;
	// ��� ����� �� ����������� � ��������� � ����������� �������
	QDoubleSpinBox* &hStepGridMetricDblSpBx = paramDlg->ui->hStepGridMetricDblSpBx;
	QDoubleSpinBox* &vStepGridMetricDblSpBx = paramDlg->ui->vStepGridMetricDblSpBx;
	// ������� ��������� ������������ �������� �� ���������
	QLabel* &KUnitlbl = paramDlg->ui->KUnitlbl;

	QComboBox* &unitCmBx = paramDlg->ui->unitCmBx;

	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)plot->axisScaleEngine(QwtPlot::xTop)->transformation();

	// �������� ��������������� � ������� �����������
	QPointF origin = transform->transform(plot->rasterItem()->origin());
	//// ��� ����� ��������������� � ������� �����������
	//double hStepMetric = transform->transform(paramDlg->ui->hStepGridMetricDblSpBx->value());
	//double vStepMetric = transform->transform(paramDlg->ui->vStepGridMetricDblSpBx->value());

	switch(unitCmBx->currentIndex())
	{
		case 0: // �������
		{
			transform->setUnit(ICEIMAGE_NS::NONE);
			ratioDblSpBx->setEnabled(false);
			hStepGridMetricDblSpBx->setEnabled(false);
			vStepGridMetricDblSpBx->setEnabled(false);
			break;
		}
		case 1: // �����
		{
			transform->setUnit(ICEIMAGE_NS::M);
			ratioDblSpBx->blockSignals(true);
			ratioDblSpBx->setValue(1.0 / transform->ratio());
			ratioDblSpBx->blockSignals(false);
			ratioDblSpBx->setEnabled(true);
			KUnitlbl->setText(unitCmBx->currentText());
			break;
		}
		case 2: default: // ���������
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

	// � ������ �������� ��������������� � ����� ������������� ������� ���������
	origin = transform->invTransform(origin);
	// ��� �������� ������ ��������� ����� ������������� ���������� �������
	// ���������
	paramDlg->ui->hUnitBeginCoordTxt->setText(unitCmBx->currentText());
	paramDlg->ui->vUnitBeginCoordTxt->setText(unitCmBx->currentText());
	updateAxes = false;
	paramDlg->ui->vOriginDblSpBx->setValue(origin.x());
	paramDlg->ui->hOriginDblSpBx->setValue(origin.y());
	updateAxes = true;

	updateTransform(transform);
	updateAxis();

	// ��������������� � ����� ������������� ������� � ��� ����� � ����������� �������
	switch(unitCmBx->currentIndex())
	{
		case 0:
		{
			hStepGridPixelDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked()); // �������� ����, ������ ���� "�� �� �������" (3)
			vStepGridPixelDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked());
			break;
		}
		case 1:
		case 2:
		default: // ����������� �������
		{
			double mnj;
			if(unitCmBx->currentText() != paramDlg->ui->vUnitMetricGridTxt->text())
			{
				if(unitCmBx->currentIndex() == 1) // �� �� � �
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
			hStepGridMetricDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked());  // ��. (3)
			vStepGridMetricDblSpBx->setEnabled(!paramDlg->ui->onLineChkBx->isChecked());  // ��. (3)
			paramDlg->ui->hUnitMetricGridTxt->setText(unitCmBx->currentText());
			paramDlg->ui->vUnitMetricGridTxt->setText(unitCmBx->currentText());
			break;
		}
	}
	// ���������� ��������� ������� "�����"
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

	// ���������� ��������� 
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

	// ��������/������ �����
	connect(paramDlg->ui->showGridChkBx, SIGNAL(stateChanged(int)), this, SLOT(showGridSlot(int)));
	// ����������/����� �������� ����� "�� �������"
	connect(paramDlg->ui->onLineChkBx, SIGNAL(stateChanged(int)), this, SLOT(onLineSlot(int)));
	// ���������� ��� ����� � �������� �� ����������� � �������� 
	connect(paramDlg->ui->hStepGridPixelDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(hStepPixelValueChangedSlot(double)));
	// ���������� ��� ����� � �������� �� ��������� � ��������
	connect(paramDlg->ui->vStepGridPixelDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(vStepPixelValueChangedSlot(double)));
	// ���������� ��� ����� � ����������� ������� �� ����������� 
	connect(paramDlg->ui->hStepGridMetricDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(hStepMetricValueChangedSlot(double)));
	// ���������� ��� ����� � ����������� ������� �� ��������� 
	connect(paramDlg->ui->vStepGridMetricDblSpBx, SIGNAL(valueChanged(double)), 
		this, SLOT(vStepMetricValueChangedSlot(double)));
	// ����� ����� ����� �����
	connect(paramDlg->ui->colorPenGridPBtn, SIGNAL(clicked()), this, SLOT(colorGridPBtnSlot()));
	// ��������� ������������ �����
	connect(paramDlg->ui->transparencySldr, SIGNAL(valueChanged(int)), this, SLOT(transparencySldrSlot(int)));
	// ���������� ��� �������� �����, ������� ������ ����������� ��� ������ �������� �����������
	connect(paramDlg->ui->useGridParamByDefaultCmBx, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(useGridParamByDefaultCmBxCurrentIndexChangedSlot(int)));
	// ��������� ������� ��������� ����� ��� ����������������
	connect(paramDlg->ui->saveUserGridParamPBtn, SIGNAL(pressed()), this, SLOT(saveUserGridParamPBtnPressedSlot()));
	// ��������� ��������� ����� �� ���������
	connect(paramDlg->ui->loadDefaultGridParamPBtn, SIGNAL(pressed()), this, SLOT(loadDefaultGridParamPBtnSlot()));
	// ��������� ��������� ����� ����������������
	connect(paramDlg->ui->loadUserGridParamPBtn, SIGNAL(pressed()), this, SLOT(loadUserGridParamPBtnSlot()));
}

void GridFrm::initData(const ICECORE_NS::ImageGridSettings& imageGridSettings)
{
	blockSignals(true);

	ICEQWT_NS::IceQwtPlotGrid* grid = this->grid();

	// ��������/������ �����
	QCheckBox* &showGridChkBx = paramDlg->ui->showGridChkBx;
	showGridChkBx->setChecked(((ICEQWT_NS::IceQwtPlot*)(grid->plot()))->gridVisible());

	// ����������/����� �������� ����� "�� �������"
	QCheckBox* &onLineChkBx = paramDlg->ui->onLineChkBx;
	onLineChkBx->setChecked(grid->isOnLine());

	// ���������� ��� ����� � �������� �� ����������� 
	QDoubleSpinBox* &hStepGridPixelDblSpBx = paramDlg->ui->hStepGridPixelDblSpBx;
	hStepGridPixelDblSpBx->setValue(grid->hStepPixel());

	// ���������� ��� ����� � �������� �� ��������� 
	QDoubleSpinBox* &vStepGridPixelDblSpBx = paramDlg->ui->vStepGridPixelDblSpBx;
	vStepGridPixelDblSpBx->setValue(grid->vStepPixel());

	// ���������� ��� ����� � ����������� ������� �� ����������� 
	QDoubleSpinBox* &hStepGridMetricDblSpBx = paramDlg->ui->hStepGridMetricDblSpBx;
	hStepGridMetricDblSpBx->setValue(grid->hStepMetric());

	// ���������� ��� ����� � ����������� ������� �� ��������� 
	QDoubleSpinBox* &vStepGridMetricDblSpBx = paramDlg->ui->vStepGridMetricDblSpBx;
	vStepGridMetricDblSpBx->setValue(grid->vStepMetric());

	// ����������/����� �������� ����� �������� � ��������� 800% � ����
	QCheckBox* &showGrid800ChBx = paramDlg->ui->showGrid800ChBx;
	showGrid800ChBx->setChecked(grid->isShow800());

	// ����� ����� ����� �����
	// ������������� ������� ���� ����� � ��������� ����� �����
	QFrame* &colorPenGridClrFrm = paramDlg->ui->colorPenGridClrFrm;
	QPalette palette = colorPenGridClrFrm->palette();
	QPen pen = grid->majorPen();
	palette.setColor(colorPenGridClrFrm->backgroundRole(), pen.color());
	colorPenGridClrFrm->setPalette(palette);

	// ��������� ������������ �����
	QSlider* &transparencySldr = paramDlg->ui->transparencySldr;
	transparencySldr->setValue(100 - grid->majorPen().color().alpha() / 255.0 * 100);

	blockSignals(false);

	// ���������� ��� �������� �����, ������� ������ ����������� ��� ������ �������� �����������
	// �������� ���������������� ��������� �����
	//ICECORE_NS::Settings* settings = paramDlg->plot()->getSettings(); 
	//ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();

	// �������������, ����� ��������� ����� ������������ ��� ��������
	QComboBox* &useGridParamByDefaultCmBx = paramDlg->ui->useGridParamByDefaultCmBx;
	isUseUserParam = imageGridSettings.isUseUserParam;
	if(!isUseUserParam)
	{
		useGridParamByDefaultCmBx->setCurrentIndex(0); // �� ���������
	}
	else
	{
		useGridParamByDefaultCmBx->setCurrentIndex(1); // ����������������
	}
	// ������������ ������� ��������� ��������� ����� ��������
	QFont font = paramDlg->ui->loadUserGridParamPBtn->font();
	font.setUnderline(isUseUserParam);
	paramDlg->ui->loadUserGridParamPBtn->setFont(font);
	font.setUnderline(!isUseUserParam);
	paramDlg->ui->loadDefaultGridParamPBtn->setFont(font);

	// �������� ���������������� ��������� � ���������� ���������
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

	//���������� ������� ��������� � ����������� �������
	if(imageGridSettings.unit == ICEIMAGE_NS::M)
	{
		paramDlg->ui->hUnitMetricGridTxt->setText("����");
		paramDlg->ui->vUnitMetricGridTxt->setText("����");
	}
	else
	{
		paramDlg->ui->hUnitMetricGridTxt->setText("��������");
		paramDlg->ui->vUnitMetricGridTxt->setText("��������");
	}
	initData(imageGridSettings);
}

// �������� ���������������� ��������� � ���������� ��������� � ������������ ��
// ������� ��������
void GridFrm::settingsChange()
{
	// ����� ����� ������� ��������
	ICECORE_NS::ImageGridSettings currentImageGridSettings = getCurrentImageGridSettings();
	// ����� ����� ��������� �������� �� ���������
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettingsDefault = settings->getImageGridSettingsDefault();
	// ����� ����� ��������� ���������������� ��������
	ICECORE_NS::ImageGridSettings imageGridSettingsUser = settings->getImageGridSettings();

	// ���� ����� �������� ��������� � ���������� ����������� �� ���������
	paramDlg->ui->loadDefaultGridParamPBtn->setEnabled(!(currentImageGridSettings == imageGridSettingsDefault));

	// ���� ����� �������� ��������� � ���������� ����������������� �����������
	paramDlg->ui->loadUserGridParamPBtn->setEnabled(!(currentImageGridSettings == imageGridSettingsUser));
	paramDlg->ui->saveUserGridParamPBtn->setEnabled(paramDlg->ui->loadUserGridParamPBtn->isEnabled());
}

// �������� ������� ���������
ICECORE_NS::ImageGridSettings GridFrm::getCurrentImageGridSettings()
{
	ICECORE_NS::ImageGridSettings imageGridSettings;

	imageGridSettings.hStepPixel = paramDlg->ui->hStepGridPixelDblSpBx->value();
	imageGridSettings.vStepPixel = paramDlg->ui->vStepGridPixelDblSpBx->value();
	imageGridSettings.hStepMetric = paramDlg->ui->hStepGridMetricDblSpBx->value();
	imageGridSettings.vStepMetric = paramDlg->ui->vStepGridMetricDblSpBx->value();

	QString unitStr = paramDlg->ui->hUnitMetricGridTxt->text();
	if(unitStr == "����")
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

// ��������� ��������� ����� �� ���������
void GridFrm::loadDefaultGridParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettingsDefault = settings->getImageGridSettingsDefault();
	paramDlg->plot()->setupGrid(imageGridSettingsDefault);
	initData(imageGridSettingsDefault);

	// ������������ ������� ��������� ��������� ����� ��������
	QFont font = paramDlg->ui->loadDefaultGridParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadDefaultGridParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadUserGridParamPBtn->setFont(font);

	//settingsChange();
}

// ��������� ��������� ����� ����������������
void GridFrm::loadUserGridParamPBtnSlot()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageGridSettings imageGridSettings = settings->getImageGridSettings();
	paramDlg->plot()->setupGrid(imageGridSettings);
	initData(imageGridSettings);

	// ������������ ������� ��������� ��������� ����� ��������
	QFont font = paramDlg->ui->loadUserGridParamPBtn->font();
	font.setUnderline(true);
	paramDlg->ui->loadUserGridParamPBtn->setFont(font);
	font.setUnderline(false);
	paramDlg->ui->loadDefaultGridParamPBtn->setFont(font);

	//settingsChange();
}

// ��������� ������� ��������� ����� ��� ����������������
void GridFrm::saveUserGridParamPBtnPressedSlot()
{
	ICECORE_NS::ImageGridSettings imageGridSettings = getCurrentImageGridSettings();
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	settings->setImageGridSettings(imageGridSettings);
	settingsChange();
}

// �������� ��� �������� �����, ������������ ��� �������� �����������
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

// ��������/������ �����
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

// ����������/����� �������� ����� "�� �������"
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
			// ����� �� ����� �� �������
			grid->setIsOnLine(false);

			//���������� ������ ��, ����� ������� ��������� ����������� �� ������� "�������"
			switch(paramDlg->ui->unitCmBx->currentIndex())
			{
				case 0: // �������
				{
					paramDlg->ui->hStepGridPixelDblSpBx->setEnabled(true);
					paramDlg->ui->hUnitGridTxt->setEnabled(true);
					paramDlg->ui->vStepGridPixelDblSpBx->setEnabled(true);
					paramDlg->ui->vUnitGridTxt->setEnabled(true);
					break;
				}
				case 1:
				case 2:
				default: // ����������� �������
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
			// ����� ����� �� �������
			grid->setIsOnLine(true);

			// ��� ����������
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

// ���������� ��� ����� � �������� �� ����������� 
void GridFrm::hStepPixelValueChangedSlot(double /*value*/)
{
	grid()->setHStepPixel(paramDlg->ui->hStepGridPixelDblSpBx->value());
	// ��� ��� ��� �������� ��������� ��������� ���� �����
	onLineSlot(Qt::Unchecked);
}

// ���������� ��� ����� � �������� �� ��������� 
void GridFrm::vStepPixelValueChangedSlot(double /*value*/)
{
	grid()->setVStepPixel(paramDlg->ui->vStepGridPixelDblSpBx->value());
	// ��� ��� ��� �������� ��������� ��������� ���� �����
	onLineSlot(Qt::Unchecked);
}

// ���������� ��� ����� � ����������� ������� �� ����������� 
void GridFrm::hStepMetricValueChangedSlot(double /*value*/)
{
	grid()->setHStepMetric(paramDlg->ui->hStepGridMetricDblSpBx->value());
	// ��� ��� ��� �������� ��������� ��������� ���� �����
	onLineSlot(Qt::Unchecked);
}

// ���������� ��� ����� � ����������� ������� �� ��������� 
void GridFrm::vStepMetricValueChangedSlot(double /*value*/)
{
	grid()->setVStepMetric(paramDlg->ui->vStepGridMetricDblSpBx->value());
	// ��� ��� ��� �������� ��������� ��������� ���� �����
	onLineSlot(Qt::Unchecked);
}

// ����� ����� ����� �����
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

		// �������� ������������� ������������ ��� ���������� �����
		transparencySldrSlot(0);
		//grid->plot()->replot();
		//SettingsChange();
	}
}

// ��������� ������������ �����
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
