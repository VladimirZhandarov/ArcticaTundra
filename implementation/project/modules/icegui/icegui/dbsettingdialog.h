#ifndef DBSETTINGDIALOG_H
#define DBSETTINGDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_dbsettingdialog.h"

namespace ICEGUI_NS
{

/**
* @class DbSettingDialog
* Диалоговое окно настройки соединения с базой данных
*/
class ICEGUI_EXPORT DbSettingDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param user имя пользователя
	* @param password пароль пользователя
	* @param dbName имя базы данных
	* @param host имя или IP-адрес хоста, где находится СУБД
	* @param port порт СУБД (5432)
	* @param parent родительский объект
	* @param f флаги окна
	*/
	DbSettingDialog(const QString& user, const QString& password, const QString& dbName,
		const QString& host, unsigned int port, QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* Деструктор
	*/
	~DbSettingDialog();

	/**
	* Получить имя пользователя
	* @return имя пользователя
	*/
	QString getUser() const;

	/**
	* Получить пароль пользователя
	* @return пароль пользователя
	*/
	QString getPassword() const;

	/**
	* Получить имя базы данных
	* @return имя базы данных
	*/
	QString getDbName() const;

	/**
	* Получить имя или IP-адрес хоста, где находится СУБД
	* @return имя или IP-адрес хоста, где находится СУБД
	*/
	QString getHost() const;

	/**
	* Получить порт СУБД
	* @return порт СУБД
	*/
	unsigned int getPort() const;

private:
	Ui::DbSettingDialog ui;	///< Ui класс
};

} // namespace ICEGUI_NS

#endif // DBSETTINGDIALOG_H
