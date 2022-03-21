#include "filtereditdialog.h"

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>

#include <icecore/core.h>
#include <icecore/settings.h>
#include <icecore/filteritemmodel.h>

#include <iceproc/editwidget.h>
#include <iceproc/imageprocinterface.h>
#include <iceproc/imageprocinterfaceqobject.h>
#include <iceproc/imagewindowinterface.h>

#include <iceodb/odb/filterparam.hxx>
#include <iceodb/odb/filterimage.hxx>

#include "sectionselectdialog.h"
#include "imageinfowidget.h"
#include "processingwidget.h"

#define IMAGE_WIDGET_WIDTH 250
#define ADDED_HEIGHT 290
#define UP_BORDER 50
#define DOWN_BORDER 50

namespace ICEGUI_NS
{

QMap<QString, ICEGUI_NS::FilterEditDialog*> FilterEditDialog::createFilterEditDialogMap =
	QMap<QString, ICEGUI_NS::FilterEditDialog*>();

FilterEditDialog::FilterEditDialog(const QModelIndex& filterParamModelIndex, ICEPROC_NS::ImageProcInterface* proc,
	ICEPROC_NS::ImageWindowInterface* imageWindowInterface, QWidget* parent, Qt::WindowFlags f) :
	QDialog(parent, f), m_filterParamModelIndex(filterParamModelIndex), m_proc(proc),
	m_imageWindowInterface(imageWindowInterface), m_imageWidgetWidth(0),
	m_databaseConnected(false), m_filterModel(0)
{
	ui.setupUi(this);

	if(!m_filterParamModelIndex.isValid())
	{
		setWindowTitle(tr("Создать ") + m_proc->getShortName());
		ui.closeChBox->hide();
	}
	else
	{
		setWindowTitle(m_proc->getShortName());
	}
	Q_ASSERT(m_proc);
	m_contentWidget = proc->getEditWidget(m_imageWindowInterface);
	Q_ASSERT(m_contentWidget);
	m_contentWidgetWidth = m_contentWidget->width() + ui.paramWidgetLayout->contentsMargins().left() +
		ui.paramWidgetLayout->contentsMargins().right();
	ui.paramWidget->setFixedWidth(m_contentWidgetWidth);

	if(m_contentWidget)
	{
		connect(m_contentWidget, SIGNAL(maskCreated(int, const QString&)),
			this, SLOT(onMaskCreated(int, const QString&)));
		connect(m_contentWidget, SIGNAL(maskRemoved(int)), this, SLOT(onMaskRemoved(int)));
		connect(m_contentWidget, SIGNAL(currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)),
			this, SLOT(onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)));
		ui.mainFrameLayout->addWidget(m_contentWidget);
	}
	ui.showImagePB->setChecked(true);

	initSize();
	if(!m_contentWidget->isWithImages())
	{
		ui.showImagePB->hide();
		showImagePB_clicked(false);
	}
	else
	{
		showImagePB_clicked(true);
	}
	ui.backgroundMaskViewWidget->setMaskName(tr("Фон"));
	ui.backgroundMaskViewWidget->setLabelText(tr("Фон:"));
	ui.backgroundMaskViewWidget->setUseColorState(Qt::Unchecked);
}

FilterEditDialog* FilterEditDialog::getFilterEditDialog(unsigned int filterParamId) const
{
	return m_openFilterEditDialogMap.value(filterParamId);
}

FilterEditDialog* FilterEditDialog::createFilterEditDialog(const QModelIndex& filterParamModelIndex,
	QWidget* parent, Qt::WindowFlags f)
{
	unsigned int filterParamId = ICECORE_NS::FilterItemModel::getFilterId(filterParamModelIndex);
	FilterEditDialog* result = m_openFilterEditDialogMap.value(filterParamId);
	if(!result)
	{
		result = new FilterEditDialog(filterParamModelIndex, m_proc, m_imageWindowInterface,
			parent, f);
		result->setModel(m_filterModel);
		result->setWindowIcon(m_proc->getViewIcon());

		connect(result, SIGNAL(filterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>&)),
			this, SIGNAL(filterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>&)));
		connect(result, SIGNAL(started(ICEGUI_NS::FilterEditDialog*)), this, SIGNAL(started(ICEGUI_NS::FilterEditDialog*)));
		connect(result, SIGNAL(finished(ICEGUI_NS::FilterEditDialog*)), this, SIGNAL(finished(ICEGUI_NS::FilterEditDialog*)));
		connect(result, SIGNAL(finished(ICEGUI_NS::FilterEditDialog*)), this, SLOT(onOpenDialogFinished(ICEGUI_NS::FilterEditDialog*)));
		connect(this, SIGNAL(currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)),
			result, SLOT(onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)));
		connect(this, SIGNAL(databaseConnectionChanged(bool)), result, SLOT(onDatabaseConnectionChanged(bool)));

		result->onDatabaseConnectionChanged(m_databaseConnected);
		m_openFilterEditDialogMap.insert(filterParamId, result);
	}
	return result;
}

