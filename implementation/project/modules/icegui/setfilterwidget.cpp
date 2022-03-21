#include "setfilterwidget.h"

#include "filtermanagedialog.h"
#include "objecttablewidget.h"

#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QProgressBar>

#include <iceimage/gdalimagebuilder.h>
#include <iceimage/contouritemmodel.h>

#include <iceodb/odb/filterparam.hxx>

#include <iceproc/imageprocinterface.h>

#include <icecore/settings.h>
#include <icecore/core.h>
#include <icecore/filteritemmodel.h>
#include <icecore/global.h>

namespace ICEGUI_NS
{

bool SetFilterWidget::forAllDestinationDir = false;
bool SetFilterWidget::isForce = false;

QStringList SetFilterWidget::selectDir(const QStringList& data, QWidget* parent)
{
	QFileDialog dialog(parent, tr("����� ����������"), data[0]);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setNameFilters(QStringList(ICECORE_NS::imageFileSuffix) <<
		QStringList(ICECORE_NS::imageFileSuffix).join(" "));
	dialog.selectNameFilter(data[1]);
	if(dialog.exec() == QDialog::Accepted)
	{
		return QStringList() << dialog.directory().absolutePath() << dialog.selectedNameFilter();
	}
	return data;
}

void SetFilterWidget::deleteDir(const QDir& dir)
{
	QDir localDir(dir);
	if(isDirEmpty(localDir))
	{
		localDir.rmdir(localDir.absolutePath());
	}
}

bool SetFilterWidget::isDirEmpty(const QDir& dir)
{
	return 2 >= dir.count(); // 1 - ".", 2 - ".." - ������ ���� � ��������
}

QStringList SetFilterWidget::selectDestinationDir(const QStringList& data, QWidget* parent)
{
	QStringList dir = selectDir(data);
	if(dir[0].isNull() || ICECORE_NS::NotSet.toLatin1() == dir[0] || data[0] == dir[0])
	{
		return data;
	}
	QDir d(dir[0]);
	// ���� ������� �������� �����
	if(!isDirEmpty(d))
	{
		if(QMessageBox::warning(parent, tr("�������� ��������� ..."),
			QObject::tr("��������� ������� �������� ����� �(���) �����������.\n\n����������?"),
			QObject::tr("��"), QObject::tr("���"), QString(), 1, 1) == 1)
		{
			return QStringList(QString::Null()) << data[1]; // ������������ ��������� �� �������� ���������
		}
	}
	return dir;
}

int SetFilterWidget::copyFileToFile(const QString& filePathFrom, const QString& filePathTo,
	bool* force, QWidget* parent)
{
	// ���� ����� ���� � ����� ������ ��� ���������� � �������� ����� � ��
	// ������� ������������� ������������ �����������
	if(QFile::exists(filePathTo) && !*force)
	{
		if(QMessageBox::warning(parent, tr("�������� ��������� ..."),
			QObject::tr("������� ��� ������������ ������� ��� �������� ������ � ����� ������.\n\n������������?"),
			QObject::tr("��, ��� ����"), QObject::tr("��������"), QString(), 1, 1) == 1)
		{
			return -1; // �����: ��������
		}
		else
		{
			*force = true; // �����: ��, ��� ����
		}
	}
	QFile::remove(filePathTo); // ������� ���� �� ������ ������
	return QFile::copy(filePathFrom, filePathTo) ? 1 : 0;
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> SetFilterWidget::getFilterParam()
{
	ICEGUI_NS::FilterManageDialog dialog(QStringList(), 0);
	dialog.setWindowTitle(QObject::tr("�������� ��������"));
	dialog.setSelectEnabled(true);
	dialog.setModel(ICECORE_NS::Core::Instance()->getFilterItemModel());
	dialog.exec();
	return dialog.getCurrentFilterParam();
}

SetFilterWidget::SetFilterWidget(const ICECORE_NS::FilterSettings& filterSettings,
	QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f), m_analysisNum(0),
	m_sortedNum(0)
{
	ui.setupUi(this);

	// ��������� ��������� ��������
	m_sourceDir = filterSettings.sourceDir;
	ui.sourceDirLE->setText(m_sourceDir);
	m_filterParam = ICECORE_NS::Core::Instance()->getFilterItemModel()->getFilterParam(
		filterSettings.filterId);
	if(m_filterParam)
	{
		ui.filterNameLE->setText(m_filterParam->getName());
	}
	else
	{
		ui.filterNameLE->setText(ICECORE_NS::NotSet.toLatin1());
	}
	m_destinationDir = filterSettings.destinationDir;
	ui.destinationDirLE->setText(m_destinationDir);
	ui.paramSB->setValue(filterSettings.param);
	ui.useChB->setChecked(filterSettings.isUse);
	useChB_toggled(ui.useChB->isChecked());
	m_filter = filterSettings.filter;
	ui.extensionsL->setText(m_filter);

	// ����� ��������� ������� � highlight(bool) ��������
	setAutoFillBackground(true);
}

SetFilterWidget::~SetFilterWidget()
{

}

int SetFilterWidget::getAnalysisNum() const
{
	return m_analysisNum;
}

int SetFilterWidget::getSortedNum() const
{
	return m_sortedNum;
}

QString SetFilterWidget::getName() const
{
	Q_ASSERT(m_filterParam.get());
	return m_filterParam->getName();
}

bool SetFilterWidget::isSetFilter() const
{
	return m_filterParam.get();
}

void SetFilterWidget::highlight(bool on)
{
	QPalette palette(palette());
	if(on)
	{
		palette.setColor(QPalette::Window, QColor(Qt::green));
	}
	else
	{
		QPalette paletteApp = QApplication::palette();
		palette.setColor(QPalette::Window, paletteApp.color(QPalette::Window));
	}
	setPalette(palette);
	repaint();
}

ICECORE_NS::FilterSettings SetFilterWidget::currentFilterSetting() const
{
	ICECORE_NS::FilterSettings filterSettings;
	filterSettings.destinationDir = ui.destinationDirLE->text();
	if(m_filterParam)
	{
		filterSettings.filterId = m_filterParam->getId();
	}
	else
	{
		filterSettings.filterId = 0;
	}
	filterSettings.isUse = ui.useChB->isChecked();
	filterSettings.param = ui.paramSB->value();
	filterSettings.sourceDir = ui.sourceDirLE->text();
	filterSettings.filter = ui.extensionsL->text();
	return filterSettings;
}

bool SetFilterWidget::createDestinationDir(const QString& externalDestinationDir)
{
	// ������ ������ ���� ������������ � �������� ���������
	Q_ASSERT(isUse());
	Q_ASSERT(m_filterParam.get());
	QDir dir;
	// ���� �� ������ ������� �������, �� ���� ������������ �������, ��������� 
	// ������������� ��� ����� �������
	if(externalDestinationDir.isNull())
	{
		if(ui.destinationDirLE->text() == ICECORE_NS::NotSet.toLatin1())
		{
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������: ") + m_filterParam->getName() + tr("\n������� "
				"������������ ������� �� ����������.\n\n��������� ����� ��������!"),
				tr("��������"));
			return false;
		}
		dir.setPath(ui.destinationDirLE->text());
		m_isExternalDistDir = false;
	}
	else
	{
		// ���� ������� ������� ������ (�������� ������ ��� ���� ��������). � ���� ������ ����
		// ��������� � ���� �������� ������� � ������ ������� �������
		dir.setPath(externalDestinationDir + QDir::separator() + m_filterParam->getName());
		m_isExternalDistDir = true;
	}
	if(dir.mkpath(dir.path()))
	{
		m_destinationDir = dir.path(); // ���������� ������� ��� ������������ �������
		return true;
	}
	else
	{
		QMessageBox::warning(this, tr("�������� ��������� ..."),
			tr("������: ") + m_filterParam->getName() + tr("\n������ �������� "
			"��������\n������������ �������.\n\n��������� ����� ��������!"),
			tr("��������"));
		return false;
	}
}

