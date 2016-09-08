/*
* 创建日期：2016-09-02
* 最后修改：2016-09-07
* 作      者：syf
* 描      述：
*/
#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QPalette>
#include <QLabel>

namespace Ui {
class loginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
	enum AccountType{
		Developer	= 0,
		Admin		= 1,
		Tester		= 2
	};
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

	void CreateUi(void);
	void SwitchAccount(AccountType type);

public slots:
	void OnLoginClicked();
	void OnCancelClicked();
	void OnDeveloperClicked();
	void OnAdminClicked();
	void OnTesterClicked();

private:
    Ui::loginDlg *ui;

	AccountType m_accountType;
};

#endif // LOGINDLG_H
