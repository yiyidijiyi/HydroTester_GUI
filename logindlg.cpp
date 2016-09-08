/*
* 创建日期：2016-09-02
* 最后修改：2016-09-07
* 作      者：syf
* 描      述：
*/
#include "logindlg.h"
#include "ui_logindlg.h"


/*
* 参数：
* 返回：
* 功能：构造函数
*/
LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDlg),
	m_accountType(Admin)
{	
	CreateUi();

	connect(ui->pushButton_login, &QPushButton::clicked, this, &LoginDlg::OnLoginClicked);
	connect(ui->pushButton_cancel, &QPushButton::clicked, this, &LoginDlg::OnCancelClicked);
	connect(ui->pushButton_developer, &QPushButton::clicked, this, &LoginDlg::OnDeveloperClicked);
	connect(ui->pushButton_admin, &QPushButton::clicked, this, &LoginDlg::OnAdminClicked);
	connect(ui->pushButton_tester, &QPushButton::clicked, this, &LoginDlg::OnTesterClicked);
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
LoginDlg::~LoginDlg()
{
    delete ui;
}


/*
* 参数：
* 返回：
* 功能：登录界面设定
*/
void LoginDlg::CreateUi()
{
	ui->setupUi(this);

	// 固定登录窗口大小
	setFixedSize(1366, 728);

	// 无边框显示
	setWindowFlags(Qt::FramelessWindowHint);

	// 透明背景
	//setAttribute(Qt::WA_TranslucentBackground);

	// 设置背景图片
	this->setStyleSheet("LoginDlg{ background-image:url(:/login/resource/login/bg.png)}"
		"QLabel{font-family:'Microsoft YaHei';}"
		"QPushButton{font-family:'Microsoft YaHei';}"
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px;color:#979797}");

	// 登录框
	ui->label_login->setPixmap(QPixmap(":/login/resource/login/dlg.png"));

	// 设置登录，取消按钮样式
	ui->pushButton_login->setStyleSheet("QPushButton{font-size:18px; color:#979797}"
		"QPushButton{border-image: url(:/login/resource/login/btn3.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/login/resource/login/btn1.png);}");
	ui->pushButton_cancel->setStyleSheet("QPushButton{font-size:18px; color:#979797}"
		"QPushButton{border-image: url(:/login/resource/login/btn3.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/login/resource/login/btn1.png);}");

	// 设置账户类型选择按钮样式
	ui->pushButton_developer->setStyleSheet("QPushButton{font-size:14px}");
	ui->pushButton_admin->setStyleSheet("QPushButton{font-size:14px;}");
	ui->pushButton_tester->setStyleSheet("QPushButton{font-size:14px;}");
	SwitchAccount(m_accountType);

	// 设置用户名、密码输入框样式
	ui->lineEdit_name->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setEchoMode(QLineEdit::Password);

	// 设置版权说明样式
	ui->label_copyright->setStyleSheet("QLabel {font-size:12px; color:#ffffff;}");

	// 设置账户类型样式
	ui->label_account->setStyleSheet("QLabel {font-size:24px; color:#979797;}");
}


/*
* 参数：
* 返回：
* 功能：登录界面设定
*/
void LoginDlg::SwitchAccount(AccountType type)
{
	ui->pushButton_developer->setStyleSheet("QPushButton{color:#979797; border-image: url(:/login/resource/login/btn4.png);}");
	ui->pushButton_admin->setStyleSheet("QPushButton{color:#979797; border-image: url(:/login/resource/login/btn4.png);}");
	ui->pushButton_tester->setStyleSheet("QPushButton{color:#979797; border-image: url(:/login/resource/login/btn4.png);}");

	switch (type)
	{
	case Developer:
		ui->pushButton_developer->setStyleSheet("QPushButton{color:#ffffff; border-image: url(:/login/resource/login/btn2.png);}");
		break;
	case Admin:
		ui->pushButton_admin->setStyleSheet("QPushButton{color:#ffffff; border-image: url(:/login/resource/login/btn2.png);}");
		break;
	case Tester:
		ui->pushButton_tester->setStyleSheet("QPushButton{color:#ffffff; border-image: url(:/login/resource/login/btn2.png);}");
		break;
	default:
		ui->pushButton_tester->setStyleSheet("QPushButton{color:#ffffff; border-image: url(:/login/resource/login/btn2.png);}");
		break;
	}
}


/*
* 参数：
* 返回：
* 功能：点击登录按钮
*/
void LoginDlg::OnLoginClicked()
{
	accept();
}


/*
* 参数：
* 返回：
* 功能：点击取消按钮
*/
void LoginDlg::OnCancelClicked()
{
	close();
}


/*
* 参数：
* 返回：
* 功能：点击开发者按钮
*/
void LoginDlg::OnDeveloperClicked()
{
	m_accountType = Developer;
	SwitchAccount(m_accountType);
	ui->label_account->setText(QStringLiteral("开发者"));
}


/*
* 参数：
* 返回：
* 功能：点击管理员按钮
*/
void LoginDlg::OnAdminClicked()
{
	m_accountType = Admin;
	SwitchAccount(m_accountType);
	ui->label_account->setText(QStringLiteral("管理员"));
}



/*
* 参数：
* 返回：
* 功能：点击测试员按钮
*/
void LoginDlg::OnTesterClicked()
{
	m_accountType = Tester;
	SwitchAccount(m_accountType);
	ui->label_account->setText(QStringLiteral("测试员"));
}



/*
* 参数：
* 返回：
* 功能：
*/