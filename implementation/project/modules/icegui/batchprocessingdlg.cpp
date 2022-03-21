#include "batchprocessingdlg.h"

#include "ui_batchprocessingdlg.h"

#include "setfilterwidget.h"
#include "mapviewdialog.h"

#include <icecore/settings.h>
#include <icecore/global.h>

#include <QtGui/QMessageBox>

#include <QtCore/QDir>

double startLat = 78.59;
double startLon = 104.27;

double currentLat = startLat;
double currentLon = startLon;

namespace ICEGUI_NS
{

/**
* ����������, ���������� ������ ��� ���
//*/
//static bool isDirEmpty(const QDir& dir)
//{
//	return 2 >= dir.count(); // 1 - ".", 2 - ".." - ������ ���� � ��������
//}

/**
* ������� ����������, ���� ��� �� ������
* @param dir - �������, ��� ��������� �������� dirStr
*/
//static void deleteDir(const QDir& dr, const QString& dirStr)
//	{
//	QString dir = QFileDialog::getExistingDirectory(0, QObject::tr("����� ����������"),
//		beginDir);
//	}
//	{
//{
//	QDir dir(dr);
//	dir.cd(dirStr);
//  if(SetFilterWgt::isDirEmpty(dir))
//	{
//		dir.cd("..");
//		dir.rmdir(dirStr);
//	}
//	else
//	{
//		dir.cd("..");
//	}
//}
//		ui->sourceDirTlBtn->text());
//	{
//}

/** 
* �����������.
* \param settings - ����������� ��������� ��� �������� ���������
*/
BatchProcessingDlg::BatchProcessingDlg(ICECORE_NS::Settings* settings,
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_settings(settings)
{
	ui = new Ui::BatchProcessingDlg;
	ui->setupUi(this);

	m_mapViewDialog = new MapViewDialog(this);

	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("������"));

	connect(ui->sourceTlBtn, SIGNAL(pressed()), this, SLOT(sourceTlBtnSlot()));
	connect(ui->sourceGrpBx, SIGNAL(toggled(bool)), this, SLOT(sourceGrpBxSlot(bool)));
	connect(ui->distTlBtn, SIGNAL(pressed()), this, SLOT(distTlBtnSlot()));
	connect(ui->distGrpBx, SIGNAL(toggled(bool)), this, SLOT(distGrpBxSlot(bool)));
	connect(ui->addFilterTlBtn, SIGNAL(pressed()), this, SLOT(addFilterTlBtnSlot()));
	connect(ui->delFilterTlBtn, SIGNAL(pressed()), this, SLOT(delFilterTlBtnSlot()));
	connect(ui->useAllChBx, SIGNAL(toggled(bool)), this, SLOT(useAllChBxSlot(bool)));
	connect(ui->defaultPBtn, SIGNAL(pressed()), this, SLOT(defaultPBtnSlot()));
	connect(ui->beginPBtn, SIGNAL(pressed()), this, SLOT(beginPBtnSlot()));
	connect(ui->mapPBtn, SIGNAL(pressed()), this, SLOT(mapPBtnSlot()));

	init();
}

/** �������������. */
void BatchProcessingDlg::init()
{
	// ��������� �������� ���������
	ui->prBr->reset();
	Q_ASSERT(m_settings);

	//��������������� ��������� �������� ���������

	ICECORE_NS::BatchProcessingSettings batchProcessingSettings =
		m_settings->getBatchProcessingSettings();

	// ��������������� ��������� �������� �������� ���������
	QVector<ICECORE_NS::FilterSettings> filterSettingsVector =
		m_settings->getFilterSettings();
	for(int i=0; i<filterSettingsVector.size(); i++)
	{
		SetFilterWidget* filterWidget = new SetFilterWidget(filterSettingsVector[i], this);
		addSetFilterWidget(i - 1, filterWidget);
	}

	// �������� �������� ���������
	ui->sourceLnEdt->setText(batchProcessingSettings.sourceDir);
	ui->sourceGrpBx->setChecked(batchProcessingSettings.isUseSourceDir);
	sourceGrpBxSlot(batchProcessingSettings.isUseSourceDir);
	ui->distLnEdt->setText(batchProcessingSettings.destinationDir);
	ui->distGrpBx->setChecked(batchProcessingSettings.isUseDestinationDir);
	distGrpBxSlot(batchProcessingSettings.isUseDestinationDir);
	ui->extensionsLbl->setText(batchProcessingSettings.filter);
}

/** ����������. */
BatchProcessingDlg::~BatchProcessingDlg()
{
	m_mapViewDialog->close();
	delete m_mapViewDialog;
	if(result() != QDialog::Accepted)
	{
		return;
	}

	// ��������� ��������� �������� ���������

	// ������� ��� ���������������� ���������
	m_settings->removeBatchProcessingSettings();
	ICECORE_NS::BatchProcessingSettings batchProcessingSettings;
	// ��������
	batchProcessingSettings.sourceDir = ui->sourceLnEdt->text();
	batchProcessingSettings.isUseSourceDir = ui->sourceGrpBx->isChecked();
	batchProcessingSettings.destinationDir = ui->distLnEdt->text();
	batchProcessingSettings.isUseDestinationDir = ui->distGrpBx->isChecked();
	batchProcessingSettings.filter = ui->extensionsLbl->text();
	m_settings->setBatchProcessingSettings(batchProcessingSettings);

	// ��������� ��������� �������� �������� ���������
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		ICECORE_NS::FilterSettings filterSetting = m_filterWidgetVector[i]->currentFilterSetting();
		m_settings->setFilterSettings(filterSetting, i + 1);
		// �� ���� ������� ... �� �������� ������
		//delete m_filterWidgetVector[i];
	}
}

