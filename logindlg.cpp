/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-07
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
* ������
* ���أ�
* ���ܣ���������
*/
LoginDlg::~LoginDlg()
{
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
	SwitchAccount(m_accountType);

	// �����û����������������ʽ
	ui->lineEdit_name->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setStyleSheet("QLineEdit{border-image: url(:/login/resource/login/input.png);}");
	ui->lineEdit_password->setEchoMode(QLineEdit::Password);

	// ���ð�Ȩ˵����ʽ
	ui->label_copyright->setStyleSheet("QLabel {font-size:12px; color:#ffffff;}");

	// �����˻�������ʽ
	ui->label_account->setStyleSheet("QLabel {font-size:24px; color:#979797;}");
}


/*
* ������
* ���أ�
* ���ܣ���¼�����趨
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
* ������
* ���أ�
* ���ܣ������¼��ť
*/
void LoginDlg::OnLoginClicked()
{
	accept();
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
	m_accountType = Developer;
	SwitchAccount(m_accountType);
	ui->label_account->setText(QStringLiteral("������"));
}


/*
* ������
* ���أ�
* ���ܣ��������Ա��ť
*/
void LoginDlg::OnAdminClicked()
{
	m_accountType = Admin;
	SwitchAccount(m_accountType);
	ui->label_account->setText(QStringLiteral("����Ա"));
}



/*
* ������
* ���أ�
* ���ܣ��������Ա��ť
*/
void LoginDlg::OnTesterClicked()
{
	m_accountType = Tester;
	SwitchAccount(m_accountType);
	ui->label_account->setText(QStringLiteral("����Ա"));
}



/*
* ������
* ���أ�
* ���ܣ�
*/