bool SetFilterWidget::isUse() const
{
	return ui.useChB->isChecked();
}

void SetFilterWidget::setIsUseExternalSourceDir(bool on)
{
	ui.sourceDirLE->setEnabled(!on);
	ui.extensionsL->setEnabled(!on);
}

void SetFilterWidget::setIsUseExternalDestinationDir(bool on)
{
	ui.destinationDirLE->setEnabled(!on);
}

void SetFilterWidget::setIsUse(bool on)
{
	ui.useChB->setChecked(on);
}

void SetFilterWidget::setFilterNum(int num)
{
	ui.filterNumL->setText(QString::number(num));
}

void SetFilterWidget::setSourceDir(const QStringList& externalSourceDir)
{
	// ���� ������ ������� �������� ������� ��� �������, ���������� ������������ ���, �����
	// ������������ ������� ��������� �������������
	m_sourceDir = externalSourceDir[0];
	m_filter = externalSourceDir[1];
	m_isExternalSourceDir = true;
	if(m_sourceDir.isNull())
	{
		m_sourceDir = ui.sourceDirLE->text();
		m_filter = ui.extensionsL->text();
		m_isExternalSourceDir = false;
	}
}

bool SetFilterWidget::checkOnValidate()
{
	if(!m_isExternalSourceDir)
	{
		// �������� ������� ��� ������� ������ ������������. ������������� ������������ �� ����������
		// ������� � ������ �������� ������ �� �����, ��� ��� ��� ����� ��������� ������ � ��������
		// �������� ���������, ���� ������ ������� ��� ������� ������� �������� ��������� ��� 
		// ��������������� �������.
		if(m_sourceDir == ICECORE_NS::NotSet)
		{
			Q_ASSERT(m_filterParam.get());
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������: ") + m_filterParam->getName() + tr("\n������� ��� "
				"�������� ������� �� ����������.\n\n��������� ����� ��������!"),
				tr("��������"));
			return false;
		}
		QDir sourceDir(m_sourceDir);
		if(!sourceDir.exists())
		{
			Q_ASSERT(m_filterParam.get());
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������: ") + m_filterParam->getName() + 
				tr("\n��������� ������� ��� �������� ������� �� ����������.\n\n") + 
				tr("���������� ���������� �������!"),
				tr("��������"));
			return false;
		}
	}
	if(!m_isExternalDistDir)
	{
		// ������� ��� ��������������� ������� ������ ������������. ���� �� ��� �����
		// ������ ������� ��� ��������������� �������, ������� ��� ��������������� �������
		// ����� ���, ��� ����� ��� ������������, ���� ������ ������� ��� �������, �� �������
		// ��� ��������������� ������� ����� ��������� ������ ������� �������� ��� ������
		// ������� �������
		const QDir destinationDir(m_destinationDir);
		if(!destinationDir.exists())
		{
			Q_ASSERT(m_filterParam.get());
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������: ") + m_filterParam->getName() + 
				tr("\n��������� ������� ��� ������������ ������� �� ����������.\n\n") +
				tr("���������� ���������� �������!"), tr("��������"));
			return false;
		}

		// ������������� ������������ � ������� ������ � �������� ��� ��������������� �������
		if(!isDirEmpty(destinationDir) && !forAllDestinationDir)
		{
			Q_ASSERT(m_filterParam.get());
			int res = QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������: ") + m_filterParam->getName() + 
				tr("\n��������� ������� ��� ������������ ������� �������� ����� �(���) �����������.\n\n") +
				tr("����������?"), tr("��, ��� ����"), tr("��"), tr("���"), 2, 2);
			if(res == 2) // "���"
			{
				return false;
			}
			else if(res == 0) // "��, ��� ����"
			{
				forAllDestinationDir = true;
			}
		}
	}
	return true;
}

