#ifndef DBSETTINGDIALOG_H
#define DBSETTINGDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_dbsettingdialog.h"

namespace ICEGUI_NS
{

/**
* @class DbSettingDialog
* ���������� ���� ��������� ���������� � ����� ������
*/
class ICEGUI_EXPORT DbSettingDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* �����������
	* @param user ��� ������������
	* @param password ������ ������������
	* @param dbName ��� ���� ������
	* @param host ��� ��� IP-����� �����, ��� ��������� ����
	* @param port ���� ���� (5432)
	* @param parent ������������ ������
	* @param f ����� ����
	*/
	DbSettingDialog(const QString& user, const QString& password, const QString& dbName,
		const QString& host, unsigned int port, QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* ����������
	*/
	~DbSettingDialog();

	/**
	* �������� ��� ������������
	* @return ��� ������������
	*/
	QString getUser() const;

	/**
	* �������� ������ ������������
	* @return ������ ������������
	*/
	QString getPassword() const;

	/**
	* �������� ��� ���� ������
	* @return ��� ���� ������
	*/
	QString getDbName() const;

	/**
	* �������� ��� ��� IP-����� �����, ��� ��������� ����
	* @return ��� ��� IP-����� �����, ��� ��������� ����
	*/
	QString getHost() const;

	/**
	* �������� ���� ����
	* @return ���� ����
	*/
	unsigned int getPort() const;

private:
	Ui::DbSettingDialog ui;	///< Ui �����
};

} // namespace ICEGUI_NS

#endif // DBSETTINGDIALOG_H
