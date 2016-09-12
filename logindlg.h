/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-12
* ��      �ߣ�syf
* ��      ����
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

#include "common.h"

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
	bool AuthenticateAccount(int &id, const QString &userName, const QString &passward);

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

	ENUM_AccountType m_accountType;
};

#endif // LOGINDLG_H