/** �������� ������.*/
void BatchProcessingDlg::addSetFilterWidget(int indexAfter, SetFilterWidget* widget)
{
	Q_ASSERT(widget);
	ui->verticalLayout->insertWidget(indexAfter + 1, widget);
	m_filterWidgetVector.insert(indexAfter + 1, widget);

	// ���������� ���� ��������� ��� �� ��������� ������ ��������� ��� �������� � 
	// ������������ �������
	widget->setIsUseExternalSourceDir(ui->sourceGrpBx->isChecked());
	widget->setIsUseExternalDestinationDir(ui->distGrpBx->isChecked());

	// ���������������� ������ ��������
	reconfFilterNum();
}

/** ������������������� ������ ��������.*/
void BatchProcessingDlg::reconfFilterNum()
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		m_filterWidgetVector[i]->setFilterNum(i+1);
	}
}

/** ���������� ������� ������ ������ ����� ��� �������� �������.*/
void BatchProcessingDlg::sourceTlBtnSlot()
{
	QStringList sL = SetFilterWidget::selectDir(QStringList(ui->sourceLnEdt->text()) <<
		ui->extensionsLbl->text());
	QString dir = sL[0];
	if(dir.isNull() || ICECORE_NS::NotSet.toLatin1() == dir)
	{
		return;
	}

	// ���������� �� ���������� �������, ������� ������ �������������� ���� ���������
	// ��� ������������� � �������� ��������� ������� �������� ��� �������� �������
	QString filter = sL[1];

	QDir d(dir);
	QStringList listFiles = d.entryList(filter.split(" "), QDir::Files);
	if(listFiles.count() == 0)
	{
		QMessageBox::warning(this, tr("�������� ��������� ..."),
			tr("��������� ������ ������� �� �������� �������.\n\n���������� ���������� �������!"),
			tr("��������"));
		return;
	}
	ui->sourceLnEdt->setText(dir);
	ui->extensionsLbl->setText(filter);
}

/** ���������� ������� ������ ������ ������� �������� ��� ������������ �������.*/
void BatchProcessingDlg::distTlBtnSlot()
{
	QStringList sL = SetFilterWidget::selectDestinationDir(QStringList(ui->distLnEdt->text()) <<
		ui->extensionsLbl->text());
	QString dir = sL[0];
	if(dir.isNull())
	{
		return;
	}
	ui->distLnEdt->setText(dir);
}

/** ���������� ��������� ������������� ������� �������� �������.*/
void BatchProcessingDlg::sourceGrpBxSlot(bool on)
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ���� ����������� ������������� ������� ��������, �� ���� 
		// ���������� ���� ���� � �������
		m_filterWidgetVector[i]->setIsUseExternalSourceDir(on);
	}
}