FilterEditDialog::~FilterEditDialog()
{
	QList<unsigned int> keyList = m_openFilterEditDialogMap.keys();
	QList<unsigned int>::iterator iterEnd = keyList.end();
	for(QList<unsigned int>::iterator iter = keyList.begin(); iter != iterEnd; iter++)
	{
		ICEGUI_NS::FilterEditDialog* dialog = m_openFilterEditDialogMap.take(*iter);
		delete dialog;
	}
}

//TODO refac
FilterEditDialog::FilterEditDialog(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
	ICEPROC_NS::ImageProcInterface* proc, QWidget* parent, Qt::WindowFlags f) :
	QDialog(parent, f), m_filterParam(filterParam), m_proc(proc), m_imageWindowInterface(0),
	m_imageWidgetWidth(0), m_databaseConnected(false), m_filterModel(0)
{
	ui.setupUi(this);

	Q_ASSERT(m_proc);
	m_contentWidget = proc->getEditWidget(m_imageWindowInterface);
	Q_ASSERT(m_contentWidget);
	m_contentWidgetWidth = m_contentWidget->width() + ui.paramWidgetLayout->contentsMargins().left() +
		ui.paramWidgetLayout->contentsMargins().right();
	ui.paramWidget->setFixedWidth(m_contentWidgetWidth);

	if(m_contentWidget)
	{
		connect(m_contentWidget, SIGNAL(maskCreated(int, const QString&)),
			this, SLOT(onMaskCreated(int, const QString&)));
		connect(m_contentWidget, SIGNAL(maskRemoved(int)), this, SLOT(onMaskRemoved(int)));
		connect(m_contentWidget, SIGNAL(currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)),
			this, SLOT(onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>&)));
		ui.mainFrameLayout->addWidget(m_contentWidget);
	}
	ui.showImagePB->setChecked(true);

	initSize();
	if(!m_contentWidget->isWithImages())
	{
		ui.showImagePB->hide();
		showImagePB_clicked(false);
	}
	else
	{
		showImagePB_clicked(true);
	}
	ui.backgroundMaskViewWidget->setMaskName(tr("Фон"));
	ui.backgroundMaskViewWidget->setLabelText(tr("Фон:"));
	ui.backgroundMaskViewWidget->setUseColorState(Qt::Unchecked);

	setWindowTitle(m_proc->getShortName());
}

void FilterEditDialog::init()
{
	initData();
	ui.useFilterPB->hide();
	ui.saveFilterToFilePB->hide();
	ui.saveFilterPB->hide();
	ui.cancelPB->setText(tr("Закрыть"));
}

unsigned int FilterEditDialog::getFilterParamId() const
{
	return ICECORE_NS::FilterItemModel::getFilterId(m_filterParamModelIndex);
}

QString FilterEditDialog::getName() const
{
	return ui.nameLE->text();
}

const QString& FilterEditDialog::getDllName() const
{
	Q_ASSERT(m_proc);
	return m_proc->getDllName();
}

void FilterEditDialog::setUseFilterPB(bool enabled)
{
	ui.useFilterPB->setEnabled(enabled);
}

bool FilterEditDialog::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	if(m_contentWidget)
	{
		if(m_contentWidget->prepareFilter(imageBuilder))
		{
			return true;
		}
	}
	return false;
}

bool FilterEditDialog::isDbConnected() const
{
	return m_databaseConnected;
}

QVector<QSharedPointer<QObject> > FilterEditDialog::getParamVector() const
{
	QVector<QSharedPointer<QObject> > result;
	if(m_contentWidget)
	{
		for(int i=0; i<m_contentWidget->getMaskParamCount(); i++)
		{
			result.append(m_contentWidget->getParamObject(i));
		}
	}
	return result;
}

QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > FilterEditDialog::getMaskViewVector() const
{
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > result;
	if(m_contentWidget)
	{
		for(int i=0; i<m_contentWidget->getMaskParamCount(); i++)
		{
			result.append(m_contentWidget->getMaskView(i));
		}
	}
	return result;
}

QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > FilterEditDialog::getLineViewVector() const
{
	QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > result;
	if(m_contentWidget)
	{
		for(int i=0; i<m_contentWidget->getMaskParamCount(); i++)
		{
			result.append(m_contentWidget->getLineView(i));
		}
	}
	return result;
}

const std::tr1::shared_ptr<ICEODB_NS::MaskView>& FilterEditDialog::getBackgroundMaskView() const
{
	return ui.backgroundMaskViewWidget->getMaskView();
}

void FilterEditDialog::clearFilterEditDialogMap()
{
	QList<QString> keyList = createFilterEditDialogMap.keys();
	QList<QString>::iterator iterEnd = keyList.end();
	for(QList<QString>::iterator iter = keyList.begin(); iter != iterEnd; iter++)
	{
		ICEGUI_NS::FilterEditDialog* dialog = createFilterEditDialogMap.take(*iter);
		delete dialog;
	}
}

void FilterEditDialog::setImage(int index, const QString& name, const QString& filename,
	const QImage& image)
{
	ImageInfoWidget* imageInfoWidget = 0;
	for(int i=0; i<ui.imageListWidgetLayout->count(); i++)
	{
		QLayoutItem* item = ui.imageListWidgetLayout->itemAt(i);
		if(item)
		{
			ImageInfoWidget* currentImageInfoWidget = dynamic_cast<ImageInfoWidget*>(item->widget());
			if(currentImageInfoWidget && currentImageInfoWidget->getIndex() == index)
			{
				imageInfoWidget = currentImageInfoWidget;
				if(image.isNull())
				{
					ui.imageListWidgetLayout->removeWidget(imageInfoWidget);
					delete imageInfoWidget;
					return;
				}
				break;
			}
		}
	}
	if(!image.isNull())
	{
		if(!imageInfoWidget)
		{
			imageInfoWidget = new ImageInfoWidget(ui.imageListWidget);
			ui.imageListWidgetLayout->insertWidget(index, imageInfoWidget);
		}
		imageInfoWidget->setImage(index, name, filename, image);
	}
}

void FilterEditDialog::setModel(ICECORE_NS::FilterItemModel* filterModel)
{
	m_filterModel = filterModel;
}

void FilterEditDialog::showDialog()
{
	if(isVisible())
	{
		QDialog::hide();
	}
	QDialog::show();
	emit started(this);
}

//TODO refac
void FilterEditDialog::initData()
{
	m_filterParamFromDb = std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	ui.nameLE->clear();
	ui.descriptionTE->clear();
	ui.useFilterPB->show();
	ui.saveFilterToFilePB->show();
	ui.saveFilterPB->show();
	ui.cancelPB->setText(tr("Отмена"));

	if(m_contentWidget)
	{
		for(int i=0; i<m_contentWidget->getMaskParamCount(); i++)
		{
			setImage(i, "", "", QImage());
		}
		m_contentWidget->setFilterParamId(ICECORE_NS::FilterItemModel::getFilterId(
			m_filterParamModelIndex));
		if(m_imageWindowInterface)
		{
			setUseFilterPB(m_contentWidget->isCanUsed(m_imageWindowInterface->getCurrentImageBuilder()));
		}
		else
		{
			setUseFilterPB(false);
		}
	}
	Q_ASSERT(m_proc && m_filterModel);
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam;
	if(m_filterParamModelIndex.isValid())
	{
		try
		{
			filterParam = m_filterModel->getFilterParam(m_filterParamModelIndex);
			m_filterParamFromDb = filterParam;
		}
		catch(const ICECORE_NS::DataBaseException& exc)
		{
			QMessageBox::warning(this, m_proc->getViewName(),
				tr("Ошибка работы с базой данных\r\n") +
				QString::fromStdString(exc.getErrorDescription()));
		}
	}
	if(!filterParam.get())
	{
		filterParam = m_filterParam;
	}
	if(filterParam.get())
	{
		setWindowTitle(m_proc->getShortName() + ": " + filterParam->getName());
		ui.nameLE->setText(filterParam->getName());
		ui.descriptionTE->setPlainText(filterParam->getDescription());
		ui.backgroundMaskViewWidget->setMaskView(filterParam->getBackgroundView());

		if(m_contentWidget)
		{
			m_contentWidget->clear();
			int childCount = 0;
			for(int i=0; i<filterParam->getChildParamCount(); i++)
			{
				std::tr1::shared_ptr<ICEODB_NS::FilterParam> currentFilterParam =
					m_filterModel->getChildFilterParam(filterParam, i);
				if(!currentFilterParam.get())
				{
					continue;
				}
				int addedChildCount = m_contentWidget->setChildFilterParam(childCount,
					currentFilterParam);
				childCount += addedChildCount;
			}
			for(int i=0; i<filterParam->getMaskParamCount(); i++)
			{
				std::tr1::shared_ptr<ICEODB_NS::MaskParam> currentMaskParam =
					m_filterModel->getMaskParam(filterParam, i);
				if(!currentMaskParam.get())
				{
					continue;
				}
				m_contentWidget->setMaskParam(i, currentMaskParam);
				std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage =
					currentMaskParam->getFilterImage();
				if(filterImage.get())
				{
					QString filename = filterImage->getFilename();
					QString name = m_contentWidget->getMaskName(i);
					QImage image((uchar*)filterImage->getImage().data(),
						filterImage->getWidth(), filterImage->getHeight(),
						(QImage::Format)filterImage->getFormat());
						setImage(i, name, filename, image);
				}
			}
		}
	}
	nameLE_textChanged(ui.nameLE->text());
}

