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
* Определить, директория пустая или нет
//*/
//static bool isDirEmpty(const QDir& dir)
//{
//	return 2 >= dir.count(); // 1 - ".", 2 - ".." - всегда есть в каталоге
//}

/**
* Удалить директорию, если она не пустая
* @param dir - каталог, где удаляются каталоги dirStr
*/
//static void deleteDir(const QDir& dr, const QString& dirStr)
//	{
//	QString dir = QFileDialog::getExistingDirectory(0, QObject::tr("Выбор директории"),
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
* Конструктор.
* \param settings - сохраненные настройки для пакетной обработки
*/
BatchProcessingDlg::BatchProcessingDlg(ICECORE_NS::Settings* settings,
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_settings(settings)
{
	ui = new Ui::BatchProcessingDlg;
	ui->setupUi(this);

	m_mapViewDialog = new MapViewDialog(this);

	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));

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

/** Инициализация. */
void BatchProcessingDlg::init()
{
	// Индикатор процесса обработки
	ui->prBr->reset();
	Q_ASSERT(m_settings);

	//Восстанавливаем настройки пакетной обработки

	ICECORE_NS::BatchProcessingSettings batchProcessingSettings =
		m_settings->getBatchProcessingSettings();

	// Восстанавливаем настройки фильтров пакетной обработки
	QVector<ICECORE_NS::FilterSettings> filterSettingsVector =
		m_settings->getFilterSettings();
	for(int i=0; i<filterSettingsVector.size(); i++)
	{
		SetFilterWidget* filterWidget = new SetFilterWidget(filterSettingsVector[i], this);
		addSetFilterWidget(i - 1, filterWidget);
	}

	// Каталоги пакетной обработки
	ui->sourceLnEdt->setText(batchProcessingSettings.sourceDir);
	ui->sourceGrpBx->setChecked(batchProcessingSettings.isUseSourceDir);
	sourceGrpBxSlot(batchProcessingSettings.isUseSourceDir);
	ui->distLnEdt->setText(batchProcessingSettings.destinationDir);
	ui->distGrpBx->setChecked(batchProcessingSettings.isUseDestinationDir);
	distGrpBxSlot(batchProcessingSettings.isUseDestinationDir);
	ui->extensionsLbl->setText(batchProcessingSettings.filter);
}

/** Деструктор. */
BatchProcessingDlg::~BatchProcessingDlg()
{
	m_mapViewDialog->close();
	delete m_mapViewDialog;
	if(result() != QDialog::Accepted)
	{
		return;
	}

	// Сохраняем настройки пакетной обработки

	// Удаляем все пользовательские настройки
	m_settings->removeBatchProcessingSettings();
	ICECORE_NS::BatchProcessingSettings batchProcessingSettings;
	// Каталоги
	batchProcessingSettings.sourceDir = ui->sourceLnEdt->text();
	batchProcessingSettings.isUseSourceDir = ui->sourceGrpBx->isChecked();
	batchProcessingSettings.destinationDir = ui->distLnEdt->text();
	batchProcessingSettings.isUseDestinationDir = ui->distGrpBx->isChecked();
	batchProcessingSettings.filter = ui->extensionsLbl->text();
	m_settings->setBatchProcessingSettings(batchProcessingSettings);

	// Сохраняем настройки фильтров пакетной обработки
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		ICECORE_NS::FilterSettings filterSetting = m_filterWidgetVector[i]->currentFilterSetting();
		m_settings->setFilterSettings(filterSetting, i + 1);
		// Не надо удалять ... их родитель удалит
		//delete m_filterWidgetVector[i];
	}
}

/** Добавить фильтр.*/
void BatchProcessingDlg::addSetFilterWidget(int indexAfter, SetFilterWidget* widget)
{
	Q_ASSERT(widget);
	ui->verticalLayout->insertWidget(indexAfter + 1, widget);
	m_filterWidgetVector.insert(indexAfter + 1, widget);

	// Отображаем факт установки или не установки единых каталогов для исходных и 
	// обработанных снимков
	widget->setIsUseExternalSourceDir(ui->sourceGrpBx->isChecked());
	widget->setIsUseExternalDestinationDir(ui->distGrpBx->isChecked());

	// Переконфигурирем номера фильтров
	reconfFilterNum();
}

/** Переконфигурировать номера фильтров.*/
void BatchProcessingDlg::reconfFilterNum()
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		m_filterWidgetVector[i]->setFilterNum(i+1);
	}
}