bool SetFilterWidget::batchProccess(QProgressBar* progressBar)
{
	// ������ ������ ���� ������������ � �������� ���������
	Q_ASSERT(isUse());

	// ������ ������ ���� ����������
	// � ���� ������������ �������� ������ ���� ����������� ���� �������
	if(!m_filterParam)
	{
		QMessageBox::warning(this, tr("�������� ��������� ..."),
			tr("������ �� ����������.\n\n") + 
			tr("��������� ������������� ������� ��� ���������� ���!"),
			tr("��������"));
		return false;
	}

	// �������� �� ������������ ������������� ���� ������
	if(!checkOnValidate())
	{
		return false;
	}
	m_analysisNum = 0;	// ���-�� ������������������ ������
	m_sortedNum = 0;	// ���-�� ��������������� ������

	QStringList cutUpFileTypeList;
	cutUpFileTypeList << "*.icl"; //TODO refac
	QDir srcDir(m_sourceDir);
	QStringList cutUpFileList = srcDir.entryList(cutUpFileTypeList, QDir::Files);
	foreach(QString file, cutUpFileList)
	{
		QFileInfo fileInfo(srcDir, file);
		QDir dir(m_destinationDir);
		QString destFilePath = dir.absoluteFilePath(fileInfo.completeBaseName() + "." +
			fileInfo.suffix());
		copyFileToFile(fileInfo.absoluteFilePath(), destFilePath, &isForce, this);
	}
	// ���� �� ���� ������� � �������� ��������
	QStringList listFiles = srcDir.entryList(m_filter.split(" "), QDir::Files);
	Q_ASSERT(progressBar);
	progressBar->setRange(0, listFiles.count());
	int currentFileNum = 0;
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
	ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();

	QList<ICEIMAGE_NS::ContourItemModel*> summaryContourItemModelList;
	QStringList imageFileNameList;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder;
	foreach(QString file, listFiles)
	{
		progressBar->setValue(++currentFileNum);

		// �� ������������ �� �����, ������� ����� ������� ������������ ������, �������������
		// � ������� ������������ ������ (��� ����� ���� ���� � ������ ��������� �������� ��� 
		// ������� ������������ ������������ ������� �� ������� �������)
		QFileInfo fileInfo(file);
		if(fileInfo.completeSuffix().section('.', 0, 0) == ICECORE_NS::prefOriginalFile)
		{
			continue;
		}
		// ������ ���� � �����
		const QString filePath = srcDir.absoluteFilePath(file);

		// �������������� ����� ������� (�����)
		prepFilterMask();

		// ���������� ����� ������� � ������ (�����) � ���������
		// �������� �������� ����������
		double value = applyFilterMask(filePath, &imageBuilder);
		if(value == -1.0) // ������ �� �������� � ������� ������
		{
			Q_ASSERT(m_filterParam.get());
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("��������� ������: ") + m_filterParam->getName() +
				tr(" �� �������� � ������ �������.\n\n��������� ����� ��������!"),
				tr("��������"));
			return false;
		}
		Q_ASSERT(imageBuilder);
		// ���������� ����������
		if(value >= ui.paramSB->value() && !imageBuilder.isNull())
		{
			// ��������� � ������ �����
			//TODO fix
			insertTime(imageBuilder);

			// �������� ������������ ������ � ������� ������������ ������� �
			// ������������ ������ � �������� ������������ �������
			int res = copyToDistDir(filePath, imageBuilder);

			ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
			Q_ASSERT(settings);
			ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
			if(imageAddedSettings.objectTableFileCreateAuto)
			{
				QString fileCsvPathError = ObjectTableWidget::createCsvFiles(
					fileInfo.absoluteFilePath(), m_destinationDir, imageBuilder);
				if(!fileCsvPathError.isEmpty())
				{
					Q_ASSERT(m_filterParam.get());
					if(QMessageBox::warning(this, tr("�������� ��������� ..."),
						tr("������: ") + m_filterParam->getName() +
						tr("\n������ ������ ����� ������� ") + fileCsvPathError +
						tr("\n� ������� ������������ �������.\n\n����������?"),
						tr("��"), QObject::tr("���"), QString(), 1, 1) == 1)
					{
						return false;
					}
				}
			}
			if(imageAddedSettings.htmlFileCreateAuto)
			{
				if(!imageLineSettings.isUseUserParam)
				{
					imageLineSettings = settings->getImageLineSettingsDefault();
				}
				QString fileHtmlPathError = ObjectTableWidget::createHtmlFile(
					QStringList() << fileInfo.absoluteFilePath(), m_destinationDir,
					imageBuilder, imageLineSettings.unit, imageLineSettings.ratio,
					imageAddedSettings.halfArcLength);
				imageFileNameList.append(fileInfo.absoluteFilePath());
				int maskListSize = imageBuilder->getMaskListSize();
				if(summaryContourItemModelList.isEmpty())
				{
					for(int i=0; i<maskListSize; i++)
					{
						ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
						if(contourItemModel)
						{
							ICEIMAGE_NS::ContourItemModel* summaryContourItemModel =
								new ICEIMAGE_NS::ContourItemModel();
							summaryContourItemModelList.append(summaryContourItemModel);
							summaryContourItemModel->setTransformation(contourItemModel->getDivK(),
								contourItemModel->getUnit());
						}
						else
						{
							summaryContourItemModelList.append(0);
						}
					}
				}
				for(int i=0; i<maskListSize; i++)
				{
					ICEIMAGE_NS::ContourItemModel* contourItemModel = imageBuilder->getContourItemModel(i);
					if(contourItemModel)
					{
						ICEIMAGE_NS::ContourItemModel* summaryContourItemModel = summaryContourItemModelList[i];
						Q_ASSERT(summaryContourItemModel);
						double divK = contourItemModel->getDivK();
						summaryContourItemModel->addSumData(contourItemModel->getObjectCount(),
							contourItemModel->getObjectAreaSum() * divK * divK,
							contourItemModel->getArcLengthSum() * divK,
							contourItemModel->getSkippedAreaSum() * divK * divK,
							contourItemModel->getImagePointCount());
					}
				}

				if(!fileHtmlPathError.isEmpty())
				{
					Q_ASSERT(m_filterParam.get());
					if(QMessageBox::warning(this, tr("�������� ��������� ..."),
						tr("������: ") + m_filterParam->getName() +
						tr("\n������ ������ ����� ������ ") + fileHtmlPathError +
						tr("\n� ������� ������������ �������.\n\n����������?"),
						tr("��"), QObject::tr("���"), QString(), 1, 1) == 1)
					{
						return false;
					}
				}
			}
			if(res == -1) // ������������ ������� �����������
			{
				return false;
			}
			else if(res == 0) // ������ ����������� �����
			{
				//TODO refac
				QDir distDir(m_destinationDir);
				QFileInfo distFileInfo(filePath);
				Q_ASSERT(m_filterParam.get());
				if(QMessageBox::warning(this, tr("�������� ��������� ..."),
					tr("������: ") + m_filterParam->getName() +
					tr("\n������ ������ ����� ") + distFileInfo.absoluteFilePath() +
					tr("\n� ������� ������������ �������.\n\n����������?"),
					tr("��"), QObject::tr("���"), QString(), 1, 1) == 1)
				{
					return false;
				}
			}
			m_sortedNum++;
		}
		m_analysisNum++;
	}
	if(!imageBuilder.isNull() && imageAddedSettings.htmlFileCreateAuto)
	{
		ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
		Q_ASSERT(settings);
		ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
		ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
		if(!imageLineSettings.isUseUserParam)
		{
			imageLineSettings = settings->getImageLineSettingsDefault();
		}
		QString fileHtmlPathError = ObjectTableWidget::createHtmlFile(
			imageFileNameList, m_destinationDir, imageBuilder,
			imageLineSettings.unit, imageLineSettings.ratio,
			imageAddedSettings.halfArcLength, summaryContourItemModelList);
		if(!fileHtmlPathError.isEmpty())
		{
			Q_ASSERT(m_filterParam.get());
			if(QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������: ") + m_filterParam->getName() +
				tr("\n������ ������ ����� ���������� ������ ") + fileHtmlPathError +
				tr("\n� ������� ������������ �������.\n\n����������?"),
				tr("��"), QObject::tr("���"), QString(), 1, 1) == 1)
			{
				return false;
			}
		}
	}
	for(int i=0;i<summaryContourItemModelList.size();i++)
	{
		delete summaryContourItemModelList[i];
		summaryContourItemModelList[i] = 0;
	}
	return true;
}

