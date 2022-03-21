#include "settings.h"

#include "global.h"

#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QVector>

namespace ICECORE_NS
{

bool ImageLineSettings::operator==(const ImageLineSettings& imageLineSettings) const
{
	return (unit == imageLineSettings.unit &&
		isUseAlwaysUnit == imageLineSettings.isUseAlwaysUnit &&
		ratio == imageLineSettings.ratio &&
		origin == imageLineSettings.origin &&
		isShow == imageLineSettings.isShow);
}

bool ImageGridSettings::operator==(const ImageGridSettings& imageGridSettings) const
{
	if(hStepPixel != imageGridSettings.hStepPixel)
	{
		return false;
	}
	if(vStepPixel != imageGridSettings.vStepPixel)
	{
		return false;
	}
	// Приводим все к единой метрической системе и сравниваем значения 
	// в метрической системе
	if(unit == imageGridSettings.unit)
	{
		if(hStepMetric != imageGridSettings.hStepMetric)
		{
			return false;
		}
		if(vStepMetric != imageGridSettings.vStepMetric)
		{
			return false;
		}
	}
	else
	{
		double h = hStepMetric;
		double v = vStepMetric;
		if(imageGridSettings.unit == ICEIMAGE_NS::M)
		{
			h *= 1000;
			v *= 1000;
		}
		else
		{
			h /= 1000;
			v /= 1000;
		}
		if(h != imageGridSettings.hStepMetric)
		{
			return false;
		}
		if(v != imageGridSettings.vStepMetric)
		{
			return false;
		}
	}

	if(isOnLine != imageGridSettings.isOnLine)
	{
		return false;
	}
	if(isShow != imageGridSettings.isShow)
	{
		return false;
	}
	if(isShow800 != imageGridSettings.isShow800)
	{
		return false;
	}
	if(color != imageGridSettings.color)
	{
		return false;
	}
	return true;
}

bool ImageAddedSettings::operator==(const ImageAddedSettings& imageAddedSettings) const
{
	return (objectTableFileCreateAuto == imageAddedSettings.objectTableFileCreateAuto &&
		htmlFileCreateAuto == imageAddedSettings.htmlFileCreateAuto &&
		halfArcLength == imageAddedSettings.halfArcLength &&
		jpegQuality == imageAddedSettings.jpegQuality &&
		colorCursorR == imageAddedSettings.colorCursorR &&
		colorCursorG == imageAddedSettings.colorCursorG &&
		colorCursorB == imageAddedSettings.colorCursorB &&
		colorCursorA == imageAddedSettings.colorCursorA);
}

QString Settings::mainWinGroup = "mainWin";
QString Settings::geometryKey = "geometry";
QString Settings::stateKey = "state";

QString Settings::imageScaleKey = "imageScale";
int Settings::imageScaleDefaultValue = 100;

QString Settings::dbGroup = "db";

QString Settings::dbUserKey = "user";
QString Settings::dbPasswordKey = "password";
QString Settings::dbNameKey = "dbName";
QString Settings::dbHostKey = "host";
QString Settings::dbPortKey = "port";

QString Settings::dbUserDefaultValue = "postgres";
QString Settings::dbPasswordDefaultValue = "postgres";
QString Settings::dbNameDefaultValue = "icedb";
QString Settings::dbHostDefaultValue = "localhost";
unsigned short int Settings::dbPortDefaultValue = 5432;

QString Settings::imageLineSettingsKey[] =
	{"imageLine", "unit", "isUseAlwaysUnit", "ratio", "origin", "isShow", "isUseUserParam"};

// Настройки линеек по умолчанию
ImageLineSettings Settings::imageLineSettingsDefault =
	{ICEIMAGE_NS::NONE, false, 1, QPointF(), false, false};

QString Settings::imageGridSettingsKey[] =
	{"imageGrid", "hStepPixel", "vStepPixel", "hStepMetric", "vStepMetric", "unit",
	"isOnLine", "isShow", "isShow800", "color", "isUseUserParam"};

// Настройки сетки по умолчанию
ImageGridSettings Settings::imageGridSettingsDefault =
	{200, 200, 100, 100, ICEIMAGE_NS::M, true, false, true,
	QColor(0, 0, 255, 255 - 30.0 / 100 * 255), false};

QString Settings::imageAddedSettingsKey[] =
	{"imageAdded", "objectTableFileCreateAuto", "htmlFileCreateAuto", "halfArcLength",
	"jpegQuality", "useColorCursor", "colorCursorR", "colorCursorG", "colorCursorB",
	"colorCursorA"};

ImageAddedSettings Settings::imageAddedSettingsDefault =
	{true, false, false, 95, false, 255, 255, 255, 255};

// Настройки пакетной обработки по умолчанию
BatchProcessingSettings Settings::batchProcessingSettingsDefault =
	{NotSet, true, NotSet, true, imageFileSuffix[0]};

QString Settings::batchProcessingSettingsKey[] =
	{"batchProcessing", "sourceDir", "isUseSourceDir", "destinationDir", "isUseDestinationDir",
	"filter"};

// Настройки пакетной обработки по умолчанию (настройки фильтров)
FilterSettings Settings::filterSettingsDefault =
	{NotSet, NotSet, 0, 10, false, imageFileSuffix[0]};

QString Settings::filterSettingsKey[] =
	{"filter", "sourceDir", "destinationDir", "filterId", "param", "isUse", "filter"};

// Настройки процесса применения фильтра к снимку
ProcessingSettings Settings::processingSettingsDefault =
	{false};

QString Settings::processingSettingsKey[] =
	{"Processing", "isCloseAfterFinish"};

QString Settings::procPluginGroup = "procPlugin";
QString Settings::defaultValuesGroup = "defaultValues";

QString Settings::filterParamSaveDirKey = "filterParamSaveDir";
QString Settings::filterParamLoadDirKey = "filterParamLoadDir";
QString Settings::imageCutUpSaveDirKey = "imageCutUpSaveDir";
QString Settings::imageCutUpLoadDirKey = "imageCutUpLoadDir";
QString Settings::imageSaveDirKey = "imageSaveDir";
QString Settings::imageLoadDirKey = "imageLoadDir";
QString Settings::imageSaveSuffixKey = "imageSaveSuffix";
QString Settings::imageLoadSuffixKey = "imageLoadSuffix";

Settings::Settings(const QFileInfo& fileInfo, QObject* parent) :
	QSettings(fileInfo.absoluteFilePath(), QSettings::IniFormat, parent)
{
	if(!fileInfo.exists())
	{
		setDefaultValues();
	}
}

Settings::~Settings()
{

}

void Settings::setMainWindowGeometry(const QByteArray& data)
{
	setValue(geometryKey, data);
}

QByteArray Settings::getMainWindowGeometry() const
{
	return value(geometryKey).toByteArray();
}

void Settings::setMainWindowState(const QByteArray& data)
{
	setValue(stateKey, data);
}

QByteArray Settings::getMainWindowState() const
{
	return value(stateKey).toByteArray();
}

void Settings::setImageScale(int scalePercent)
{
	setValue(imageScaleKey, scalePercent);
}

int Settings::getImageScale() const
{
	return value(imageScaleKey, imageScaleDefaultValue).toInt();
}

void Settings::setDbSettings(const DbSettings& dbSettings)
{
	beginGroup(dbGroup);

	setValue(dbUserKey, dbSettings.user);
	setValue(dbPasswordKey, dbSettings.password);
	setValue(dbNameKey, dbSettings.dbName);
	setValue(dbHostKey, dbSettings.host);
	setValue(dbPortKey, dbSettings.port);

	endGroup();
}

DbSettings Settings::getDbSettings()
{
	DbSettings result;
	beginGroup(dbGroup);

	result.user = value(dbUserKey).toString();
	result.password = value(dbPasswordKey).toString();
	result.dbName = value(dbNameKey).toString();
	result.host = value(dbHostKey).toString();
	bool intOk = false;
	result.port = value(dbPortKey).toUInt(&intOk);
	if(!intOk)
	{
		result.port = dbPortDefaultValue;
	}

	endGroup();
	return result;
}

void Settings::setImageLineSettings(const ImageLineSettings& imageLineSettings)
{
	beginGroup(imageLineSettingsKey[0]);
	setValue(imageLineSettingsKey[1], imageLineSettings.unit);
	setValue(imageLineSettingsKey[2], imageLineSettings.isUseAlwaysUnit);
	setValue(imageLineSettingsKey[3], imageLineSettings.ratio);
	setValue(imageLineSettingsKey[4], imageLineSettings.origin);
	setValue(imageLineSettingsKey[5], imageLineSettings.isShow);
	setValue(imageLineSettingsKey[6], imageLineSettings.isUseUserParam);
	endGroup();
}

ImageLineSettings Settings::getImageLineSettings()
{
	ImageLineSettings result;
	beginGroup(imageLineSettingsKey[0]);
	result.unit = (ICEIMAGE_NS::UNIT)value(imageLineSettingsKey[1], imageLineSettingsDefault.unit).toInt();
	result.isUseAlwaysUnit = value(imageLineSettingsKey[2]).toBool();
	result.ratio = value(imageLineSettingsKey[3], imageLineSettingsDefault.ratio).toDouble();
	result.origin = value(imageLineSettingsKey[4]).toPointF();
	result.isShow = value(imageLineSettingsKey[5]).toBool();
	result.isUseUserParam = value(imageLineSettingsKey[6]).toBool();
	endGroup();
	return result;
}

ImageLineSettings Settings::getImageLineSettingsDefault()
{
	return imageLineSettingsDefault;
}

void Settings::setImageGridSettings(const ImageGridSettings& imageGridSettings)
{
	beginGroup(imageGridSettingsKey[0]);
	setValue(imageGridSettingsKey[1], imageGridSettings.hStepPixel);
	setValue(imageGridSettingsKey[2], imageGridSettings.vStepPixel);
	setValue(imageGridSettingsKey[3], imageGridSettings.hStepMetric);
	setValue(imageGridSettingsKey[4], imageGridSettings.vStepMetric);
	setValue(imageGridSettingsKey[5], imageGridSettings.unit);
	setValue(imageGridSettingsKey[6], imageGridSettings.isOnLine);
	setValue(imageGridSettingsKey[7], imageGridSettings.isShow);
	setValue(imageGridSettingsKey[8], imageGridSettings.isShow800);
	setValue(imageGridSettingsKey[9], imageGridSettings.color);
	setValue(imageGridSettingsKey[10], imageGridSettings.isUseUserParam);
	endGroup();
}

ImageGridSettings Settings::getImageGridSettings()
{
	ImageGridSettings result;
	beginGroup(imageGridSettingsKey[0]);
	result.hStepPixel = value(imageGridSettingsKey[1], imageGridSettingsDefault.hStepPixel).toInt();
	result.vStepPixel = value(imageGridSettingsKey[2], imageGridSettingsDefault.vStepPixel).toInt();
	result.hStepMetric = value(imageGridSettingsKey[3], imageGridSettingsDefault.hStepMetric).toDouble();
	result.vStepMetric = value(imageGridSettingsKey[4], imageGridSettingsDefault.vStepMetric).toDouble();
	result.unit = (ICEIMAGE_NS::UNIT)value(imageGridSettingsKey[5], imageGridSettingsDefault.unit).toInt();
	result.isOnLine = value(imageGridSettingsKey[6]).toBool();
	result.isShow = value(imageGridSettingsKey[7]).toBool();
	result.isShow800 = value(imageGridSettingsKey[8]).toBool();
	result.color = value(imageGridSettingsKey[9], imageGridSettingsDefault.color).value<QColor>();
	result.isUseUserParam = value(imageGridSettingsKey[10]).toBool();
	endGroup();
	return result;
}

ImageGridSettings Settings::getImageGridSettingsDefault()
{
	return imageGridSettingsDefault;
}

void Settings::setImageAddedSettings(const ImageAddedSettings& imageAddedSettings)
{
	beginGroup(imageAddedSettingsKey[0]);

	setValue(imageAddedSettingsKey[1], imageAddedSettings.objectTableFileCreateAuto);
	setValue(imageAddedSettingsKey[2], imageAddedSettings.htmlFileCreateAuto);
	setValue(imageAddedSettingsKey[3], imageAddedSettings.halfArcLength);
	setValue(imageAddedSettingsKey[4], imageAddedSettings.jpegQuality);
	setValue(imageAddedSettingsKey[5], imageAddedSettings.useColorCursor);
	setValue(imageAddedSettingsKey[6], imageAddedSettings.colorCursorR);
	setValue(imageAddedSettingsKey[7], imageAddedSettings.colorCursorG);
	setValue(imageAddedSettingsKey[8], imageAddedSettings.colorCursorB);
	setValue(imageAddedSettingsKey[9], imageAddedSettings.colorCursorA);

	endGroup();
}

ImageAddedSettings Settings::getImageAddedSettings()
{
	ImageAddedSettings result;
	beginGroup(imageAddedSettingsKey[0]);

	result.objectTableFileCreateAuto = value(imageAddedSettingsKey[1], imageAddedSettingsDefault.objectTableFileCreateAuto).toBool();
	result.htmlFileCreateAuto = value(imageAddedSettingsKey[2], imageAddedSettingsDefault.htmlFileCreateAuto).toBool();
	result.halfArcLength = value(imageAddedSettingsKey[3], imageAddedSettingsDefault.halfArcLength).toBool();
	result.jpegQuality = value(imageAddedSettingsKey[4], imageAddedSettingsDefault.jpegQuality).toInt();
	result.useColorCursor = value(imageAddedSettingsKey[5], imageAddedSettingsDefault.useColorCursor).toBool();
	result.colorCursorR = value(imageAddedSettingsKey[6], imageAddedSettingsDefault.colorCursorR).toInt();
	result.colorCursorG = value(imageAddedSettingsKey[7], imageAddedSettingsDefault.colorCursorG).toInt();
	result.colorCursorB = value(imageAddedSettingsKey[8], imageAddedSettingsDefault.colorCursorB).toInt();
	result.colorCursorA = value(imageAddedSettingsKey[9], imageAddedSettingsDefault.colorCursorA).toInt();

	endGroup();
	return result;
}

ImageAddedSettings Settings::getImageAddedSettingsDefault()
{
	return imageAddedSettingsDefault;
}

void Settings::setBatchProcessingSettings(const BatchProcessingSettings& batchProcessingSettings)
{
	beginGroup(batchProcessingSettingsKey[0]);

	setValue(batchProcessingSettingsKey[1], batchProcessingSettings.sourceDir);
	setValue(batchProcessingSettingsKey[2], batchProcessingSettings.isUseSourceDir);
	setValue(batchProcessingSettingsKey[3], batchProcessingSettings.destinationDir);
	setValue(batchProcessingSettingsKey[4], batchProcessingSettings.isUseDestinationDir);
	setValue(batchProcessingSettingsKey[5], batchProcessingSettings.filter);

	endGroup();
}

void Settings::removeBatchProcessingSettings()
{
	// Удаляем информацию о пакетной обработке, так как количество фильтров может
	// записываться меньше чем было. Если не удалять, то кол-во фильтров в файле 
	// не изменится
	remove(batchProcessingSettingsKey[0]);
}

void Settings::setFilterSettings(const FilterSettings& filterSettings, int num)
{
	beginGroup(batchProcessingSettingsKey[0]);
	beginGroup(filterSettingsKey[0] + QString::number(num));

	setValue(filterSettingsKey[1], filterSettings.sourceDir);
	setValue(filterSettingsKey[2], filterSettings.destinationDir);
	setValue(filterSettingsKey[3], filterSettings.filterId);
	setValue(filterSettingsKey[4], filterSettings.param);
	setValue(filterSettingsKey[5], filterSettings.isUse);
	setValue(filterSettingsKey[6], filterSettings.filter);

	endGroup();
	endGroup();
}

BatchProcessingSettings Settings::getBatchProcessingSettings()
{
	BatchProcessingSettings result;
	beginGroup(batchProcessingSettingsKey[0]);

	result.sourceDir = value(batchProcessingSettingsKey[1]).toString();
	if(result.sourceDir.isNull())
	{
		result.sourceDir = batchProcessingSettingsDefault.sourceDir.toLatin1();
	}
	result.isUseSourceDir = value(batchProcessingSettingsKey[2], batchProcessingSettingsDefault.isUseSourceDir).toBool();
	result.destinationDir = value(batchProcessingSettingsKey[3]).toString();
	if(result.destinationDir.isNull())
	{
		result.destinationDir = batchProcessingSettingsDefault.destinationDir.toLatin1();
	}
	result.isUseDestinationDir = value(batchProcessingSettingsKey[4], batchProcessingSettingsDefault.isUseSourceDir).toBool();
	if(result.filter.isNull())
	{
		result.filter = batchProcessingSettingsDefault.filter.toLatin1();
	}

	endGroup();
	return result;
}

QVector<FilterSettings> Settings::getFilterSettings()
{
	QVector<FilterSettings> filterSettingsVector;

	beginGroup(batchProcessingSettingsKey[0]);
	int i = 1; 
	while(true)
	{
		FilterSettings result;
		beginGroup(filterSettingsKey[0] + QString::number(i));
		result.sourceDir = value(filterSettingsKey[1]).toString();
		if(result.sourceDir.isNull())
		{
			// Если мы только что зашли в секцию фильтров и там нет фильтров - возвращаем один фильтр по умолчанию
			if(i == 1)
			{
				result.destinationDir = Settings::filterSettingsDefault.destinationDir.toLatin1();
				result.filterId = Settings::filterSettingsDefault.filterId;
				result.isUse = Settings::filterSettingsDefault.isUse;
				result.param = Settings::filterSettingsDefault.param;
				result.sourceDir = Settings::filterSettingsDefault.sourceDir.toLatin1();
				result.filter = Settings::filterSettingsDefault.filter.toLatin1();
				filterSettingsVector.push_back(result);
				//TODO refac
				endGroup();
				endGroup();
				return filterSettingsVector;
			}
			// Все, больше фильтров нет
			else
			{
				endGroup();
				endGroup();
				return filterSettingsVector;
			}
		}
		result.destinationDir = value(filterSettingsKey[2]).toString();
		result.filterId = value(filterSettingsKey[3]).toInt();
		result.param = value(filterSettingsKey[4]).toDouble();
		result.isUse = value(filterSettingsKey[5]).toBool();
		result.filter = value(filterSettingsKey[6]).toString();
		endGroup();
		filterSettingsVector.push_back(result);
		i++;
	}
}

BatchProcessingSettings Settings::getBatchProcessingSettingsDefault()
{
	return batchProcessingSettingsDefault;
}

void Settings::setProcessingSettings(const ProcessingSettings& processingSettings)
{
	beginGroup(processingSettingsKey[0]);

	setValue(processingSettingsKey[1], processingSettings.isCloseAfterFinish);

	endGroup();
}

ProcessingSettings Settings::getProcessingSettings()
{
	ProcessingSettings result;
	beginGroup(processingSettingsKey[0]);

	result.isCloseAfterFinish = value(processingSettingsKey[1], processingSettingsDefault.isCloseAfterFinish).toBool();

	endGroup();
	return result;
}


void Settings::setProcPluginDefaultValues(const QString& procPluginId, const QVariantMap& defaultValues)
{
	beginGroup(procPluginGroup);
	beginGroup(procPluginId);
	beginGroup(defaultValuesGroup);

	QVariantMap::const_iterator iterEnd = defaultValues.constEnd();
	for(QVariantMap::const_iterator iter = defaultValues.constBegin(); iter != iterEnd; iter++)
	{
		setValue(iter.key(), iter.value());
	}

	endGroup();
	endGroup();
	endGroup();
}

QVariantMap Settings::getProcPluginDefaultValues(const QString& procPluginId)
{
	QVariantMap result;
	beginGroup(procPluginGroup);
	beginGroup(procPluginId);
	beginGroup(defaultValuesGroup);

	QStringList keys = childKeys();
	QStringList::const_iterator iterEnd = keys.constEnd();
	for(QStringList::const_iterator iter = keys.constBegin(); iter != iterEnd; iter++)
	{
		result.insert(*iter, value(*iter));
	}

	endGroup();
	endGroup();
	endGroup();
	return result;
}

void Settings::setFilterParamSaveDir(const QString& dir)
{
	setValue(filterParamSaveDirKey, dir);
}

QString Settings::getFilterParamSaveDir() const
{
	return value(filterParamSaveDirKey).toString();
}

void Settings::setFilterParamLoadDir(const QString& dir)
{
	setValue(filterParamLoadDirKey, dir);
}

QString Settings::getFilterParamLoadDir() const
{
	return value(filterParamLoadDirKey).toString();
}

void Settings::setImageCutUpSaveDir(const QString& dir)
{
	setValue(imageCutUpSaveDirKey, dir);
}

QString Settings::getImageCutUpSaveDir() const
{
	return value(imageCutUpSaveDirKey).toString();
}

void Settings::setImageCutUpLoadDir(const QString& dir)
{
	setValue(imageCutUpLoadDirKey, dir);
}

QString Settings::getImageCutUpLoadDir() const
{
	return value(imageCutUpLoadDirKey).toString();
}

void Settings::setImageSaveDir(const QString& dir)
{
	setValue(imageSaveDirKey, dir);
}

QString Settings::getImageSaveDir() const
{
	return value(imageSaveDirKey).toString();
}

void Settings::setImageLoadDir(const QString& dir)
{
	setValue(imageLoadDirKey, dir);
}

QString Settings::getImageLoadDir() const
{
	return value(imageLoadDirKey).toString();
}

void Settings::setImageSaveSuffix(const QString& suffix)
{
	setValue(imageSaveSuffixKey, suffix);
}

QString Settings::getImageSaveSuffix() const
{
	return value(imageSaveSuffixKey).toString();
}

void Settings::setImageLoadSuffix(const QString& suffix)
{
	setValue(imageLoadSuffixKey, suffix);
}

QString Settings::getImageLoadSuffix() const
{
	return value(imageLoadSuffixKey).toString();
}

void Settings::setDefaultValues()
{
	setImageScale(imageScaleDefaultValue);

	DbSettings dbSettings;
	dbSettings.user = dbUserDefaultValue;
	dbSettings.password = dbPasswordDefaultValue;
	dbSettings.dbName = dbNameDefaultValue;
	dbSettings.host = dbHostDefaultValue;
	dbSettings.port = dbPortDefaultValue;
	setDbSettings(dbSettings);

	setImageLineSettings(imageLineSettingsDefault);
	setImageGridSettings(imageGridSettingsDefault);
}

} // namespace ICECORE_NS