/** Обработчик нажатия кнопки выбора папки для исходных снимков.*/
void BatchProcessingDlg::sourceTlBtnSlot()
{
	QStringList sL = SetFilterWidget::selectDir(QStringList(ui->sourceLnEdt->text()) <<
		ui->extensionsLbl->text());
	QString dir = sL[0];
	if(dir.isNull() || ICECORE_NS::NotSet.toLatin1() == dir)
	{
		return;
	}

	// Запоминаем те расширения снимков, который должны обрабатываться всем фильтрами
	// при использовании в пакетной обработке единого каталога для исходный снимков
	QString filter = sL[1];

	QDir d(dir);
	QStringList listFiles = d.entryList(filter.split(" "), QDir::Files);
	if(listFiles.count() == 0)
	{
		QMessageBox::warning(this, tr("Пакетная обработка ..."),
			tr("Выбранный единый каталог не содержит снимков.\n\nУстановите правильный каталог!"),
			tr("Прервать"));
		return;
	}
	ui->sourceLnEdt->setText(dir);
	ui->extensionsLbl->setText(filter);
}

/** Обработчик нажатия кнопки выбора единого каталога для обработанных снимков.*/
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

/** Обработчик установки использования единого каталога снимков.*/
void BatchProcessingDlg::sourceGrpBxSlot(bool on)
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Если установлено использование единого каталога, то надо 
		// отобразить этот факт в фильтре
		m_filterWidgetVector[i]->setIsUseExternalSourceDir(on);
	}
}

/** Обработчик установки использования единого каталога для отсортированных снимков.*/
void BatchProcessingDlg::distGrpBxSlot(bool on)
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Если установлено использование единого каталога, то надо 
		// отобразить этот факт в фильтре
		m_filterWidgetVector[i]->setIsUseExternalDestinationDir(on);
	}
}

/** Обработчик добавления фильтра.*/
void BatchProcessingDlg::addFilterTlBtnSlot()
{
	int filterNum = m_filterWidgetVector.size();
	for(int i=filterNum-1; i>=0; i--)
	{
		// Находим первый используемый фильтр с конца
		SetFilterWidget* filterWidget = m_filterWidgetVector[i];
		Q_ASSERT(filterWidget);
		if(filterWidget->isUse())
		{
			// Создаем новый фильтр с параметрами найденного фильтра
			SetFilterWidget* widget = new SetFilterWidget(filterWidget->currentFilterSetting(), this);
			// Добавляем новый фильтр после найденного
			addSetFilterWidget(i, widget);
			// Добавляем только один фильтр
			return;
		}
	}
	if(filterNum != 0)
	{
		// Если фильтры есть, но нет ни одного используемого фильтра, то добавляем последний фильтр
		// Создаем новый фильтр с параметрами последнего фильтра
		SetFilterWidget* widget = new SetFilterWidget(m_filterWidgetVector[m_filterWidgetVector.size() - 1]->currentFilterSetting(), this);
		// Добавляем новый фильтр после последнего
		addSetFilterWidget(m_filterWidgetVector.size()-1, widget);
	}
	else
	{
		// Если фильтров вообще нет, то создаем фильтр с параметрами первого фильтра (фильтра по умолчанию)
		QVector<ICECORE_NS::FilterSettings> filterSettingsVector = m_settings->getFilterSettings();
		SetFilterWidget* widget = new SetFilterWidget(filterSettingsVector[0], this);
		addSetFilterWidget(-1, widget);
	}
}

/** Обработчик удаления фильтра.*/
void BatchProcessingDlg::delFilterTlBtnSlot()
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Удаляем все выбранные фильтры
		SetFilterWidget* filterWidget = m_filterWidgetVector[i];
		Q_ASSERT(filterWidget);
		if(filterWidget->isUse())
		{
			ui->verticalLayout->takeAt(i);
			m_filterWidgetVector.remove(i);
			filterWidget->setParent(0);
			delete filterWidget;
			// Рекурсия
			delFilterTlBtnSlot();
			// Переконфигурирем номера фильтров
			reconfFilterNum();
			// Удаляем выбранные фильтры по одному
			return;
		}
	}
}

/** Обработчик выбора/невыбора всех фильтров.*/
void BatchProcessingDlg::useAllChBxSlot(bool on)
{
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		m_filterWidgetVector[i]->setIsUse(on);
	}
}

/** Сбросить настройки фильтров и их количество до момента первого открытия диалогового окна.*/
void BatchProcessingDlg::defaultPBtnSlot()
{
	// Помечаем все фильтры
	useAllChBxSlot(true);
	// Удаляем все помеченные фильтры
	delFilterTlBtnSlot();
	// Восстанавливаем настройки фильтров пакетной обработки
	QVector<ICECORE_NS::FilterSettings> filterSettingsVector =
		m_settings->getFilterSettings();
	for(int i=0; i<filterSettingsVector.size(); i++)
	{
		SetFilterWidget* widget = new SetFilterWidget(filterSettingsVector[i], this);
		addSetFilterWidget(i - 1, widget);
	}
}