/** ���������� ��������� ������������� ������� �������� ��� ��������������� �������.*/
void BatchProcessingDlg::distGrpBxSlot(bool on)
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ���� ����������� ������������� ������� ��������, �� ���� 
		// ���������� ���� ���� � �������
		m_filterWidgetVector[i]->setIsUseExternalDestinationDir(on);
	}
}

/** ���������� ���������� �������.*/
void BatchProcessingDlg::addFilterTlBtnSlot()
{
	int filterNum = m_filterWidgetVector.size();
	for(int i=filterNum-1; i>=0; i--)
	{
		// ������� ������ ������������ ������ � �����
		SetFilterWidget* filterWidget = m_filterWidgetVector[i];
		Q_ASSERT(filterWidget);
		if(filterWidget->isUse())
		{
			// ������� ����� ������ � ����������� ���������� �������
			SetFilterWidget* widget = new SetFilterWidget(filterWidget->currentFilterSetting(), this);
			// ��������� ����� ������ ����� ����������
			addSetFilterWidget(i, widget);
			// ��������� ������ ���� ������
			return;
		}
	}
	if(filterNum != 0)
	{
		// ���� ������� ����, �� ��� �� ������ ������������� �������, �� ��������� ��������� ������
		// ������� ����� ������ � ����������� ���������� �������
		SetFilterWidget* widget = new SetFilterWidget(m_filterWidgetVector[m_filterWidgetVector.size() - 1]->currentFilterSetting(), this);
		// ��������� ����� ������ ����� ����������
		addSetFilterWidget(m_filterWidgetVector.size()-1, widget);
	}
	else
	{
		// ���� �������� ������ ���, �� ������� ������ � ����������� ������� ������� (������� �� ���������)
		QVector<ICECORE_NS::FilterSettings> filterSettingsVector = m_settings->getFilterSettings();
		SetFilterWidget* widget = new SetFilterWidget(filterSettingsVector[0], this);
		addSetFilterWidget(-1, widget);
	}
}

/** ���������� �������� �������.*/
void BatchProcessingDlg::delFilterTlBtnSlot()
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ��� ��������� �������
		SetFilterWidget* filterWidget = m_filterWidgetVector[i];
		Q_ASSERT(filterWidget);
		if(filterWidget->isUse())
		{
			ui->verticalLayout->takeAt(i);
			m_filterWidgetVector.remove(i);
			filterWidget->setParent(0);
			delete filterWidget;
			// ��������
			delFilterTlBtnSlot();
			// ���������������� ������ ��������
			reconfFilterNum();
			// ������� ��������� ������� �� ������
			return;
		}
	}
}

/** ���������� ������/�������� ���� ��������.*/
void BatchProcessingDlg::useAllChBxSlot(bool on)
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		m_filterWidgetVector[i]->setIsUse(on);
	}
}

/** �������� ��������� �������� � �� ���������� �� ������� ������� �������� ����������� ����.*/
void BatchProcessingDlg::defaultPBtnSlot()
{
	// �������� ��� �������
	useAllChBxSlot(true);
	// ������� ��� ���������� �������
	delFilterTlBtnSlot();
	// ��������������� ��������� �������� �������� ���������
	QVector<ICECORE_NS::FilterSettings> filterSettingsVector =
		m_settings->getFilterSettings();
	for(int i=0; i<filterSettingsVector.size(); i++)
	{
		SetFilterWidget* widget = new SetFilterWidget(filterSettingsVector[i], this);
		addSetFilterWidget(i - 1, widget);
	}
}

/** �������� ��������� ��� ������������ �������.*/ 
bool BatchProcessingDlg::createDistDirs()
{
	QString distDir = QString::null;
	// ���� ��� �������� ��������� ���� ������������� ������ ������� ������������ �������
	// �� ������� ���� �������
	if(ui->distGrpBx->isChecked())
	{
		distDir = ui->distLnEdt->text();
		if(ICECORE_NS::NotSet.toLatin1() == distDir)
		{
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������ ������� ������������ ������� �� ����������.\n\n��������� ����� ��������!"),
				tr("��������"));
			return false;
		}
		if(!QDir().mkpath(distDir))
		{
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������ �������� ������� ��������\n������������ �������.\n\n��������� ����� ��������!"),
				tr("��������"));
			return false;
		}
	}
	// ���� ����� ������ �������, �� ������ ������ ������ � ���� �������� ������� ���� �������
	// � �������� �������, ���� ������ �������� �� ���� ������������ (�� ����� QString::null), ��
	// ������ ������ ������ ������� ���� ������� ��� ��������������� �������, ������� ��� ����
	// ������ ������������
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ������
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// ����������� ��� ������ ���� �� ������������
		if(!widget->isUse())
		{
			continue;
		}
		// ���� distDir == QString::null, �� ������ ������� �� ���� ������������ � ������ ������ �������
		// ���� ������� ��� ��������������� �������, ��������� �������������. ���� ������ ������� ����
		// ������������, �� ������ ������ ������� � ���� �������� ���� ���������� ��� ���������������
		// ������� � ������ �������
		if(!widget->createDestinationDir(distDir))
		{
			return false;
		}
	}
	return true;
}