/** ���������� ����� ��� �������.*/
void SetFilterWidget::prepFilterMask()
{
	Q_ASSERT(m_filterParam.get());
	// ����� �����
	for(int i=0; i<m_filterParam->getMaskParamCount(); i++)
	{
		ICECORE_NS::Core::Instance()->getFilterItemModel()->getMaskParam(m_filterParam, i);
	}
	for(int i=0; i<m_filterParam->getChildParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> childFilterParam =
			ICECORE_NS::Core::Instance()->getFilterItemModel()->getChildFilterParam(m_filterParam, i);
		if(childFilterParam.get())
		{
			for(int j=0; j<childFilterParam->getMaskParamCount(); j++)
			{
				ICECORE_NS::Core::Instance()->getFilterItemModel()->getMaskParam(childFilterParam, j);
			}
		}
	}
}

double SetFilterWidget::applyFilterMask(const QString& filePath, QSharedPointer<ICEIMAGE_NS::ImageBuilder>* imageBuilder)
{
	// ��������� ������ � ����������� �����������
	*imageBuilder = QSharedPointer<ICEIMAGE_NS::ImageBuilder>(new ICEIMAGE_NS::GdalImageBuilder());
	(*imageBuilder)->setSync(true);
	if((*imageBuilder)->load(filePath, true))
	{
		// �����
		Q_ASSERT(!(*imageBuilder).isNull() && m_filterParam.get() && ICECORE_NS::Core::Instance());
		ICEPROC_NS::ImageProcInterface* proc = ICECORE_NS::Core::Instance()->getImageProc(
			m_filterParam->getFilterType()->getDllName());
		if(proc)
		{
			ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
			Q_ASSERT(settings);
			ICECORE_NS::ImageLineSettings imageLineSettings = settings->getImageLineSettings();
			if(!imageLineSettings.isUseUserParam)
			{
				imageLineSettings = settings->getImageLineSettingsDefault();
			}
			proc->prepAndCalcMaskVector(*imageBuilder, m_filterParam, imageLineSettings.ratio,
				imageLineSettings.unit);
			ICEIMAGE_NS::ContourItemModel* contourItemModel = (*imageBuilder)->getContourItemModel(0);
			if(!contourItemModel)
			{
				return 0.0;
			}
			return contourItemModel->getObjectAreaPercent();
		}
		else
		{
			return -1.0;
		}
	}
	else
	{
		return -0.5;
	}
}

