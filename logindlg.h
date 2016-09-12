/*
* 创建日期：2016-09-02
* 最后修改：2016-09-12
* 作      者：syf
* 描      述：
*/
#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QPalette>
#include <QLabel>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "useraccount.h"

namespace Ui {
class loginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

	void CreateUi(void);
	void SwitchAccount(ENUM_AccountType type);
//	bool AuthenticateAccount(int &id, const QString &userName, const QString &passward);

public slots:
	void OnLoginClicked();
	void OnCancelClicked();
	void OnDeveloperClicked();
	void OnAdminClicked();
	void OnTesterClicked();

signals:
	void AccountID(int id);

private:
    Ui::loginDlg *ui;

	ENUM_AccountType m_userType;
	UserAccount *m_pAccount;
};

#endif // LOGINDLG_H
