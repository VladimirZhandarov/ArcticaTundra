#ifndef FILTERITEMMODEL_H
#define FILTERITEMMODEL_H

#include "icecoredef.h"

#include <QtCore/QAbstractItemModel>

#include "exception.h"

#include <boost/tr1/memory.hpp>
#include <vector>

namespace odb
{
	class database;
};

namespace ICEODB_NS
{
	class FilterBaseType;
	class FilterSection;
	class FilterParam;
	class MaskParam;
	class MaskView;
	class FilterImage;
	class FilterType;
}

namespace ICECORE_NS
{

/**
* @class FilterItemModel
* Модель дерева разделов и фильтров
*/
class ICECORE_EXPORT FilterItemModel : public QAbstractItemModel
{
	Q_OBJECT
private:
	enum
	{
		TypeRole = Qt::UserRole,	///< Роль для типа элемента дерева
		FilterTypeRole,				///< Роль для типа фильтра
		FilterIdRole,				///< Роль для идентификатора фильтра
		FilterDescriptionRole		///< Роль для текстовго описания фильтра
	};

	/// Тип элемента дерева
	enum
	{
		UNDEFUNED_TYPE = 0,	///< Неопределено
		SECTION_TYPE,		///< Раздел
		PARAM_TYPE,			///< Фильтр
		NUM_TYPE			///< Количество типов элементов дерева
	};

public:
	/// Колонки дерева разделов и фильтров
	enum
	{
		NAME_COLNUM = 0,	///< Колонка с названием раздела или фильтра
		NUM_COLS			///< Количество колонок дерева разделов и фильтров
	};

	/**
	* Конструктор
	* @param parent родительский объект
	*/
	FilterItemModel(QObject* parent = 0);

	/**
	* Деструктор
	*/
	~FilterItemModel();

	/**
	* Соединиться с базой данных
	* @param user имя пользователя
	* @param password пароль пользователя
	* @param dbName имя базы данных
	* @param host имя или IP-адрес хоста, где находится СУБД
	* @param port порт СУБД (5432)
	*/
	void initDb(const QString& user, const QString& password, const QString& dbName,
		const QString& host, unsigned short int port) throw(ICECORE_NS::DataBaseException);

	/**
	* Обновить данные типов фильтров в БД, если программа загрузила новые для БД
	* фильтры
	* @param filterTypeList список типов фильтров/названий файлов plugin
	*/
	void setLoadedFilterType(const QStringList& filterTypeList) throw(ICECORE_NS::DataBaseException);

	/**
	* Задать соответсвие типа фильтра и иконки
	* @param filterType тип фильтра/название файла plugin
	* @param icon иконка
	*/
	void setIcon(const QString& filterType, const QIcon& icon);

	/**
	* Добавить раздел в дерево разделов и фильтров в БД
	* @param section раздел
	* @param parent родительский объект для нового раздела
	*/
	void addSection(const std::tr1::shared_ptr<ICEODB_NS::FilterSection>& section,
		const QModelIndex& parent);

	/**
	* Редактировать раздел в дереве разделов и фильтров в БД
	* @param section отредактированный раздел
	*/
	void editSection(const std::tr1::shared_ptr<ICEODB_NS::FilterSection>& section);

	/**
	* Удалить раздел из дерева разделов и фильтров в БД
	* @param modelIndex объект в дереве разделов
	*/
	void removeSection(const QModelIndex& modelIndex);

	/**
	* Добавить фильтр в дерево разделов и фильтров в БД
	* @param param фильтр
	* @param parent родительский объект для нового фильтра
	* @result TODO: comment
	*/
	QModelIndex addParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param,
		const QModelIndex& parent) throw(ICECORE_NS::DataBaseException);

	//TODO refac
	void addChildParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param)
		throw(ICECORE_NS::DataBaseException);

	/**
	* Редактировать фильтр в дереве разделов и фильтров в БД
	* @param param фильтр
	*/
	void editParam(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& param);

	/**
	* Удалить фильтр из дерева разделов и фильтров в БД
	* @param index объект в дереве разделов
	*/
	void removeParam(const QModelIndex& modelIndex);

	void setParentSection(const QModelIndex& modelIndex, const QModelIndex& parentModelIndex);

	QModelIndex getModelIndexOfDuplicateFilterName(const QString& name, unsigned int id,
		const QString& dllName, const QModelIndex& parent) const;

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getFilterParam(const QModelIndex& modelIndex) const;
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getFilterParam(unsigned int id) const;
	std::tr1::shared_ptr<ICEODB_NS::FilterType> getFilterType(const QString& key) const;
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> getChildFilterParam(
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam, int index) const;
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> getMaskParam(
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> filterParam, int index) const;

	virtual Qt::ItemFlags flags(const QModelIndex& modelIndex) const;
	virtual int columnCount(const QModelIndex& parent) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex& modelIndex, int role) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent) const
		 throw(ICECORE_NS::DataBaseException);
	virtual QModelIndex parent(const QModelIndex& child) const;
	virtual int rowCount(const QModelIndex& parent) const;

	template<typename T>
	static T* castFilterToType(void* ptr)
	{
		if(!ptr)
		{
			return 0;
		}
		T* baseTypeItem = static_cast<T*>(ptr);
		if(!baseTypeItem)
		{
			return 0;
		}
		if(baseTypeItem->getTypeName() == typeid(T).name())
		{
			return static_cast<T*>(ptr);
		}
		return 0;
	}

	static bool isFilterSection(const QModelIndex& modelIndex);
	static bool isFilterParam(const QModelIndex& modelIndex);
	static int getType(const QModelIndex& modelIndex);
	static QString getFilterType(const QModelIndex& modelIndex);
	static unsigned int getFilterId(const QModelIndex& modelIndex);
	static QString getFilterDescription(const QModelIndex& modelIndex);

private:
	void eraseSection(ICEODB_NS::FilterSection* section);
	std::tr1::shared_ptr<ICEODB_NS::FilterSection> getRootFilterSection();
	std::tr1::shared_ptr<ICEODB_NS::FilterSection> createRootFilterSection();

	void checkAndCreateFilterRoot();
	void addFilterType(const QString& dllName);
	void fillFilterTypeMap();

	std::auto_ptr<odb::database> m_db;	///< Объект-соединение с БД

	std::tr1::shared_ptr<ICEODB_NS::FilterSection> m_rootSection;

	QMap<QString, std::tr1::shared_ptr<ICEODB_NS::FilterType> > m_filterTypeMap;
	QMap<QString, QIcon> m_filterIconMap;
	mutable QMap<unsigned int, std::tr1::shared_ptr<ICEODB_NS::FilterParam> > m_filterParamMap;
};

} // namespace ICECORE_NS

#endif // FILTERITEMMODEL_H
