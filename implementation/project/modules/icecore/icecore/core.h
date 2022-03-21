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
* Ядро проекта клиент ПО ЛЁД (Синглетон)
*/
class ICECORE_EXPORT Core : public QObject
{
	Q_OBJECT
public:
	static Core* Instance();

	/**
	* Деструктор
	*/
	virtual ~Core();

	/**
	* Получить модель дерева разделов и фильтров
	* @return модель дерева разделов и фильтров
	*/
	FilterItemModel* getFilterItemModel() const;

	void setSettings(Settings* settings);
	Settings* getSettings() const;

	/**
	* Получить список идетификаторов алгоритмов обработки
	* @return список идетификаторов алгоритмов обработки
	*/
	QStringList getImageProcKey() const;

	/**
	* Получить алгоритм обработки
	* @param key идетификатор алгоритма обработки
	* @return алгоритм обработки
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
	* Конструктор
	* @param parent родительский объект
	*/
	Core(QObject* parent = 0);

private:
	/**
	* Загрузить доступные plugin с алгоритмами обработки
	*/
	void fillImageProcPluginMap();

	FilterItemModel* m_filterItemModel;	///< Модель дерева разделов и фильтров
	Settings* m_settings;

	QMap<QString, ICEPROC_NS::ImageProcInterface*> m_imageProcPluginMap;
		///< Доступные plugin с алгоритмами обработки

	static Core* instance;
};

} // namespace ICECORE_NS

#endif // ICECORE_CORE_H
