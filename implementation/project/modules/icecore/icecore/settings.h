#ifndef ICECORE_SETTINGS_H
#define ICECORE_SETTINGS_H

#include "icecoredef.h"

#include <iceimage/global.h>

#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QPointF>

#include <QtGui/QColor>

namespace ICECORE_NS
{

/**
* @struct DbSettings
* Настройки соединения с базой данных
*/
struct DbSettings
{
	QString user;				///< Имя пользователя
	QString password;			///< Пароль пользователя
	QString dbName;				///< Имя базы данных
	QString host;				///< Имя или IP-адрес хоста, где находится СУБД
	unsigned short int port;	///< Порт СУБД (5432)
};

/**
* @struct ImageLineSettings
* Настройки параметров линеек
*/
struct ImageLineSettings
{
	ICEIMAGE_NS::UNIT unit;	///< Единицы измерения
	bool isUseAlwaysUnit;	///< Флаг "Всегда использовать по умолчанию"
	double ratio;			///< Коэффициент масштаба на местности
	QPointF origin;			///< Начало координат
	bool isShow;			///< Флаг "Показывать линейку"
	bool isUseUserParam;	///< Флаг "Использовать пользовательские параметры по умолчанию"

	bool ICECORE_EXPORT operator==(const ImageLineSettings& imageLineSettings)const;
};

/**
* @struct ImageGridSettings
* Настройки параметров сетки
*/
struct ImageGridSettings
{
	double hStepPixel;		///< Шаг по горизонтали в пикселях
	double vStepPixel;		///< Шаг по вертикали в пикселях
	double hStepMetric;		///< Шаг по горизонтали в метрической системе
	double vStepMetric;		///< Шаг по вертикали в метрической системе
	ICEIMAGE_NS::UNIT unit;	///< Единицы шага по горизонтали и вертикал в метрической системе
	bool isOnLine;			///< Флаг "По линейке"
	bool isShow;			///< Флаг "Показывать сетку"
	bool isShow800;			///< Флаг "Показать сетку пикселей с масштабом 800% и выше"
	QColor color;			///< Цвет сетки
	bool isUseUserParam;	///< Флаг "Использовать пользовательские параметры сетки по умолчанию"

	bool ICECORE_EXPORT operator==(const ImageGridSettings& imageGridSettings) const;
};

struct ImageAddedSettings
{
	bool objectTableFileCreateAuto;
	bool htmlFileCreateAuto;
	bool halfArcLength;
	int jpegQuality;
	bool useColorCursor;
	int colorCursorR;
	int colorCursorG;
	int colorCursorB;
	int colorCursorA;
	bool ICECORE_EXPORT operator==(const ImageAddedSettings& imageAddedSettings) const;
};

/**
* @struct BatchProcessingSettings
* Настройки параметров пакетной обработки
*/
struct BatchProcessingSettings
{
	QString sourceDir;			///< Папка с исходными снимками
	bool isUseSourceDir;		///< Флаг использования единого каталога для исходных снимков
	QString destinationDir;		///< Папка с отсортированными снимками
	bool isUseDestinationDir;	///< Флаг использования единого каталога для отсортированных снимков
	QString filter;				///< Фильтр расширения для снимков, которые идут на обработку
};

struct FilterSettings
{
	QString sourceDir;		///< Папка с исходными снимками
	QString destinationDir;	///< Папка с обработанными снимками
	unsigned int filterId;	///< Идентификатор фильтра
	double param;			///< Параметр фильтра (коэффициент отсортировки)
	bool isUse;				///< Флаг использования фильтра
	QString filter;			///< Фильтр расширения для снимков, которые идут на обработку
};

/** Настройки процесса обработки изображения (применения фильтра) .*/
struct ProcessingSettings
{
	bool isCloseAfterFinish;	///< Флаг автомитического закрытия окна отображения процесса 
								///< применения фильтра после окончания самого процесса
};

/**
* @class Settings
* Настойки приложения (хранение в ini-файле)
*/
class ICECORE_EXPORT Settings : public QSettings
{
	Q_OBJECT
	static QString mainWinGroup;
	static QString geometryKey;
	static QString stateKey;

