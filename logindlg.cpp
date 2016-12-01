/*
* 创建日期：2016-09-02
* 最后修改：2016-09-12
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
LoginDlg::LoginDlg(QWidget *parent) 
   : QDialog(parent)
    ,ui(new Ui::loginDlg)
	,m_userType(Admin)
	, m_pAccount(NULL)
{	
	CreateUi();

	m_pAccount = new UserAccount(this);

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
	if (m_pAccount)
	{
		delete m_pAccount;
	}

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
	SwitchAccount(m_userType);

	// 设置用户名、密码输入框样式
	ui->lineEdit_name->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setEchoMode(QLineEdit::Password);
	ui->lineEdit_name->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));
	ui->lineEdit_password->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));

	// 设置版权说明样式
	ui->label_copyright->setStyleSheet("QLabel {font-size:12px; color:#ffffff;}");

	// 设置账户类型样式
	ui->label_account->setStyleSheet("QLabel {font-size:24px; color:#979797;}");

	// 设置提示消息样式
	ui->label_message->setStyleSheet("QLabel {font-size:12px; color:#ff0000;}");
}


/*
* 参数：
* 返回：
* 功能：登录界面设定
*/
void LoginDlg::SwitchAccount(ENUM_AccountType type)
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
* 参数：id--账户id
* 返回：
* 功能：校验登陆密码
*/
//bool LoginDlg::AuthenticateAccount(int &id, const QString &userName, const QString &passward)
//{
//	bool state = false;
//	QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
//	db.setDatabaseName("./data/account.db");
//	db.setPassword("caep17305");
//
//	if (db.open())
//	{
//		//QString strQuery = "SELECT id FROM WHERE userType = ? and userName = ? and userPassward = ?";
//		QString strQuery = "SELECT id FROM account WHERE userName = ? AND userPassward = ? AND userType = ?";
//		QSqlQuery query(strQuery, db);	
//		query.bindValue(0, userName);
//		query.bindValue(1, passward);
//		query.bindValue(2, static_cast<int>(m_userType));
//
//		query.exec();
//
//		int count = 0;
//
//		//QString errStr = query.lastError().text();
//
//		while (query.next())
//		{
//			count++;
//			id = query.value(0).toInt();
//		}
//
//		if (1 == count)
//		{
//			state = true;
//		}
//	}
//	else
//	{
//		ui->label_message->setText(QStringLiteral("连接账号数据库出错，请联系厂家！"));
//	}
//
//	db.close();
//
//	return state;
//}


/*
* 参数：
* 返回：
* 功能：点击登录按钮
*/
void LoginDlg::OnLoginClicked()
{
	bool state = false;
	int id = 0;
	QString userName = ui->lineEdit_name->text();
	QString passward = ui->lineEdit_password->text();
	
	if (userName.isEmpty())
	{
		ui->label_message->setText(QStringLiteral("账户名不能为空！"));
		return;
	}


	if (passward.isEmpty())
	{
		ui->label_message->setText(QStringLiteral("密码不能为空！"));
		return;
	}

	state = m_pAccount->AuthenticateAccount(id, userName, passward, static_cast<int>(m_userType));

	if (state)
	{
		emit AccountID(id);
		accept();
	}
	else
	{
		QStringList message = m_pAccount->GetMessageList();
		size_t size = message.size();

		if (size > 0)
		{
			ui->label_message->setText(message[0]);
		}
		else
		{
			ui->label_message->setText(QStringLiteral("用户名或密码错误！"));
		}		
	}

	//bool state = AuthenticateAccount():
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
	m_userType = Developer;
	SwitchAccount(m_userType);
	ui->label_account->setText(QStringLiteral("开发者"));
}


/*
* 参数：
* 返回：
* 功能：点击管理员按钮
*/
void LoginDlg::OnAdminClicked()
{
	m_userType = Admin;
	SwitchAccount(m_userType);
	ui->label_account->setText(QStringLiteral("管理员"));
}



/*
* 参数：
* 返回：
* 功能：点击测试员按钮
*/
void LoginDlg::OnTesterClicked()
{
	m_userType = Tester;
	SwitchAccount(m_userType);
	ui->label_account->setText(QStringLiteral("测试员"));
}



/*
* 参数：
* 返回：
* 功能：
*/