void SetFilterWidget::insertTime(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	QDateTime dateTime;
	imageBuilder->setAddedTextListToBottomRight(QStringList() <<
		QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	imageBuilder->apply();
}

int SetFilterWidget::copyToDistDir(const QString& filePath,
	const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder)
{
	Q_ASSERT(!imageBuilder.isNull());

	// ������� ��� ������������ �������
	QDir distDir(m_destinationDir);

	// �������� ������������ ������ � ������� ������������ ������� 
	QFileInfo fileInfo(filePath);
	ICECORE_NS::Settings* settings = ICECORE_NS::Core::Instance()->getSettings();
	Q_ASSERT(settings);
	ICECORE_NS::ImageAddedSettings imageAddedSettings = settings->getImageAddedSettings();
	if(!imageBuilder->save(distDir.absoluteFilePath(fileInfo.fileName()), imageAddedSettings.jpegQuality))
	{
		return 0;
	}
	// �������� �������� ������ � ������� ��� ������������ ������� ��������
	// ������� orig
	QString origFilePath = distDir.absoluteFilePath(fileInfo.completeBaseName() +
		"." + ICECORE_NS::prefOriginalFile + "." + fileInfo.completeSuffix());
	return copyFileToFile(filePath, origFilePath, &isForce, this);
}

void SetFilterWidget::sourceDirTB_pressed()
{
	QStringList sL = selectDir(QStringList(ui.sourceDirLE->text()) << m_filter);
	QString dir = sL[0];
	if(dir.isNull() || ICECORE_NS::NotSet.toLatin1() == dir)
	{
		return;
	}

	// ���������� �� ���������� �������, ������� ������ �������������� ������ ��������
	m_filter = sL[1];
	ui.extensionsL->setText(m_filter);

	//��� �������� �� ������� �������, ��� ��� ����� ��������� ��������
	//���������� ������� �������, ������� ����� ��������� ����� ��������� 
	//����� �������

	ui.sourceDirLE->setText(dir);
}

void SetFilterWidget::destinationDirTB_pressed()
{
	QStringList sL = selectDestinationDir(QStringList(ui.destinationDirLE->text())
		<< m_filter);
	QString dir = sL[0];

	if(dir.isNull())
	{
		return;
	}
	ui.destinationDirLE->setText(dir);
}

void SetFilterWidget::filterPB_pressed()
{
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam = getFilterParam();
	if(filterParam)
	{
		m_filterParam = filterParam;
		ui.filterNameLE->setText(m_filterParam->getName());
	}
}

void SetFilterWidget::useChB_toggled(bool checked)
{
	ui.filterNameLE->setEnabled(checked);
}

void SetFilterWidget::deleteDestinationDir()
{
	deleteDir(QDir(m_destinationDir));
}

} // namespace ICEGUI_NS