/** Создание каталогов для отработанных снимков.*/ 
bool BatchProcessingDlg::createDistDirs()
{
	QString distDir = QString::null;
	// Если для пакетной обработки надо использоваьть единый каталог отработанных снимков
	// то создаем этот каталог
	if(ui->distGrpBx->isChecked())
	{
		distDir = ui->distLnEdt->text();
		if(ICECORE_NS::NotSet.toLatin1() == distDir)
		{
			QMessageBox::warning(this, tr("Пакетная обработка ..."),
				tr("Единый каталог обработанных снимков не установлен.\n\nОбработка будет прервана!"),
				tr("Прервать"));
			return false;
		}
		if(!QDir().mkpath(distDir))
		{
			QMessageBox::warning(this, tr("Пакетная обработка ..."),
				tr("Ошибка создания единого каталога\nобработанных снимков.\n\nОбработка будет прервана!"),
				tr("Прервать"));
			return false;
		}
	}
	// Если задан единый каталог, то каждый фильтр должен в этом каталого создать свой каталог
	// с имененем фильтра, если единый каталого не надо использовать (он равен QString::null), то
	// каждый фильтр должен создать свой каталог для отсортированных снимков, который для него
	// указал пользователь
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Текущий фильтр
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// Задействуем его только если он используется
		if(!widget->isUse())
		{
			continue;
		}
		// Если distDir == QString::null, то единый каталог не надо использовать и каждый фильтр создает
		// свой каталог для отсортированных снимков, указанный пользователем. Если единый каталог надо
		// использовать, то каждый фильтр создает в этом каталоге свой подкаталог для отсортированных
		// снимков с именем фильтра
		if(!widget->createDestinationDir(distDir))
		{
			return false;
		}
	}
	return true;
}

/** Установка исходного каталога для всех фильтров.*/ 
bool BatchProcessingDlg::setSourceDirs()
{
	QString sourceDir = QString::null;
	// Если для пакетной обработки надо использоваьть единый каталог исходных снимков
	// то используем его для всех фильтров, если единый каталог не задан то каждый
	// фильтр будет использовать свой каталог для исходных снимков, установленный пользователем
	if(ui->sourceGrpBx->isChecked())
	{
		sourceDir = ui->sourceLnEdt->text();
		if(ICECORE_NS::NotSet.toLatin1() == sourceDir)
		{
			QMessageBox::warning(this, tr("Пакетная обработка ..."),
				tr("Единый каталог исходных снимков не установлен.\n\nОбработка будет прервана!"),
				tr("Прервать"));
			return false;
		}
	}
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Текущий фильтр
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// Задействуем его только если он используется
		if(!widget->isUse())
		{
			continue;
		}
		widget->setSourceDir(QStringList(sourceDir) << ui->extensionsLbl->text());
	}
	return true;
}

/** Проверка всех установленных данных для обработки на валидность.*/ 
bool BatchProcessingDlg::checkOnValidates()
{
	// Должен быть задействован хотя бы один фильтр
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
		QMessageBox::warning(this, tr("Пакетная обработка ..."),
			tr("В пакетной обработке на задействован ни один фильтр.\n\n") + 
			tr("Задействуйте фильтры!"),
			tr("Прервать"));
		return false;
	}
	/* Единые каталоги все равно устанавливаются в каждый фильтр ...
	*  Т.о. эти проверки заменяются проверками каталогов исходных снимков и
	*  каталогов отсортированных снимков самими фильтрами
	*/
	//// Единый исходный каталог для снимков должен существовать
	//// если он задействован
	//if(ui->sourceGrpBx->isChecked())
	//{
	//	const QDir sourceDir(ui->sourceLnEdt->text());
	//	if(!sourceDir.exists())
	//	{
	//		QMessageBox::warning(this, tr("Пакетная обработка ..."),
	//			tr("Выбранный единый каталог для исходных снимков не существует.\n\n") + 
	//			tr("Установите правильный каталог!"),
	//			tr("Прервать"));
	//		return false;
	//	}
	//	// Предупреждаем пользователя об отстутствии снимков в едином исходном каталоге
	//	if(!sourceDir.entryList(ICECORE_NS::filterImage, QDir::Files).count())
	//	{
	//		QMessageBox::warning(this, tr("Пакетная обработка ..."),
	//			tr("Выбранный единый каталог для исходных снимков не содержит снимков.\n\n") +
	//			tr("Установите правильный каталог!"),
	//			tr("Прервать"));
	//		return false;
	//	}
	//}
	//// Единый каталог для отсортированных снимков должен существовать
	//// если он задействован
	//if(ui->distGrpBx->isChecked())
	//{
	//	const QDir distDir(ui->distLnEdt->text());
	//	if(!distDir.exists())
	//	{
	//		QMessageBox::warning(this, tr("Пакетная обработка ..."),
	//			tr("Выбранный единый каталог для отработанных снимков не существует.\n\n") + 
	//			tr("Установите правильный каталог!"),
	//			tr("Прервать"));
	//		return false;
	//	}
	//	// Предупреждаем пользователя о наличии файлов в каталоге для отсортированных снимков
	//	if(!SetFilterWgt::isDirEmpty(distDir))
	//	{
	//		if(QMessageBox::warning(this, tr("Пакетная обработка ..."),
	//			tr("Выбранный единый каталог для отработанных снимков содержит файлы и(или) подкаталоги.\n\n") +
	//			tr("Продолжить?"), tr("Да"), tr("Нет"), QString(), 1, 1) == 1)
	//		{
	//			return false;
	//		}
	//	}
	//}
	// Проверяем на валидность установки всех значений каждого 
	// задействованного в пакетной обработке фильтра
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Текущий фильтр
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// Задействуем его только если он используется
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

