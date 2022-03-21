#ifndef SETFILTERWIDGET_H
#define SETFILTERWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>
#include "ui_setfilterwidget.h"

#include <boost/tr1/memory.hpp>

class QDir;
class QProgressBar;

namespace ICECORE_NS
{
	struct FilterSettings;
}

namespace ICEIMAGE_NS
{
	class ImageBuilder;
	class ContourItemModel;
}

namespace ICEODB_NS
{
	class FilterParam;
}

namespace ICEGUI_NS
{

/** Виджет установки фильтра и задействование фильтра в пакетной обработке.*/
class ICEGUI_EXPORT SetFilterWidget : public QWidget
{
	Q_OBJECT
	static bool isForce;		///< Пользователь согласился перезаписывать файлы
	static bool forAllDestinationDir;	///< Пользователь согласился работать с не пустым каталогом для отсортированных снимков

public:
	static QStringList selectDir(const QStringList& data, QWidget* parent = 0);
	static QStringList selectDestinationDir(const QStringList& data, QWidget* parent = 0);
	static void deleteDir(const QDir& dir);
	static bool isDirEmpty(const QDir& dir);

	SetFilterWidget(const ICECORE_NS::FilterSettings& filterSettings, QWidget* parent = 0,
		Qt::WindowFlags f = 0);
	~SetFilterWidget();

	ICECORE_NS::FilterSettings currentFilterSetting() const;
	bool isUse() const;
	void setIsUse(bool on);
	void setFilterNum(int num);
	bool checkOnValidate();

	void setIsUseExternalSourceDir(bool on);
	void setIsUseExternalDestinationDir(bool on);

	bool batchProccess(QProgressBar* progressBar);

	bool createDestinationDir(const QString& externalDestinationDir = QString::null);
	void deleteDestinationDir();
	void setSourceDir(const QStringList& externalSourceDir);

	int getAnalysisNum() const;
	int getSortedNum() const;
	QString getName() const;
	bool isSetFilter() const;

	void highlight(bool);

protected slots:
	void sourceDirTB_pressed();
	void destinationDirTB_pressed();
	void filterPB_pressed();
	void useChB_toggled(bool);

private: 
	static int copyFileToFile(const QString& filePathFrom, const QString& filePathTo, 
		bool* force, QWidget* parent = 0);
	static std::tr1::shared_ptr<ICEODB_NS::FilterParam> getFilterParam();

	// Работа с каталогами
	int copyToDistDir(const QString& filePath,
		const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);

	// Работа с масками фильтра
	void prepFilterMask();
	double applyFilterMask(const QString& filePath,
		QSharedPointer<ICEIMAGE_NS::ImageBuilder>* imageBuilder);

	void insertTime(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);

private:
	Ui::SetFilterWidget ui;

	int m_analysisNum;			///< Кол-во проанализированных файлов
	int m_sortedNum;			///< Кол-во отсортированных файлов
	QString m_sourceDir;		///< Исходный каталог для снимков
	QString m_destinationDir;	///< Каталог для отработанных снимков
	bool m_isExternalDistDir;	///< Факт установки каталога для отсортированных снимков извне
	bool m_isExternalSourceDir;	///< Факт установки исходного каталога для снимков извне
	QString m_filter;			///< Расширения снимков, который должне обрабатываться данным фильтром
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> m_filterParam;
};

} // namespace ICEGUI_NS

#endif // SETFILTERWIDGET_H