void FilterEditDialog::show(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
	bool filterCanBeUsed)
{
	m_filterParam = filterParam;
	if(isVisible())
	{
		QDialog::hide();
		if(!m_filterParamModelIndex.isValid())
		{
			QDialog::show();
			emit started(this);
			return;
		}
	}
	ui.useFilterPB->setVisible(filterCanBeUsed);
	ui.closeChBox->setVisible(filterCanBeUsed);
	if(m_filterParamModelIndex.isValid())
	{
		if(filterCanBeUsed)
		{
			ui.closeChBox->show();
		}
		else
		{
			ui.closeChBox->hide();
		}
	}
	initData();
	QDialog::show();
	emit started(this);
}

FilterEditDialog* FilterEditDialog::createInstance(ICEPROC_NS::ImageProcInterface* proc,
	ICEPROC_NS::ImageWindowInterface* imageWindowInterface, ICECORE_NS::FilterItemModel* filterItemModel,
	bool createDialog, QWidget* parent, Qt::WindowFlags f)
{
	Q_ASSERT(proc);
	//TODO refac
	bool dbConnected = false;
	if(createDialog)
	{
		FilterEditDialog* dialog = createFilterEditDialogMap.take(proc->getDllName());
		delete dialog;
	}
	else
	{
		FilterEditDialog* dialog = createFilterEditDialogMap.value(proc->getDllName());
		if(dialog)
		{
			dbConnected = dialog->isDbConnected();
		}
	}
	FilterEditDialog* result = new FilterEditDialog(QModelIndex(), proc, imageWindowInterface,
		parent, f);
	if(createDialog)
	{
		createFilterEditDialogMap.insert(proc->getDllName(), result);
	}
	result->setWindowIcon(proc->getViewIcon());
	result->setModel(filterItemModel);
	result->onDatabaseConnectionChanged(dbConnected);
	return result;
}

FilterEditDialog* FilterEditDialog::createInstance(const QModelIndex& filterParamModelIndex,
	QWidget* parent, Qt::WindowFlags f)
{
	FilterEditDialog* result = createFilterEditDialogMap.value(
		ICECORE_NS::FilterItemModel::getFilterType(filterParamModelIndex));
	Q_ASSERT(result);
	if(ICECORE_NS::FilterItemModel::getFilterId(filterParamModelIndex))
	{
		result = result->createFilterEditDialog(filterParamModelIndex, parent, f);
	}
	return result;
}

FilterEditDialog* FilterEditDialog::getInstance(const QString& key, unsigned int filterParamId)
{
	FilterEditDialog* result = createFilterEditDialogMap.value(key);
	if(filterParamId && result)
	{
		result = result->getFilterEditDialog(filterParamId);
	}
	return result;
}

