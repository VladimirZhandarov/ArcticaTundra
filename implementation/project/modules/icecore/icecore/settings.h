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
* ��������� ���������� � ����� ������
*/
struct DbSettings
{
	QString user;				///< ��� ������������
	QString password;			///< ������ ������������
	QString dbName;				///< ��� ���� ������
	QString host;				///< ��� ��� IP-����� �����, ��� ��������� ����
	unsigned short int port;	///< ���� ���� (5432)
};

/**
* @struct ImageLineSettings
* ��������� ���������� ������
*/
struct ImageLineSettings
{
	ICEIMAGE_NS::UNIT unit;	///< ������� ���������
	bool isUseAlwaysUnit;	///< ���� "������ ������������ �� ���������"
	double ratio;			///< ����������� �������� �� ���������
	QPointF origin;			///< ������ ���������
	bool isShow;			///< ���� "���������� �������"
	bool isUseUserParam;	///< ���� "������������ ���������������� ��������� �� ���������"

	bool ICECORE_EXPORT operator==(const ImageLineSettings& imageLineSettings)const;
};

/**
* @struct ImageGridSettings
* ��������� ���������� �����
*/
struct ImageGridSettings
{
	double hStepPixel;		///< ��� �� ����������� � ��������
	double vStepPixel;		///< ��� �� ��������� � ��������
	double hStepMetric;		///< ��� �� ����������� � ����������� �������
	double vStepMetric;		///< ��� �� ��������� � ����������� �������
	ICEIMAGE_NS::UNIT unit;	///< ������� ���� �� ����������� � �������� � ����������� �������
	bool isOnLine;			///< ���� "�� �������"
	bool isShow;			///< ���� "���������� �����"
	bool isShow800;			///< ���� "�������� ����� �������� � ��������� 800% � ����"
	QColor color;			///< ���� �����
	bool isUseUserParam;	///< ���� "������������ ���������������� ��������� ����� �� ���������"

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
* ��������� ���������� �������� ���������
*/
struct BatchProcessingSettings
{
	QString sourceDir;			///< ����� � ��������� ��������
	bool isUseSourceDir;		///< ���� ������������� ������� �������� ��� �������� �������
	QString destinationDir;		///< ����� � ���������������� ��������
	bool isUseDestinationDir;	///< ���� ������������� ������� �������� ��� ��������������� �������
	QString filter;				///< ������ ���������� ��� �������, ������� ���� �� ���������
};

struct FilterSettings
{
	QString sourceDir;		///< ����� � ��������� ��������
	QString destinationDir;	///< ����� � ������������� ��������
	unsigned int filterId;	///< ������������� �������
	double param;			///< �������� ������� (����������� ������������)
	bool isUse;				///< ���� ������������� �������
	QString filter;			///< ������ ���������� ��� �������, ������� ���� �� ���������
};

/** ��������� �������� ��������� ����������� (���������� �������) .*/
struct ProcessingSettings
{
	bool isCloseAfterFinish;	///< ���� ��������������� �������� ���� ����������� �������� 
								///< ���������� ������� ����� ��������� ������ ��������
};

/**
* @class Settings
* �������� ���������� (�������� � ini-�����)
*/
class ICECORE_EXPORT Settings : public QSettings
{
	Q_OBJECT
	static QString mainWinGroup;
	static QString geometryKey;
	static QString stateKey;

	static QString imageScaleKey;		///< ���� - "������� �� ���������"
	static int imageScaleDefaultValue;	///< �������� �� ��������� ����� ������������

	static QString dbGroup;			///< ������ ���������� ���������� � ��

	static QString dbUserKey;		///< ���� - "��� ������������"
	static QString dbPasswordKey;	///< ���� - "������ ������������"
	static QString dbNameKey;		///< ���� - "��� ���� ������"
	static QString dbHostKey;		///< ���� - "��� ��� IP-����� �����, ��� ��������� ����"
	static QString dbPortKey;		///< ���� - "���� ���� (5432)"

	static QString dbUserDefaultValue;				///< �������� �� ��������� ����� ������������
	static QString dbPasswordDefaultValue;			///< �������� �� ��������� ������ ������������
	static QString dbNameDefaultValue;				///< �������� �� ��������� ����� ���� ������
	static QString dbHostDefaultValue;				///< �������� �� ��������� ����� ��� IP-����� �����, ��� ��������� ����
	static unsigned short int dbPortDefaultValue;	///< �������� �� ��������� ����� ���� (5432)

	static QString imageLineSettingsKey[];				///< ����� ���������� ������ ����������� �� ���������
	static ImageLineSettings imageLineSettingsDefault;	///< ��������� ������ ����������� �� ���������

	static QString imageGridSettingsKey[];				///< ����� ���������� ����� ����������� �� ���������
	static ImageGridSettings imageGridSettingsDefault;	///< �������� ����� ����������� �� ���������

	static QString imageAddedSettingsKey[];
	static ImageAddedSettings imageAddedSettingsDefault;

	static QString batchProcessingSettingsKey[];				///< ����� ���������� �������� ���������
	static BatchProcessingSettings batchProcessingSettingsDefault;	///< ��������� �������� ���������
	static QString filterSettingsKey[];				///< ����� ���������� �������� �������� ���������
	static FilterSettings filterSettingsDefault;	///< ��������� �������� �������� ���������

	static QString processingSettingsKey[];		///< ����� ��������� �������� ��������� �����������
	static ProcessingSettings processingSettingsDefault;	///< ��������� �������� ��������� ����������� �� ���������

	static QString procPluginGroup;		///< ������ ���������� ���������� ��������� �����������
	static QString defaultValuesGroup;	///< ������ ���������� �� ���������� �� ���������

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
	* �����������
	* @param fileInfo ���� ��������
	* @param parent ������������ ������
	*/
	Settings(const QFileInfo& fileInfo, QObject* parent = 0);

	/**
	* ����������
	*/
	~Settings();

	void setMainWindowGeometry(const QByteArray& data);
	QByteArray getMainWindowGeometry() const;

	void setMainWindowState(const QByteArray& data);
	QByteArray getMainWindowState() const;

	void setImageScale(int scalePercent);
	int getImageScale() const;

	/**
	* �������� ��������� ���������� � ����� ������
	* @param dbSettings ��������� ���������� � ����� ������
	*/
	void setDbSettings(const DbSettings& dbSettings);

	/**
	* ������� ��������� ���������� � ����� ������
	* @return ��������� ���������� � ����� ������
	*/
	DbSettings getDbSettings();

	/**
	* �������� ���������������� ��������� ������ �����������
	* @param imageLineSettings ��������� ������ �����������
	*/
	void setImageLineSettings(const ImageLineSettings& imageLineSettings);

	ImageLineSettings getCurrentImageLineSettings();

	/**
	* ������� ���������������� ��������� ������ �����������
	* @return ���������������� ��������� ������ �����������
	*/
	ImageLineSettings getImageLineSettings();

	/**
	* ������� ��������� ������ ����������� �� ���������
	* @return ��������� ������ ����������� �� ���������
	*/
	ImageLineSettings getImageLineSettingsDefault();

	/**
	* �������� ���������������� ��������� ����� �����������
	* @param ImageGridSettings ��������� ������ �����������
	*/
	void setImageGridSettings(const ImageGridSettings& imageGridSettings);

	/**
	* ������� ���������������� ��������� ����� �����������
	* @return ���������������� ��������� ����� �����������
	*/
	ImageGridSettings getImageGridSettings();

	/**
	* ������� ��������� ����� ����������� �� ���������
	* @return ��������� ����� ����������� �� ���������
	*/
	ImageGridSettings getImageGridSettingsDefault();

	void setImageAddedSettings(const ImageAddedSettings& imageAddedSettings);
	ImageAddedSettings getImageAddedSettings();
	ImageAddedSettings getImageAddedSettingsDefault();

	/**������� ���������������� ��������� �������� ���������.*/
	void removeBatchProcessingSettings();

	/**�������� ���������������� ��������� �������� ���������.*/
	void setBatchProcessingSettings(const BatchProcessingSettings& batchProcessingSettings);

	/**������� ���������������� ��������� �������� ���������.*/
	BatchProcessingSettings getBatchProcessingSettings();

	/**�������� ���������������� ��������� �������� �������� ���������.*/
	void setFilterSettings(const FilterSettings& filterSettings, int num);

	/**������� ���������������� ��������� �������� �������� ���������.*/
	QVector<FilterSettings> getFilterSettings();

	/** �������� ��������� ��������� ��������.*/
	void setProcessingSettings(const ProcessingSettings&);

	/** ������� ��������� ��������� ��������.*/
	ProcessingSettings getProcessingSettings();

	/**
	* �������� �������� �� ��������� ��������� ��������� �����������
	* @param procPluginId ������������� ��������� ��������� �����������
	* @param defaultValues �������� �� ���������
	*/
	void setProcPluginDefaultValues(const QString& procPluginId, const QVariantMap& defaultValues);

	/**
	* ������� �������� �� ��������� ��������� ��������� �����������
	* @param procPluginId ������������� ��������� ��������� �����������
	* @return �������� �� ���������
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
	* ���������� �������� ���������� �� ���������
	*/
	virtual void setDefaultValues();

private:
	/**������� ��������� �������� ��������� �� ���������.*/
	BatchProcessingSettings getBatchProcessingSettingsDefault();
};

} // namespace ICECORE_NS

#endif // ICECORE_SETTINGS_H