/** ��������� ��������� �������� ��� ���� ��������.*/ 
bool BatchProcessingDlg::setSourceDirs()
{
	QString sourceDir = QString::null;
	// ���� ��� �������� ��������� ���� ������������� ������ ������� �������� �������
	// �� ���������� ��� ��� ���� ��������, ���� ������ ������� �� ����� �� ������
	// ������ ����� ������������ ���� ������� ��� �������� �������, ������������� �������������
	if(ui->sourceGrpBx->isChecked())
	{
		sourceDir = ui->sourceLnEdt->text();
		if(ICECORE_NS::NotSet.toLatin1() == sourceDir)
		{
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("������ ������� �������� ������� �� ����������.\n\n��������� ����� ��������!"),
				tr("��������"));
			return false;
		}
	}
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ������
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// ����������� ��� ������ ���� �� ������������
		if(!widget->isUse())
		{
			continue;
		}
		widget->setSourceDir(QStringList(sourceDir) << ui->extensionsLbl->text());
	}
	return true;
}

/** �������� ���� ������������� ������ ��� ��������� �� ����������.*/ 
bool BatchProcessingDlg::checkOnValidates()
{
	// ������ ���� ������������ ���� �� ���� ������
	int i;
	for(i=0; i<m_filterWidgetVector.size(); i++)
	{
		if(m_filterWidgetVector[i]->isUse())
		{
			break;
		}
	}
	if(i == m_filterWidgetVector.size())
	{
		QMessageBox::warning(this, tr("�������� ��������� ..."),
			tr("� �������� ��������� �� ������������ �� ���� ������.\n\n") + 
			tr("������������ �������!"),
			tr("��������"));
		return false;
	}
	/* ������ �������� ��� ����� ��������������� � ������ ������ ...
	*  �.�. ��� �������� ���������� ���������� ��������� �������� ������� �
	*  ��������� ��������������� ������� ������ ���������
	*/
	//// ������ �������� ������� ��� ������� ������ ������������
	//// ���� �� ������������
	//if(ui->sourceGrpBx->isChecked())
	//{
	//	const QDir sourceDir(ui->sourceLnEdt->text());
	//	if(!sourceDir.exists())
	//	{
	//		QMessageBox::warning(this, tr("�������� ��������� ..."),
	//			tr("��������� ������ ������� ��� �������� ������� �� ����������.\n\n") + 
	//			tr("���������� ���������� �������!"),
	//			tr("��������"));
	//		return false;
	//	}
	//	// ������������� ������������ �� ����������� ������� � ������ �������� ��������
	//	if(!sourceDir.entryList(ICECORE_NS::filterImage, QDir::Files).count())
	//	{
	//		QMessageBox::warning(this, tr("�������� ��������� ..."),
	//			tr("��������� ������ ������� ��� �������� ������� �� �������� �������.\n\n") +
	//			tr("���������� ���������� �������!"),
	//			tr("��������"));
	//		return false;
	//	}
	//}
	//// ������ ������� ��� ��������������� ������� ������ ������������
	//// ���� �� ������������
	//if(ui->distGrpBx->isChecked())
	//{
	//	const QDir distDir(ui->distLnEdt->text());
	//	if(!distDir.exists())
	//	{
	//		QMessageBox::warning(this, tr("�������� ��������� ..."),
	//			tr("��������� ������ ������� ��� ������������ ������� �� ����������.\n\n") + 
	//			tr("���������� ���������� �������!"),
	//			tr("��������"));
	//		return false;
	//	}
	//	// ������������� ������������ � ������� ������ � �������� ��� ��������������� �������
	//	if(!SetFilterWgt::isDirEmpty(distDir))
	//	{
	//		if(QMessageBox::warning(this, tr("�������� ��������� ..."),
	//			tr("��������� ������ ������� ��� ������������ ������� �������� ����� �(���) �����������.\n\n") +
	//			tr("����������?"), tr("��"), tr("���"), QString(), 1, 1) == 1)
	//		{
	//			return false;
	//		}
	//	}
	//}
	// ��������� �� ���������� ��������� ���� �������� ������� 
	// ���������������� � �������� ��������� �������
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ������
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// ����������� ��� ������ ���� �� ������������
		if(!widget->isUse())
		{
			continue;
		}
		if(!widget->checkOnValidate())
		{
			return false;
		}
	}
	return true;
}