	static QString imageScaleKey;		///< Ключ - "Масштаб по умолчанию"
	static int imageScaleDefaultValue;	///< Значение по умолчанию имени пользователя

	static QString dbGroup;			///< Группа параметров соединения с БД

	static QString dbUserKey;		///< Ключ - "Имя пользователя"
	static QString dbPasswordKey;	///< Ключ - "Пароль пользователя"
	static QString dbNameKey;		///< Ключ - "Имя базы данных"
	static QString dbHostKey;		///< Ключ - "Имя или IP-адрес хоста, где находится СУБД"
	static QString dbPortKey;		///< Ключ - "Порт СУБД (5432)"

	static QString dbUserDefaultValue;				///< Значение по умолчанию имени пользователя
	static QString dbPasswordDefaultValue;			///< Значение по умолчанию пароля пользователя
	static QString dbNameDefaultValue;				///< Значение по умолчанию имени базы данных
	static QString dbHostDefaultValue;				///< Значение по умолчанию имени или IP-адрес хоста, где находится СУБД
	static unsigned short int dbPortDefaultValue;	///< Значение по умолчанию порта СУБД (5432)

	static QString imageLineSettingsKey[];				///< Ключи параметров линеек изображения по умолчанию
	static ImageLineSettings imageLineSettingsDefault;	///< Параметры линеек изображения по умолчанию

	static QString imageGridSettingsKey[];				///< Ключи параметров сетки изображения по умолчанию
	static ImageGridSettings imageGridSettingsDefault;	///< Праметры сетки изображения по умолчанию

	static QString imageAddedSettingsKey[];
	static ImageAddedSettings imageAddedSettingsDefault;

	static QString batchProcessingSettingsKey[];				///< Ключи параметров пакетной обработки
	static BatchProcessingSettings batchProcessingSettingsDefault;	///< Параметры пакетной обработки
	static QString filterSettingsKey[];				///< Ключи параметров фильтров пакетной обработки
	static FilterSettings filterSettingsDefault;	///< Параметры фильтров пакетной обработки

	static QString processingSettingsKey[];		///< Ключи настройки процесса обработки изображения
	static ProcessingSettings processingSettingsDefault;	///< Настройки процесса обработки изображения по умолчанию

	static QString procPluginGroup;		///< Группа параметров алгоритмов обработки изображений
	static QString defaultValuesGroup;	///< Группа параметров со значениями по умолчанию

	static QString filterParamSaveDirKey;
	static QString filterParamLoadDirKey;
	static QString imageCutUpSaveDirKey;
	static QString imageCutUpLoadDirKey;
	static QString imageSaveDirKey;
	static QString imageLoadDirKey;
	static QString imageSaveSuffixKey;
	static QString imageLoadSuffixKey;

public:
	/**
	* Конструктор
	* @param fileInfo файл настроек
	* @param parent родительский объект
	*/
	Settings(const QFileInfo& fileInfo, QObject* parent = 0);

	/**
	* Деструктор
	*/
	~Settings();

	void setMainWindowGeometry(const QByteArray& data);
	QByteArray getMainWindowGeometry() const;

	void setMainWindowState(const QByteArray& data);
	QByteArray getMainWindowState() const;

	void setImageScale(int scalePercent);
	int getImageScale() const;

	/**
	* Записать настройки соединения с базой данных
	* @param dbSettings настройки соединения с базой данных
	*/
	void setDbSettings(const DbSettings& dbSettings);

	/**
	* Считать настройки соединения с базой данных
	* @return настройки соединения с базой данных
	*/
	DbSettings getDbSettings();

