#ifndef ICECORE_CORE_H
#define ICECORE_CORE_H

#include "icecoredef.h"

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QString>

#include <boost/tr1/memory.hpp>

namespace ICEPROC_NS
{
	class ImageProcInterface;
}

namespace ICEODB_NS
{
	class FilterParam;
	class MaskParam;
	class MaskView;
}

namespace ICECORE_NS
{
	class FilterItemModel;
	class Settings;

/**
* @class Core
* ���� ������� ������ �� ˨� (���������)
*/
class ICECORE_EXPORT Core : public QObject
{
	Q_OBJECT
public:
	static Core* Instance();

	/**
	* ����������
	*/
	virtual ~Core();

	/**
	* �������� ������ ������ �������� � ��������
	* @return ������ ������ �������� � ��������
	*/
	FilterItemModel* getFilterItemModel() const;

	void setSettings(Settings* settings);
	Settings* getSettings() const;

	/**
	* �������� ������ �������������� ���������� ���������
	* @return ������ �������������� ���������� ���������
	*/
	QStringList getImageProcKey() const;

	/**
	* �������� �������� ���������
	* @param key ������������ ��������� ���������
	* @return �������� ���������
	*/
	ICEPROC_NS::ImageProcInterface* getImageProc(const QString& key) const;

	bool saveFilterParam(const QString& filename,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const;

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> loadFilterParam(const QString& filename) const;

protected:
	void filterParamToDataStream(QDataStream& dataStream,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const;

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParamFromDataStream(
		QDataStream& dataStream) const;

	void maskParamToDataStream(QDataStream& dataStream,
		const std::tr1::shared_ptr<ICEODB_NS::MaskParam>& maskParam) const;

	std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParamFromDataStream(
		QDataStream& dataStream) const;

	void maskViewToDataStream(QDataStream& dataStream,
		const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView) const;

	std::tr1::shared_ptr<ICEODB_NS::MaskView> maskViewFromDataStream(
		QDataStream& dataStream) const;

	/**
	* �����������
	* @param parent ������������ ������
	*/
	Core(QObject* parent = 0);

private:
	/**
	* ��������� ��������� plugin � ����������� ���������
	*/
	void fillImageProcPluginMap();

	FilterItemModel* m_filterItemModel;	///< ������ ������ �������� � ��������
	Settings* m_settings;

	QMap<QString, ICEPROC_NS::ImageProcInterface*> m_imageProcPluginMap;
		///< ��������� plugin � ����������� ���������

	static Core* instance;
};

} // namespace ICECORE_NS

#endif // ICECORE_CORE_H