void FilterEditDialog::closeEvent(QCloseEvent* event)
{
	//bool b = isFilterParamChanged();
	setParent(0, Qt::Dialog);
	event->accept();
	emit finished(this);
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> FilterEditDialog::createFilterParam() const
{
	Q_ASSERT(m_proc && m_filterModel);
	std::tr1::shared_ptr<ICEODB_NS::FilterType> filterType =
		m_filterModel->getFilterType(m_proc->getDllName());
	Q_ASSERT(filterType.get());
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> result(new ICEODB_NS::FilterParam(
		ui.nameLE->text(), ui.descriptionTE->toPlainText(), filterType));
	result->setBackgroundView(ui.backgroundMaskViewWidget->getMaskView());

	std::vector<std::tr1::shared_ptr<ICEODB_NS::MaskParam> > maskParamVector;
	if(m_contentWidget)
	{
		for(int i=0; i<m_contentWidget->getMaskParamCount(); i++)
		{
			maskParamVector.push_back(m_contentWidget->getMaskParam(i));
		}
		for(int i=0; i<m_contentWidget->getChildFilterParamCount(); i++)
		{
			result->addChildParam(m_contentWidget->getChildFilterParam(i));
		}
	}
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterImage> > filterImageVector;
	filterImageVector.resize(maskParamVector.size());

	for(int i=0; i<ui.imageListWidgetLayout->count(); i++)
	{
		QLayoutItem* item = ui.imageListWidgetLayout->itemAt(i);
		if(item)
		{
			ImageInfoWidget* imageInfoWidget = dynamic_cast<ImageInfoWidget*>(item->widget());
			if(imageInfoWidget)
			{
				int index = imageInfoWidget->getIndex();
				if(index < filterImageVector.size())
				{
					const QImage& image = imageInfoWidget->getImage();
					if(!image.isNull())
					{
						filterImageVector[index] = std::tr1::shared_ptr<ICEODB_NS::FilterImage>(
							new ICEODB_NS::FilterImage(imageInfoWidget->getFilename(),
							QByteArray((const char*)image.constBits(), image.height()
							* image.bytesPerLine()), image.width(), image.height(),
							image.bytesPerLine(), image.format(), false));
					}
				}
			}
		}
	}
	for(int i=0; i<maskParamVector.size(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = maskParamVector[i];
		Q_ASSERT(maskParam.get());
		std::tr1::shared_ptr<ICEODB_NS::FilterImage> oldFilterImage = maskParam->getFilterImage();
		std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage = filterImageVector[i];
		if(filterImage.get())
		{
			if(oldFilterImage.get())
			{
				filterImage->setId(oldFilterImage->getId());
			}
			maskParam->setFilterImage(filterImage);
		}
		result->addMaskParam(maskParam);
	}
	return result;
}

bool FilterEditDialog::isFilterParamChanged() const
{
	if(!m_filterParamFromDb.get())
	{
		return false;
	}
	if(m_filterParamFromDb->getName() != ui.nameLE->text() ||
		m_filterParamFromDb->getDescription() != ui.descriptionTE->toPlainText())
	{
		return true;
	}
	std::tr1::shared_ptr<ICEODB_NS::MaskView> backgroundView = m_filterParamFromDb->getBackgroundView();
	if(backgroundView.get() && !backgroundView->equal(ui.backgroundMaskViewWidget->getMaskView().get()))
	{
		return true;
	}
	//std::tr1::shared_ptr<ICEODB_NS::FilterParam> result(new ICEODB_NS::FilterParam(
	//	ui.nameLE->text(), ui.descriptionTE->toPlainText(), filterType));
	//result->setBackgroundView(ui.backgroundMaskViewWidget->getMaskView());

	/*std::vector<std::tr1::shared_ptr<ICEODB_NS::MaskParam> > maskParamVector;
	if(m_contentWidget)
	{
		for(int i=0; i<m_contentWidget->getMaskParamCount(); i++)
		{
			maskParamVector.push_back(m_contentWidget->getMaskParam(i));
		}
		for(int i=0; i<m_contentWidget->getChildFilterParamCount(); i++)
		{
			result->addChildParam(m_contentWidget->getChildFilterParam(i));
		}
	}
	std::vector<std::tr1::shared_ptr<ICEODB_NS::FilterImage> > filterImageVector;
	filterImageVector.resize(maskParamVector.size());

	for(int i=0; i<ui.imageListWidgetLayout->count(); i++)
	{
		QLayoutItem* item = ui.imageListWidgetLayout->itemAt(i);
		if(item)
		{
			ImageInfoWidget* imageInfoWidget = dynamic_cast<ImageInfoWidget*>(item->widget());
			if(imageInfoWidget)
			{
				int index = imageInfoWidget->getIndex();
				if(index < filterImageVector.size())
				{
					const QImage& image = imageInfoWidget->getImage();
					if(!image.isNull())
					{
						filterImageVector[index] = std::tr1::shared_ptr<ICEODB_NS::FilterImage>(
							new ICEODB_NS::FilterImage(imageInfoWidget->getFilename(),
							QByteArray((const char*)image.constBits(), image.height() * image.bytesPerLine()),
							image.width(), image.height(), image.bytesPerLine(), image.format(), false));
					}
				}
			}
		}
	}
	for(int i=0; i<maskParamVector.size(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = maskParamVector[i];
		Q_ASSERT(maskParam.get());
		std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage = filterImageVector[i];
		if(filterImage.get())
		{
			maskParam->setFilterImage(filterImage);
		}
		result->addMaskParam(maskParam);
	}*/
	return false;
}

void FilterEditDialog::onDatabaseConnectionChanged(bool connected)
{
	m_databaseConnected = connected;
	nameLE_textChanged(ui.nameLE->text());
	emit databaseConnectionChanged(connected);
}

void FilterEditDialog::onOpenDialogFinished(FilterEditDialog* dialog)
{
	Q_ASSERT(dialog);
	unsigned int id = dialog->getFilterParamId();
	m_openFilterEditDialogMap.remove(id);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
}

void FilterEditDialog::onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	bool flag = false;
	if(m_contentWidget)
	{
		flag = m_contentWidget->isCanUsed(imageBuilder);
	}
	setUseFilterPB(flag);
	emit currentImageBuilderChanged(imageBuilder);
}

void FilterEditDialog::nameLE_textChanged(const QString& text)
{
	bool enabled = !text.trimmed().isEmpty() && m_databaseConnected;
	ui.saveFilterToFilePB->setEnabled(enabled);
	ui.saveFilterPB->setEnabled(enabled);
}

void FilterEditDialog::showImagePB_clicked(bool checked)
{
	if(checked)
	{
		ui.showImagePB->setText(tr("Скрыть >>"));
		ui.imageScrollArea->show();
		setMaximumWidth(16777215);
		setMinimumWidth(m_contentWidgetWidth + IMAGE_WIDGET_WIDTH + ui.mainVerticalLayout->contentsMargins().left() +
			ui.mainVerticalLayout->contentsMargins().right());
		QRect rect = geometry();
		setGeometry(rect.x() - m_imageWidgetWidth, rect.y(),
			m_contentWidgetWidth + m_imageWidgetWidth + ui.mainVerticalLayout->contentsMargins().left() +
			ui.mainVerticalLayout->contentsMargins().right(), rect.height());
	}
	else
	{
		ui.showImagePB->setText(tr("<< Больше"));
		ui.imageScrollArea->hide();
		QRect rect = geometry();
		setGeometry(rect.x() + rect.width() - m_contentWidgetWidth, rect.y(),
			m_contentWidgetWidth + ui.mainVerticalLayout->contentsMargins().left() +
			ui.mainVerticalLayout->contentsMargins().right(), rect.height());
		m_imageWidgetWidth = rect.width() - m_contentWidgetWidth;
		ui.paramWidget->setFixedWidth(m_contentWidgetWidth);
		int width = m_contentWidgetWidth + ui.mainVerticalLayout->contentsMargins().left() +
			ui.mainVerticalLayout->contentsMargins().right();
		setFixedWidth(width);
		ui.paramWidget->setFixedWidth(width - 60);
	}
}

void FilterEditDialog::useFilterPB_clicked()
{
	// Диалоговое окно отображения процесса применения фильтра
	class ProcessingDialog : public QDialog
	{
	public:
		ProcessingDialog(ProcessingWidget* processingWidget, QWidget* parent) : QDialog(parent)
		{
			setWindowTitle("Процесс обработки...");

			processingWidget->setParent(this);

			pBtn = new QPushButton(tr("Готово"), this);
			connect(pBtn, SIGNAL(pressed()), this, SLOT(accept()));
			pBtn->setEnabled(false);

			chBx = new QCheckBox(tr("Закрыть по завершению"), this);

			setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
			layout()->addWidget(processingWidget);
			QBoxLayout* hL = new QBoxLayout(QBoxLayout::LeftToRight);
			hL->addWidget(chBx);
			hL->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
			hL->addWidget(pBtn);
			layout()->addItem(hL);
			resize(processingWidget->size());

			// Восстанавливаем настройки обработки
			ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
			ICECORE_NS::ProcessingSettings processingSettings = settings->getProcessingSettings();
			chBx->setChecked(processingSettings.isCloseAfterFinish);
		}
		~ProcessingDialog()
		{
			// Сохраняем настройки обработки
			ICECORE_NS::ProcessingSettings processingSettings;
			processingSettings.isCloseAfterFinish = chBx->isChecked();
			ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
			settings->setProcessingSettings(processingSettings);
		}
		QPushButton* pBtn; ///< Кнопка "Готово"
		QCheckBox* chBx; ///< Чекбокс "Закрыть по завершению"
	};

	if(m_contentWidget && !m_contentWidget->prepareFilter())
	{
		return;
	}

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam = createFilterParam();
	Q_ASSERT(filterParam);

	/// Отображаем процесс примененения фильтра
	ProcessingWidget* processingWidget = new ProcessingWidget(filterParam);
	ProcessingDialog* processingDialog = new ProcessingDialog(processingWidget, this);

	connect(m_proc->getQObject(), SIGNAL(maskProgressRangeChanged(int, int)),
		processingWidget, SLOT(onMaskProgressRangeChanged(int, int)));
	connect(m_proc->getQObject(), SIGNAL(maskProgressValueChanged(int)),
		processingWidget, SLOT(onMaskProgressValueChanged(int)));
	connect(m_proc->getQObject(), SIGNAL(maskProgressTextChanged(const QString&)),
		processingWidget, SLOT(onMaskProgressTextChanged(const QString&)));

	connect(m_proc->getQObject(), SIGNAL(filterCalculated(int, const QString&)),
		processingWidget, SLOT(onFilterCalculated(int, const QString&)));
	processingDialog->show();
	processingDialog->repaint(); /// Чтобы диалоговое окно сразу появилось на экране
	
	for(int i=0; i<filterParam->getChildParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> childFilterParam =
			ICECORE_NS::Core::Instance()->getFilterItemModel()->getChildFilterParam(filterParam, i);
		if(childFilterParam.get())
		{
			for(int j=0; j<childFilterParam->getMaskParamCount(); j++)
			{
				ICECORE_NS::Core::Instance()->getFilterItemModel()->getMaskParam(childFilterParam, j);
			}
		}
	}
	
	emit filterParamCreated(filterParam);
	// Задерживаем отображение диалогового окна процесса обработки, чтобы его закрыл пользователь
	processingDialog->pBtn->setEnabled(true);
	if(processingDialog->chBx->isChecked())
	{
		processingDialog->show();
	}
	else
	{
		processingDialog->exec();
	}
	delete processingDialog;

	if(ui.closeChBox->checkState() != Qt::Unchecked)
	{
		QDialog::accept();
	}
}

void FilterEditDialog::saveFilterToFilePB_clicked()
{
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	QString filename =  QFileDialog::getSaveFileName(this, tr("Сохранить фильтр как..."),
		settings->getFilterParamSaveDir(), tr("Файл фильтра (*.ifl)"));
	if(filename.isEmpty())
	{
		return;
	}
	QFileInfo fileInfo(filename);
	settings->setFilterParamSaveDir(fileInfo.absolutePath());
	ICECORE_NS::Core::Instance()->saveFilterParam(filename, createFilterParam());
}

void FilterEditDialog::saveFilterPB_clicked()
{
	Q_ASSERT(m_filterModel && m_proc);

	bool simpleSave = false;
	if(m_filterParamModelIndex.isValid())
	{
		int ret = QMessageBox::question(this, m_proc->getViewName(),
			tr("Изменить параметры фильтра. Выберите действие?"), tr("Сохранить"),
			tr("Выбрать раздел"), tr("Отмена"));
		if(ret == 0)
		{
			simpleSave = true;
		}
		else if(ret == 2)
		{
			return;
		}
	}
	QModelIndex parentModelIndex;
	QModelIndex duplicateModelIndex;

	if(simpleSave)
	{
		if(m_filterParamModelIndex.isValid())
		{
			duplicateModelIndex = m_filterParamModelIndex;
			parentModelIndex = m_filterParamModelIndex.parent();
		}
	}
	else
	{
		bool rightSectionSelected = false;
		while(!rightSectionSelected)
		{
			SectionSelectDialog selectDialog(this);
			selectDialog.setModel(m_filterModel);
			if(selectDialog.exec() == QDialog::Accepted)
			{
				parentModelIndex = selectDialog.getCurrentModelIndex();
				duplicateModelIndex = m_filterModel->getModelIndexOfDuplicateFilterName(
					ui.nameLE->text(),
					ICECORE_NS::FilterItemModel::getFilterId(m_filterParamModelIndex),
					m_proc->getDllName(), parentModelIndex);
				if(duplicateModelIndex.isValid())
				{
					int ret = QMessageBox::question(this, m_proc->getViewName(),
						tr("Фильтр с указанным именем уже существует в выбранном разделе.\r\n") +
						tr("Выберите действие?"), tr("Сохранить"), tr("Выбрать раздел"),
						tr("Нет"));
					if(ret == 0)
					{
						rightSectionSelected = true;
					}
					if(ret == 2)
					{
						return;
					}
				}
				else
				{
					rightSectionSelected = true;
				}
			}
			else
			{
				return;
			}
		}
	}
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam = createFilterParam();
	Q_ASSERT(filterParam);
	try
	{
		bool createdFilterParam = false;
		if(duplicateModelIndex.isValid())
		{
			m_filterModel->removeParam(duplicateModelIndex);
		}
		/*if(m_filterParamModelIndex.isValid())
		{
			if(parentModelIndex.isValid() && m_filterParamModelIndex.parent() != parentModelIndex)
			{
				m_filterParamModelIndex = m_filterModel->addParam(filterParam, parentModelIndex);
				createdFilterParam = true;
			}
			else
			{
				std::tr1::shared_ptr<ICEODB_NS::FilterParam> editFilterParam =
					m_filterModel->getFilterParam(m_filterParamModelIndex);
				Q_ASSERT(editFilterParam.get());
				editFilterParam->setName(filterParam->getName());
				editFilterParam->setDescription(filterParam->getDescription());
				editFilterParam->setBackgroundView(filterParam->getBackgroundView());
				m_filterModel->editParam(editFilterParam);
			}
		}
		else
		{*/
			m_filterParamModelIndex = m_filterModel->addParam(filterParam, parentModelIndex);
			createdFilterParam = true;
		//}
		QMessageBox::information(this, m_proc->getViewName(),
			tr("Фильтр успешно сохранен в базе данных"));

		ICEGUI_NS::FilterEditDialog* editDialog = createInstance(m_filterParamModelIndex,
			0, Qt::Dialog);
		if(editDialog)
		{
			editDialog->setUseFilterPB(ui.useFilterPB->isEnabled());
			editDialog->show();
		}
		if(createdFilterParam)
		{
			m_filterParamModelIndex = QModelIndex();
			close();
		}
	}
	catch(const ICECORE_NS::DataBaseException& exc)
	{
		QMessageBox::warning(this, m_proc->getViewName(),
			tr("Ошибка работы с базой данных\r\n") +
			QString::fromStdString(exc.getErrorDescription()));
		return;
	}
}

void FilterEditDialog::onMaskCreated(int index, const QString& name)
{
	if(m_imageWindowInterface)
	{
		QString filename = m_imageWindowInterface->getCurrentFilename();
		QImage image = m_imageWindowInterface->getCurrentSelectedImage();
		if(!image.isNull())
		{
			setImage(index, name, filename, image);
		}
	}
}

void FilterEditDialog::onMaskRemoved(int index)
{
	setImage(index, "", "", QImage());
}

void FilterEditDialog::initSize()
{
	adjustSize();
	QRect rect = geometry();
	ui.mainFrame->adjustSize();
	QRect mainFramRect = ui.mainFrame->geometry();
	bool desktopUsed = false;
	QDesktopWidget* desktopWidget = QApplication::desktop();
	if(desktopWidget)
	{
		QSize screenSize = desktopWidget->screenGeometry().size();
		if(screenSize.width() > 0 && screenSize.height() > 0)
		{
			int x = (screenSize.width() - m_contentWidgetWidth - IMAGE_WIDGET_WIDTH) / 2 +
				IMAGE_WIDGET_WIDTH;
			int h = mainFramRect.height() + ADDED_HEIGHT;
			if(h > screenSize.height() - UP_BORDER - DOWN_BORDER)
			{
				h = screenSize.height() - UP_BORDER - DOWN_BORDER;
			}
			int y = (screenSize.height() - h) /2;
			rect = QRect(x, y, m_contentWidgetWidth, h);
			desktopUsed = true;
		}
	}
	if(!desktopUsed)
	{
		int x = IMAGE_WIDGET_WIDTH;
		int y = 0;
		if(rect.x() > IMAGE_WIDGET_WIDTH)
		{
			x = rect.x();
		}
		if(rect.y() > 0)
		{
			y = rect.y();
		}
		rect = QRect(x, y, rect.width(), rect.height());
	}
	m_imageWidgetWidth = IMAGE_WIDGET_WIDTH;
	setGeometry(rect);
}

} // namespace ICEGUI_NS