/** Обработчик кнопки "Обработать".*/ 
void BatchProcessingDlg::beginPBtnSlot()
{
	// У всех используемых фильтров должны быть установлены сами фильтры
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Текущий фильтр
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// Задействуем его только если он используется
		if(!widget->isUse())
		{
			continue;
		}
		if(!widget->isSetFilter())
		{
			QMessageBox::warning(this, tr("Пакетная обработка ..."),
				tr("Для фильтра №") + QString::number(i) + tr(" не установлен фильтр.\n\n") + 
				tr("Отключите использование фильтра или установите его!"),
				tr("Прервать"));
			return;
		}
	}
	// Устанавливаем все каталоги для исходных снимков
	if(!setSourceDirs())
	{
		return;
	}
	// Создаем все каталоги для отсортированных снимков
	if(!createDistDirs())
	{
		return;
	}
	// Проводим анализ на правильность всех установленных данных
	if(!checkOnValidates())
	{
		return;
	}
	// Единый исходный каталог для снимков
	QString sourceDir = ui->sourceGrpBx->isChecked() ? ui->sourceLnEdt->text() : QString::null;
	// Единый каталог отсортированных снимков
	QString distDir = ui->distGrpBx->isChecked() ? ui->distLnEdt->text() : QString::null;
	// Вызываем обработку для каждого фильтра
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Текущий фильтр
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// Задействуем его только если он используется
		if(!widget->isUse())
		{
			continue;
		}

		// Запускаем обработку текущего фильтра

		// Подсвечиваем виджет текущего фильтра
		widget->highlight(true);
		// Обработка
		bool res = widget->batchProccess(ui->prBr);
		// Снимаем подсветку виджета текущего фильтра
		widget->highlight(false);
		if(!res)
		{
			return;
		}
		//// Если каталог для отсортированных снимков фильтра остался пустым, удаляем его
		//setFilterWgt->deleteDistDir();
	}

	//// Если единый каталог для отсортированных снимков был задействован и пуст, удаляем его
	//if(!distDir.isNull())
	//{
	//	SetFilterWgt::deleteDir(QDir(distDir));
	//}

	// Отображение результата совместной обработки
	showResult();
}

/** Отображение результата пакетной обработки.*/
void BatchProcessingDlg::showResult()
{
	// Формируем строку вывода результатов пакетной обработки
	QString str;
	for(int i=0; i<m_filterWidgetVector.size(); i++)
	{
		// Текущий фильтр
		SetFilterWidget* widget = m_filterWidgetVector[i];
		Q_ASSERT(widget);
		// Задействуем его только если он используется
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
	QMessageBox::information(this, tr("Пакетная обработка ..."),
		tr("<table border=1 cellpadding=5 cellspacing=1>"
		"<tr>"
		"<th><big>№</big></th>"
		"<th><big>Фильтр</big></th>"
		"<th><big>Анализировано</big></th>"
		"<th><big>Выбрано</big></th>"
		"</tr>") + str + tr("</table>"), tr("Да"));
}

void BatchProcessingDlg::mapPBtnSlot()
{
	Q_ASSERT(m_mapViewDialog);
	m_mapViewDialog->show();
}

} // namespace ICEGUI_NS
