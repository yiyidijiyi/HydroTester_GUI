/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-12
* ��      �ߣ�syf
* ��      ����
*/
#include "logindlg.h"
#include "ui_logindlg.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
LoginDlg::LoginDlg(QWidget *parent) 
   : QDialog(parent)
    ,ui(new Ui::loginDlg)
	,m_userType(Admin)
	, m_pAccount(NULL)
{	
	CreateUi();

	m_pAccount = new UserAccount();

	connect(ui->pushButton_login, &QPushButton::clicked, this, &LoginDlg::OnLoginClicked);
	connect(ui->pushButton_cancel, &QPushButton::clicked, this, &LoginDlg::OnCancelClicked);
	connect(ui->pushButton_developer, &QPushButton::clicked, this, &LoginDlg::OnDeveloperClicked);
	connect(ui->pushButton_admin, &QPushButton::clicked, this, &LoginDlg::OnAdminClicked);
	connect(ui->pushButton_tester, &QPushButton::clicked, this, &LoginDlg::OnTesterClicked);
}


/*
* ������
* ���أ�
* ���ܣ���������
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
* ������
* ���أ�
* ���ܣ���¼�����趨
*/
void LoginDlg::CreateUi()
{
	ui->setupUi(this);

	// �̶���¼���ڴ�С
	setFixedSize(1366, 728);

	// �ޱ߿���ʾ
	setWindowFlags(Qt::FramelessWindowHint);

	// ͸������
	//setAttribute(Qt::WA_TranslucentBackground);

	// ���ñ���ͼƬ
	this->setStyleSheet("LoginDlg{ background-image:url(:/login/resource/login/bg.png)}"
		"QLabel{font-family:'Microsoft YaHei';}"
		"QPushButton{font-family:'Microsoft YaHei';}"
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px;color:#979797}");

	// ��¼��
	ui->label_login->setPixmap(QPixmap(":/login/resource/login/dlg.png"));

	// ���õ�¼��ȡ����ť��ʽ
	ui->pushButton_login->setStyleSheet("QPushButton{font-size:18px; color:#979797}"
		"QPushButton{border-image: url(:/login/resource/login/btn3.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/login/resource/login/btn1.png);}");
	ui->pushButton_cancel->setStyleSheet("QPushButton{font-size:18px; color:#979797}"
		"QPushButton{border-image: url(:/login/resource/login/btn3.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/login/resource/login/btn1.png);}");

	// �����˻�����ѡ��ť��ʽ
	ui->pushButton_developer->setStyleSheet("QPushButton{font-size:14px}");
	ui->pushButton_admin->setStyleSheet("QPushButton{font-size:14px;}");
	ui->pushButton_tester->setStyleSheet("QPushButton{font-size:14px;}");
	SwitchAccount(m_userType);

	// �����û����������������ʽ
	ui->lineEdit_name->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setEchoMode(QLineEdit::Password);
	ui->lineEdit_name->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));
	ui->lineEdit_password->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));

	// ���ð�Ȩ˵����ʽ
	ui->label_copyright->setStyleSheet("QLabel {font-size:12px; color:#ffffff;}");

	// �����˻�������ʽ
	ui->label_account->setStyleSheet("QLabel {font-size:24px; color:#979797;}");

	// ������ʾ��Ϣ��ʽ
	ui->label_message->setStyleSheet("QLabel {font-size:12px; color:#ff0000;}");
}


/*
* ������
* ���أ�
* ���ܣ���¼�����趨
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
* ������id--�˻�id
* ���أ�
* ���ܣ�У���½����
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
//		ui->label_message->setText(QStringLiteral("�����˺����ݿ��������ϵ���ң�"));
//	}
//
//	db.close();
//
//	return state;
//}


/*
* ������
* ���أ�
* ���ܣ������¼��ť
*/
void LoginDlg::OnLoginClicked()
{
	bool state = false;
	int id = 0;
	QString userName = ui->lineEdit_name->text();
	QString passward = ui->lineEdit_password->text();
	
	if (userName.isEmpty())
	{
		ui->label_message->setText(QStringLiteral("�˻�������Ϊ�գ�"));
		return;
	}


	if (passward.isEmpty())
	{
		ui->label_message->setText(QStringLiteral("���벻��Ϊ�գ�"));
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
			ui->label_message->setText(QStringLiteral("�û������������"));
		}		
	}

	//bool state = AuthenticateAccount():
}


/*
* ������
* ���أ�
* ���ܣ����ȡ����ť
*/
void LoginDlg::OnCancelClicked()
{
	close();
}


/*
* ������
* ���أ�
* ���ܣ���������߰�ť
*/
void LoginDlg::OnDeveloperClicked()
{
	m_userType = Developer;
	SwitchAccount(m_userType);
	ui->label_account->setText(QStringLiteral("������"));
}


/*
* ������
* ���أ�
* ���ܣ��������Ա��ť
*/
void LoginDlg::OnAdminClicked()
{
	m_userType = Admin;
	SwitchAccount(m_userType);
	ui->label_account->setText(QStringLiteral("����Ա"));
}



/*
* ������
* ���أ�
* ���ܣ��������Ա��ť
*/
void LoginDlg::OnTesterClicked()
{
	m_userType = Tester;
	SwitchAccount(m_userType);
	ui->label_account->setText(QStringLiteral("����Ա"));
}



/*
* ������
* ���أ�
* ���ܣ�
*/