	/**
	* Записать пользовательские настройки линеек изображения
	* @param imageLineSettings настройки линеек изображения
	*/
	void setImageLineSettings(const ImageLineSettings& imageLineSettings);

	ImageLineSettings getCurrentImageLineSettings();

	/**
	* Считать пользовательские настройки линеек изображения
	* @return Пользовательские настройки линеек изображения
	*/
	ImageLineSettings getImageLineSettings();

	/**
	* Считать настройки линеек изображения по умолчанию
	* @return Настройки линеек изображения по умолчанию
	*/
	ImageLineSettings getImageLineSettingsDefault();

	/**
	* Записать пользовательские настройки сетки изображения
	* @param ImageGridSettings настройки линеек изображения
	*/
	void setImageGridSettings(const ImageGridSettings& imageGridSettings);

	/**
	* Считать пользовательские настройки сетки изображения
	* @return Пользовательские настройки сетки изображения
	*/
	ImageGridSettings getImageGridSettings();

	/**
	* Считать настройки сетки изображения по умолчанию
	* @return Настройки сетки изображения по умолчанию
	*/
	ImageGridSettings getImageGridSettingsDefault();

	void setImageAddedSettings(const ImageAddedSettings& imageAddedSettings);
	ImageAddedSettings getImageAddedSettings();
	ImageAddedSettings getImageAddedSettingsDefault();

	/**Удалить пользовательские настройки пакетной обработки.*/
	void removeBatchProcessingSettings();

	/**Записать пользовательские настройки пакетной обработки.*/
	void setBatchProcessingSettings(const BatchProcessingSettings& batchProcessingSettings);

	/**Считать пользовательские настройки пакетной обработки.*/
	BatchProcessingSettings getBatchProcessingSettings();

	/**Записать пользовательские настройки фильтров пакетной обработки.*/
	void setFilterSettings(const FilterSettings& filterSettings, int num);

	/**Считать пользовательские настройки фильтров пакетной обработки.*/
	QVector<FilterSettings> getFilterSettings();

	/** Записать настройки обработки фильтром.*/
	void setProcessingSettings(const ProcessingSettings&);

	/** Считать настройки обработки фильтром.*/
	ProcessingSettings getProcessingSettings();

	/**
	* Записать значения по умолчанию алгоритма обработки изображений
	* @param procPluginId идентификатор алгоритма обработки изображений
	* @param defaultValues значения по умолчанию
	*/
	void setProcPluginDefaultValues(const QString& procPluginId, const QVariantMap& defaultValues);

	/**
	* Считать значения по умолчанию алгоритма обработки изображений
	* @param procPluginId идентификатор алгоритма обработки изображений
	* @return значения по умолчанию
	*/
	QVariantMap getProcPluginDefaultValues(const QString& procPluginId);

	void setFilterParamSaveDir(const QString& dir);
	QString getFilterParamSaveDir() const;

	void setFilterParamLoadDir(const QString& dir);
	QString getFilterParamLoadDir() const;

	void setImageCutUpSaveDir(const QString& dir);
	QString getImageCutUpSaveDir() const;

	void setImageCutUpLoadDir(const QString& dir);
	QString getImageCutUpLoadDir() const;
	
	void setImageSaveDir(const QString& dir);
	QString getImageSaveDir() const;

	void setImageLoadDir(const QString& dir);
	QString getImageLoadDir() const;

	void setImageSaveSuffix(const QString& suffix);
	QString getImageSaveSuffix() const;

	void setImageLoadSuffix(const QString& suffix);
	QString getImageLoadSuffix() const;

protected:
	/**
	* Установить значения параметров по умолчанию
	*/
	virtual void setDefaultValues();

private:
	/**Считать настройки пакетной обработки по умолчанию.*/
	BatchProcessingSettings getBatchProcessingSettingsDefault();
};

} // namespace ICECORE_NS

#endif // ICECORE_SETTINGS_H