/** ���������� ������ "����������".*/ 
void BatchProcessingDlg::beginPBtnSlot()
{
	// � ���� ������������ �������� ������ ���� ����������� ���� �������
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ������
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// ����������� ��� ������ ���� �� ������������
		if(!widget->isUse())
		{
			continue;
		}
		if(!widget->isSetFilter())
		{
			QMessageBox::warning(this, tr("�������� ��������� ..."),
				tr("��� ������� �") + QString::number(i) + tr(" �� ���������� ������.\n\n") + 
				tr("��������� ������������� ������� ��� ���������� ���!"),
				tr("��������"));
			return;
		}
	}
	// ������������� ��� �������� ��� �������� �������
	if(!setSourceDirs())
	{
		return;
	}
	// ������� ��� �������� ��� ��������������� �������
	if(!createDistDirs())
	{
		return;
	}
	// �������� ������ �� ������������ ���� ������������� ������
	if(!checkOnValidates())
	{
		return;
	}
	// ������ �������� ������� ��� �������
	QString sourceDir = ui->sourceGrpBx->isChecked() ? ui->sourceLnEdt->text() : QString::null;
	// ������ ������� ��������������� �������
	QString distDir = ui->distGrpBx->isChecked() ? ui->distLnEdt->text() : QString::null;
	// �������� ��������� ��� ������� �������
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ������
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// ����������� ��� ������ ���� �� ������������
		if(!widget->isUse())
		{
			continue;
		}

		// ��������� ��������� �������� �������

		// ������������ ������ �������� �������
		widget->highlight(true);
		// ���������
		bool res = widget->batchProccess(ui->prBr);
		// ������� ��������� ������� �������� �������
		widget->highlight(false);
		if(!res)
		{
			return;
		}
		//// ���� ������� ��� ��������������� ������� ������� ������� ������, ������� ���
		//setFilterWgt->deleteDistDir();
	}

	//// ���� ������ ������� ��� ��������������� ������� ��� ������������ � ����, ������� ���
	//if(!distDir.isNull())
	//{
	//	SetFilterWgt::deleteDir(QDir(distDir));
	//}

	// ����������� ���������� ���������� ���������
	showResult();
}

/** ����������� ���������� �������� ���������.*/
void BatchProcessingDlg::showResult()
{
	// ��������� ������ ������ ����������� �������� ���������
	QString str;
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// ������� ������
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// ����������� ��� ������ ���� �� ������������
		if(!widget->isUse())
		{
			continue;
		}
		str += tr("<tr>"
			"<td align=center><big>") + QString::number(i + 1) + tr("</big></td>"
			"<td align=center><big>") + widget->getName() + tr("</big></td>"
			"<td align=center><big>") + QString::number(widget->getAnalysisNum()) + tr("</big></td>"
			"<td align=center><big>") + QString::number(widget->getSortedNum()) + tr("</big></td>"
			"</tr>");
	}
	QMessageBox::information(this, tr("�������� ��������� ..."),
		tr("<table border=1 cellpadding=5 cellspacing=1>"
		"<tr>"
		"<th><big>�</big></th>"
		"<th><big>������</big></th>"
		"<th><big>�������������</big></th>"
		"<th><big>�������</big></th>"
		"</tr>") + str + tr("</table>"), tr("��"));
}

void BatchProcessingDlg::mapPBtnSlot()
{
	Q_ASSERT(m_mapViewDialog);
	m_mapViewDialog->show();
}

} // namespace ICEGUI_NS
