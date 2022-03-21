#include "dbsettingdialog.h"

namespace ICEGUI_NS
{

DbSettingDialog::DbSettingDialog(const QString& user, const QString& password,
	const QString& dbName, const QString& host, unsigned int port, QWidget* parent,
	Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
	ui.userLE->setText(user);
	ui.passwordLE->setText(password);
	ui.dbNameLE->setText(dbName);
	ui.hostLE->setText(host);
	ui.portSB->setValue(port);
}

DbSettingDialog::~DbSettingDialog()
{

}

QString DbSettingDialog::getUser() const
{
	return ui.userLE->text();
}

QString DbSettingDialog::getPassword() const
{
	return ui.passwordLE->text();
}

QString DbSettingDialog::getDbName() const
{
	return ui.dbNameLE->text();
}

QString DbSettingDialog::getHost() const
{
	return ui.hostLE->text();
}

unsigned int DbSettingDialog::getPort() const
{
	return ui.portSB->value();
}

} // namespace ICEGUI_NS
