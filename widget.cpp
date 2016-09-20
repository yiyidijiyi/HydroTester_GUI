/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-20
* ��      �ߣ�syf
* ��      ����
*/
#include "widget.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
Widget::Widget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::Widget)
	, m_isMousePressed(false)
	, m_mousePos(QPoint(0, 0))
	, m_pReportQueryModel(NULL)
	, m_interfaceIndex(TestInterface)
	, m_chartIndex(Video)
	, m_accountEditState(Disable)
	, m_methodEditState(Disable)
	, m_methodListIndex(-1)
	, m_currentUnitIndex(0)
	, m_pCom(NULL)
	, m_pRxThread(NULL)
	, m_bIsComOpened(false)
	, m_testState(Init)
	, m_bIsWaterIn(false)
	, m_bIsWaterOut(false)
{
	CreateUi();

	// ��ʼ���û��˺�������ݳ�Ա
	m_pAccountListModel = new QStringListModel(this);

	m_account.userType = 0;
	m_account.userName = "";
	m_account.passward = "";
	m_account.id = 0;

	m_pAccountDB = new UserAccount(this);

	// ��ʼ�����Է���������ݳ�Ա
	m_pMethodListModel = new QStringListModel(this);
	m_pMethodParam = new MethodParam(this);

	// ��ʼ�����Խ����ѯ��س�Ա
	m_pTestResult = new TestResult(this);

	// ��ʼ�������豸
	InitSerialPort();

	/*
	* �����ź����
	*/
	
	connect(ui->pushButton_min, &QPushButton::clicked, this, &Widget::OnBtnMinClicked);
	connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::OnBtnCloseClicked);

	// �豸����
	connect(ui->pushButton_connectCom, &QPushButton::clicked, this, &Widget::OnBtnComOpClicked);
	connect(ui->pushButton_waterIn, &QPushButton::clicked, this, &Widget::OnBtnWaterInClicked);
	connect(ui->pushButton_waterOff, &QPushButton::clicked, this, &Widget::OnBtnWaterOffClicked);
	connect(m_pCom, &SerialPort::DataReceived, this, &Widget::OnRxDataReceived);
	connect(m_pCom, &SerialPort::HandshakeState, this, &Widget::OnHandShakeStateReceived);

	// �����л�
	connect(ui->pushButton_testInterface, &QPushButton::clicked, this, &Widget::OnBtnTestInterfaceClicked);
	connect(ui->pushButton_testMethod, &QPushButton::clicked, this, &Widget::OnBtnTestMethodClicked);
	connect(ui->pushButton_advance, &QPushButton::clicked, this, &Widget::OnBtnAdvanceClicked);
	connect(ui->pushButton_reportQuery, &QPushButton::clicked, this, &Widget::OnBtnReportQueryClicked);
	connect(ui->pushButton_help, &QPushButton::clicked, this, &Widget::OnBtnHelpClicked);
	connect(ui->pushButton_clearInfo, &QPushButton::clicked, ui->textEdit_info, &QTextEdit::clear);

	// ���Խ������
	connect(ui->pushButton_video, &QPushButton::clicked, this, &Widget::OnBtnChartVideoClicked);
	connect(ui->pushButton_curve, &QPushButton::clicked, this, &Widget::OnBtnChartCurveClicked);
	connect(ui->pushButton_report, &QPushButton::clicked, this, &Widget::OnBtnChartReportClicked);
	connect(ui->pushButton_print, &QPushButton::clicked, this, &Widget::OnBtnChartPrintClicked);
	connect(ui->comboBox_selMethod, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombSelMethodChanged);

	// ���Է�������
	connect(ui->listView_methodList, &QListView::clicked, this, &Widget::OnMethodListItemClicked);
	connect(ui->pushButton_addMethod, &QPushButton::clicked, this, &Widget::OnBtnNewMethodClicked);
	connect(ui->comboBox_plan, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombMethodPlanChanged);
	connect(ui->comboBox_unit, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombPressureUnitChanged);
	connect(ui->pushButton_saveMethod, &QPushButton::clicked, this, &Widget::OnBtnSaveMethodClicked);
	connect(ui->pushButton_delMethod, &QPushButton::clicked, this, &Widget::OnBtnDeleteMethodClicked);
	connect(ui->pushButton_modifyMethod, &QPushButton::clicked, this, &Widget::OnBtnModifyMethodClicked);

	// ���Խ����ѯ����
	connect(ui->pushButton_query, &QPushButton::clicked, this, &Widget::OnBtnQueryClicked);
	connect(ui->pushButton_delReport, &QPushButton::clicked, this, &Widget::OnBtnDeleteReportListClicked);

	// �˻���Ϣ����
	connect(ui->listView_accountList, &QListView::clicked, this, &Widget::OnAccountListItemClicked);
	connect(ui->pushButton_newAccount, &QPushButton::clicked, this, &Widget::OnBtnNewAccountClicked);
	connect(ui->pushButton_saveAccount, &QPushButton::clicked, this, &Widget::OnBtnSaveAccountClicked);
	connect(ui->pushButton_delAccount, &QPushButton::clicked, this, &Widget::OnBtnDeleteAccountClicked);
	connect(ui->pushButton_modifyAccount, &QPushButton::clicked, this, &Widget::OnBtnModifyAccountClicked);
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
Widget::~Widget()
{
	if (m_pRxThread)
	{
		m_pRxThread->terminate();
		m_pRxThread->wait();
		delete m_pRxThread;
	}

	if (m_pCom)
	{
		delete m_pCom;
	}

	if (m_pAccountListModel)
	{
		delete m_pAccountListModel;
	}

	if (m_pMethodListModel)
	{
		delete m_pMethodListModel;
	}

	if (m_pAccountDB)
	{
		delete m_pAccountDB;
	}

	if (m_pMethodParam)
	{
		delete m_pMethodParam;
	}

	if (m_pReportQueryModel)
	{
		delete m_pReportQueryModel;
	}

	if (m_pTestResult)
	{
		delete m_pTestResult;
	}

	delete ui;
}


/*
* ������
* ���أ�
* ���ܣ������ʾ����
*/
void Widget::CreateUi()
{
	ui->setupUi(this);

	// �̶���¼���ڴ�С
	setFixedSize(1366, 768);

	// �ޱ߿���ʾ
	setWindowFlags(Qt::FramelessWindowHint);

	// ����tabBar
	ui->tabWidget->tabBar()->hide();

	// ����ͨ����ʽ
	this->setStyleSheet("QLabel{font-family:'Microsoft YaHei'; font-size:14px;color:#979797;}"
		"QPushButton{font-family:'Microsoft YaHei'; font-size:14px;}"
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; }"
		"QTextEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#000000;  background-color:#ffffff}"
		"QDateEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; background-color:#f7f7f7;}"
		"QComboBox{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; background-color:#f7f7f7}");

	// ����״̬��Label��ʽ
	ui->label_testState->setStyleSheet("QLabel{font-size:16px; color:#ffffff;}");
	ui->label_pressure->setStyleSheet("QLabel{font-size:16px; color:#ffffff;}");
	ui->label_holdingTime->setStyleSheet("QLabel{font-size:16px; color:#ffffff;}");

	// ���õײ���Ȩ��ǩ��ʽ
	ui->label_bottom->setStyleSheet("font-size:10px; color:#ffffff");

	// �û���������
	SwitchInterface(m_interfaceIndex);

	/*
	* ���ü�������ʽ
	*/
	ui->tabWidget_testInterface->tabBar()->hide();

	// ������������ˮ����ˮ����ʼ����ͣ���������ť��ʽ
	ui->pushButton_connectCom->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");
	ui->pushButton_waterIn->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");
	ui->pushButton_waterOff->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");
	ui->pushButton_startStop->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");
	ui->pushButton_pauseConti->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");
	ui->pushButton_openCloseCamera->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");

	// ������Ƶ�طš������Ϣ��ť��ʽ
	ui->pushButton_playback->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:16px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn1.png);}");

	ui->pushButton_clearInfo->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:16px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn1.png);}");

	// ͼ����ʾ����ѡ��
	SwitchChart(m_chartIndex);

	// ������ʾ��Ϣ��ʽ
	ui->label_testInterfaceMessage->setStyleSheet("QLabel{color:#ff0000}");

	/*
	* ���ü�ⷽ��������ʽ
	*/
	ui->label_methodList->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_methodParams->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_testMethodMessage->setStyleSheet("QLabel{color:#ff0000}");

	ui->listView_methodList->setStyleSheet("font-family:'Microsoft YaHei'; font-size:14px;");

	// ���������趨
	ui->lineEdit_pressureRate->setValidator(new QDoubleValidator(0, 1000000, 2, this));
	ui->lineEdit_setTime->setValidator(new QDoubleValidator(0, 1000000, 2, this));
	ui->lineEdit_targetPressure->setValidator(new QDoubleValidator(0, 1000000, 2, this));
	ui->lineEdit_pressureCycle->setValidator(new QIntValidator(0, 100, this));
	ui->lineEdit_pressureHolding->setValidator(new QDoubleValidator(0, 1000000, 2, this));


	// ����������ɾ�����޸İ�ť��ʽ
	ui->pushButton_addMethod->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/testMethod/resource/testmethod/methodBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testMethod/resource/testmethod/methodBtn1.png);}");
	ui->pushButton_delMethod->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/testMethod/resource/testmethod/methodBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testMethod/resource/testmethod/methodBtn1.png);}");
	ui->pushButton_modifyMethod->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/testMethod/resource/testmethod/methodBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testMethod/resource/testmethod/methodBtn1.png);}");

	/*
	* ���ý����ѯ������ʽ
	*/
	m_pReportQueryModel = new QStandardItemModel(this);
	CreateReportViewTable();


	// ���õ�ǰ����
	ui->dateEdit_start->setDate(QDate::currentDate().addDays(-1));
	ui->dateEdit_end->setDate(QDate::currentDate());

	// ���õ�����������
	ui->dateEdit_start->setCalendarPopup(true);
	ui->dateEdit_end->setCalendarPopup(true);

	// ���ò�ѯ��ɾ�������ɱ��水ť��ʽ
	ui->pushButton_query->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/main/resource/main/queryBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/main/resource/main/queryBtn1.png);}");
	ui->pushButton_delReport->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/main/resource/main/queryBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/main/resource/main/queryBtn1.png);}");
	ui->pushButton_genReport->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/main/resource/main/queryBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/main/resource/main/queryBtn1.png);}");

	// ������ʾ��Ϣ��ʽ
	ui->label_queryMessage->setStyleSheet("QLabel{color:#ff0000}");

	/*
	* ���ø߼����ý�����ʽ
	*/
	ui->label_accountList->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_accountInfo->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_advanceMessage->setStyleSheet("QLabel{color:#ff0000}");

	ui->pushButton_newAccount->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/advance/resource/advance/advanceBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/advance/resource/advance/advanceBtn1.png);}");
	ui->pushButton_delAccount->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/advance/resource/advance/advanceBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/advance/resource/advance/advanceBtn1.png);}");
	ui->pushButton_modifyAccount->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/advance/resource/advance/advanceBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/advance/resource/advance/advanceBtn1.png);}");

	ui->pushButton_pressureCali->setStyleSheet("QPushButton{font-family:'Microsoft YaHei'; font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/login/resource/login/btn4.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/login/resource/login/btn2.png);}");

	ui->listView_accountList->setStyleSheet("font-family:'Microsoft YaHei'; font-size:14px;");

	// �û���������ֻ������ĸ�����ֹ���
	ui->lineEdit_accountName->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));
	ui->lineEdit_accountPassword->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));
}


/*
* ������
* ���أ�
* ���ܣ����ɱ����ѯ����ʽ
*/
void Widget::CreateReportViewTable()
{
	m_pReportQueryModel->clear();
	m_pReportQueryModel->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("���")));
	m_pReportQueryModel->setHorizontalHeaderItem(1, new QStandardItem(QStringLiteral("��������")));
	m_pReportQueryModel->setHorizontalHeaderItem(2, new QStandardItem(QStringLiteral("��������")));
	m_pReportQueryModel->setHorizontalHeaderItem(3, new QStandardItem(QStringLiteral("������ʽ")));
	m_pReportQueryModel->setHorizontalHeaderItem(4, new QStandardItem(QStringLiteral("�û�")));

	ui->tableView_reportQuery->setModel(m_pReportQueryModel);

	// ����Ϊ�п�Ϊ265px
	ui->tableView_reportQuery->setColumnWidth(0, 265);
	ui->tableView_reportQuery->setColumnWidth(1, 265);
	ui->tableView_reportQuery->setColumnWidth(2, 265);
	ui->tableView_reportQuery->setColumnWidth(3, 265);
	ui->tableView_reportQuery->setColumnWidth(4, 265);

	// ������ͷ
	ui->tableView_reportQuery->verticalHeader()->hide();
	// ��������ѡ��
	ui->tableView_reportQuery->setSelectionBehavior(QAbstractItemView::SelectRows);
	// ���õ�Ԫ��ֻ��
	ui->tableView_reportQuery->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// ����horizontalHeader�������ͣ������С
	m_pReportQueryModel->horizontalHeaderItem(0)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(1)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(2)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(3)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(4)->setFont(QFont("Microsoft YaHei", 12));

	// ����horizontalHeader������ɫ
	m_pReportQueryModel->horizontalHeaderItem(0)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(1)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(2)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(3)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(4)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
}


/*
* ������
* ���أ�
* ���ܣ��л��û���������
*/
void Widget::SwitchInterface(InterfaceIndex index)
{
	ui->pushButton_testInterface->setIcon(QIcon(":/main/resource/main/testInterface0.png"));
	ui->pushButton_testMethod->setIcon(QIcon(":/main/resource/main/testMethod0.png"));
	ui->pushButton_reportQuery->setIcon(QIcon(":/main/resource/main/reportQuery0.png"));
	ui->pushButton_advance->setIcon(QIcon(":/main/resource/main/advance0.png"));
	ui->pushButton_help->setIcon(QIcon(":/main/resource/main/help0.png"));

	switch (index)
	{
	case TestInterface:
		ui->pushButton_testInterface->setIcon(QIcon(":/main/resource/main/testInterface.png"));
		break;
	case TestMethod:
		ui->pushButton_testMethod->setIcon(QIcon(":/main/resource/main/testMethod.png"));
		break;
	case ReportQuery:
		ui->pushButton_reportQuery->setIcon(QIcon(":/main/resource/main/reportQuery.png"));
		break;
	case Advance:
		ui->pushButton_advance->setIcon(QIcon(":/main/resource/main/advance.png"));
		break;
	case Help:
		ui->pushButton_help->setIcon(QIcon(":/main/resource/main/help.png"));
		break;
	default:
		ui->pushButton_help->setIcon(QIcon(":/main/resource/main/help.png"));
		break;
	}

	ui->tabWidget->setCurrentIndex(static_cast<int>(index));
}


/*
* ������
* ���أ�
* ���ܣ��л�ͼ����ʾ
*/
void Widget::SwitchChart(ChartIndex index)
{
	ui->pushButton_video->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn4.png);}");
	ui->pushButton_curve->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn4.png);}");
	ui->pushButton_report->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn4.png);}");
	ui->pushButton_print->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton:{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn4.png);}");

	switch (index)
	{
	case Video:
		ui->pushButton_video->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
		break;
	case Curve:
		ui->pushButton_curve->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
		break;
	case Report:
		ui->pushButton_report->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
		break;
	default:
		ui->pushButton_video->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
		break;
	}

	ui->tabWidget_testInterface->setCurrentIndex(static_cast<int>(index));
}


/*
* ������
* ���أ�
* ���ܣ��������ڲ������û�����
*/
Widget::InterfaceIndex Widget::GetInterfaceIndex()
{
	return m_interfaceIndex;
}


/*
* ������
* ���أ�
* ���ܣ�����ϵͳ�����б�
*/
void Widget::InitSerialPort()
{
	m_pCom = new SerialPort(this);
	m_pRxThread = new QThread;

	m_pCom->moveToThread(m_pRxThread);
	m_pRxThread->start();

	QStringList comList = m_pCom->GetComList();
	
	ui->comboBox_selCom->addItems(comList);
}


/*
* ������
* ���أ�
* ���ܣ����ڲ�����ť��ʾ
*/
void Widget::UpdateComUI()
{
	if (m_bIsComOpened)
	{
		ui->pushButton_connectCom->setText(QStringLiteral("�Ͽ�"));
	}
	else
	{
		ui->pushButton_connectCom->setText(QStringLiteral("����"));
	}
}


/*
* ������
* ���أ�
* ���ܣ������˻���Ϣ�б�
*/
void Widget::UpdateAccountList()
{
	QStringList accountList;
	QStringList testerList;
	QStringList adminList;
	bool state = false;

	// ����Ա��Ȩ�޸��˻���Ϣ������ʾ�˻��б�
	if (static_cast<int>(Tester) == m_account.userType)
	{
		return;
	}

	if (static_cast<int>(Developer) == m_account.userType)
	{
		state = m_pAccountDB->GetAccountList(adminList, Admin);

		// ������
		if (!state)
		{
			ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
			return;
		}

		state = m_pAccountDB->GetAccountList(testerList, Tester);

		// ������
		if (!state)
		{
			ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
			return;
		}

		accountList = (testerList + adminList);
	}
	else
	{
		state = m_pAccountDB->GetAccountList(testerList, Tester);

		// ������
		if (!state)
		{
			ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
			return;
		}

		// ����ӹ���Ա�˺ű���
		accountList.append(m_account.userName);

		// ��Ӳ���Ա�˺��б�
		accountList += testerList;
	}

	m_pAccountListModel->setStringList(accountList);
	ui->listView_accountList->setModel(m_pAccountListModel);

	ui->comboBox_queryUserName->clear();
	ui->comboBox_queryUserName->addItem(QStringLiteral("��ѡ���û�"));
	ui->comboBox_queryUserName->addItems(accountList);
	
}


/*
* ������
* ���أ�
* ���ܣ����²��Է����б�
*/
void Widget::UpdateTestMethodList()
{
	QStringList methodList;
	bool state = false;
	state = m_pMethodParam->GetMethodList(methodList);

	if (!state)
	{
		ui->label_testMethodMessage->setText(m_pMethodParam->GetMessageList()[0]);
		return;
	}

	m_pMethodListModel->setStringList(methodList);
	ui->listView_methodList->setModel(m_pMethodListModel);

	ui->comboBox_selMethod->clear();
	ui->comboBox_selMethod->addItem(QStringLiteral("��ѡ����Է���"));
	ui->comboBox_selMethod->addItems(methodList);
	ui->comboBox_selMethod->setCurrentIndex(0);

	ui->comboBox_queryMethodName->clear();
	ui->comboBox_queryMethodName->addItem(QStringLiteral("��ѡ�񷽷�����"));
	ui->comboBox_queryMethodName->addItems(methodList);
}


/*
* ������reportList:��ѯ���ı����б�
* ���أ�
* ���ܣ����²��Խ����ѯ��
*/
void Widget::UpdateReportQueryView(const QList<STRUCT_Reprot> &reportList)
{
	CreateReportViewTable();

	size_t size = m_reportList.size();

	for (size_t i = 0; i < size; i++)
	{
		QString strMode = "";
		switch (reportList[i].endMode)
		{
		case 0:
			strMode = QStringLiteral("�Զ�����");
			break;
		case 1:
			strMode = QStringLiteral("�ֶ�����");
			break;
		case 2:
			strMode = QStringLiteral("������");
			break;
		default:
			break;
		}
		m_pReportQueryModel->setItem(i, 0, new QStandardItem(QString::number(reportList[i].id)));
		m_pReportQueryModel->setItem(i, 1, new QStandardItem(reportList[i].methodName));
		m_pReportQueryModel->setItem(i, 2, new QStandardItem(reportList[i].testDate));
		m_pReportQueryModel->setItem(i, 3, new QStandardItem(strMode));
		m_pReportQueryModel->setItem(i, 4, new QStandardItem(reportList[i].userName));

		m_pReportQueryModel->item(i, 0)->setTextAlignment(Qt::AlignCenter);
		m_pReportQueryModel->item(i, 1)->setTextAlignment(Qt::AlignCenter);
		m_pReportQueryModel->item(i, 2)->setTextAlignment(Qt::AlignCenter);
		m_pReportQueryModel->item(i, 3)->setTextAlignment(Qt::AlignCenter);
		m_pReportQueryModel->item(i, 4)->setTextAlignment(Qt::AlignCenter);

		m_pReportQueryModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
		m_pReportQueryModel->item(i, 1)->setForeground(QBrush(QColor(0, 0, 0)));
		m_pReportQueryModel->item(i, 2)->setForeground(QBrush(QColor(0, 0, 0)));
		m_pReportQueryModel->item(i, 3)->setForeground(QBrush(QColor(0, 0, 0)));
		m_pReportQueryModel->item(i, 4)->setForeground(QBrush(QColor(0, 0, 0)));

		m_pReportQueryModel->item(i, 0)->setFont(QFont("Microsoft YaHei", 10, QFont::Black));
		m_pReportQueryModel->item(i, 1)->setFont(QFont("Microsoft YaHei", 10, QFont::Black));
		m_pReportQueryModel->item(i, 2)->setFont(QFont("Microsoft YaHei", 10, QFont::Black));
		m_pReportQueryModel->item(i, 3)->setFont(QFont("Microsoft YaHei", 10, QFont::Black));
		m_pReportQueryModel->item(i, 4)->setFont(QFont("Microsoft YaHei", 10, QFont::Black));
	}
}


/*
* ������state-�༭״̬
* ���أ�
* ���ܣ������˺���Ϣ��ʾ
*/
void Widget::UpdateAcountInfoUI(UIState state)
{
	switch (state)
	{
	case New:
		ui->lineEdit_accountName->setEnabled(true);
		ui->lineEdit_accountPassword->setEnabled(true);

		ui->comboBox_accountType->setEnabled(true);
		ui->comboBox_accountType->clear();
		if (static_cast<int>(Developer) == m_account.userType)
		{
			ui->comboBox_accountType->addItem(QStringLiteral("����Ա"));
			ui->comboBox_accountType->addItem(QStringLiteral("����Ա"));
		}
		else if (static_cast<int>(Admin) == m_account.userType)
		{
			ui->comboBox_accountType->addItem(QStringLiteral("����Ա"));
		}
		break;
	case Editable:
		ui->lineEdit_accountName->setEnabled(false);
		ui->lineEdit_accountPassword->setEnabled(true);

		ui->comboBox_accountType->setEnabled(false);
		break;
	case Disable:		
	default:
		ui->lineEdit_accountName->setEnabled(false);
		ui->lineEdit_accountPassword->setEnabled(false);

		ui->comboBox_accountType->clear();
		ui->comboBox_accountType->addItem(QStringLiteral("����Ա"));
		ui->comboBox_accountType->addItem(QStringLiteral("����Ա"));
		ui->comboBox_accountType->addItem(QStringLiteral("������"));
		break;
	}
}


/*
* ������
* ���أ�
* ���ܣ��ж�Ŀ��ѹ��ֵ�Ƿ�����
*/
bool Widget::IsPressureOverload(double p)
{
	bool state = false;

	if (p < 1000000.0)
	{
		state = true;
	}

	return state;
}


void Widget::ConvertPressureUnit(double &pressure, ENUM_PressureUnit unit0, ENUM_PressureUnit unit1)
{
	switch (unit0)
	{
	case Pa:
		switch (unit1)
		{
		case kPa:
			pressure /= 1000;
			break;
		case mBar:
			pressure /= 100;
			break;
		case mmH2O:
			pressure /= 9.8;
			break;
		default:
			break;
		}
		break;
	case kPa:
		switch (unit1)
		{
		case Pa:
			pressure *= 1000;
			break;
		case mBar:
			pressure *= 10;
			break;
		case mmH2O:
			pressure *= (1000 / 9.8);
			break;
		default:
			break;
		}
		break;
	case mBar:
		switch (unit1)
		{
		case Pa:
			pressure *= 100;
			break;
		case kPa:
			pressure /= 10;
			break;
		case mmH2O:
			pressure *= (100 / 9.8);
			break;
		default:
			break;
		}
		break;
	case mmH2O:
		switch (unit1)
		{
		case Pa:
			pressure *= 9.8;
			break;
		case kPa:
			pressure *= 0.0098;
			break;
		case mBar:
			pressure *= 0.098;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}


/*
* ������method--���Է����������
* ���أ�
* ���ܣ����ݲ��Է�������ʾ������Ϣ
*/
void Widget::ShowMethodParam(const STRUCT_MethodParam &method)
{
	ui->textEdit_methodInfo->append(QStringLiteral("�������ƣ�") + method.name);
	ui->textEdit_methodInfo->append(QStringLiteral("Ӧ�ñ�׼��") + method.standard);
	ui->textEdit_methodInfo->append(QStringLiteral("������") + method.discription);

	QString unit;

	switch (method.uint)
	{
	case 0:
		unit = QStringLiteral("Pa(��)");
		break;
	case 1:
		unit = QStringLiteral("kPa(ǧ��)");
		break;
	case 2:
		unit = QStringLiteral("mBar(����)");
		break;
		unit = QStringLiteral("mmH2O(����ˮ��)");
	case 3:
		break;
	default:
		break;
	}

	switch (method.plan)
	{
	case 0:
		ui->textEdit_methodInfo->append(QStringLiteral("���Է�����") + QStringLiteral("������ѹ"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ѹ���ʣ�") + QString::number(method.rate) + unit + QStringLiteral("/����"));
		break;
	case 1:
		ui->textEdit_methodInfo->append(QStringLiteral("���Է�����") + QStringLiteral("��ʱ��ѹ"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ѹ���ʣ�") + QString::number(method.rate) + unit + QStringLiteral("/����"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ʱʱ�䣺") + QString::number(method.timing) +  QStringLiteral("����"));
		break;
	case 2:
		ui->textEdit_methodInfo->append(QStringLiteral("���Է�����") + QStringLiteral("��ʱ��ѹ"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ѹ���ʣ�") + QString::number(method.rate) + unit + QStringLiteral("/����"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ʱʱ�䣺") + QString::number(method.timing) + QStringLiteral("����"));
		ui->textEdit_methodInfo->append(QStringLiteral("Ŀ��ѹǿ��") + QString::number(method.pressure) + unit);
		break;
	case 3:
		ui->textEdit_methodInfo->append(QStringLiteral("���Է�����") + QStringLiteral("�����ɳ�"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ѹ���ʣ�") + QString::number(method.rate) + unit + QStringLiteral("/����"));
		ui->textEdit_methodInfo->append(QStringLiteral("Ŀ��ѹǿ��") + QString::number(method.pressure) + unit);
		ui->textEdit_methodInfo->append(QStringLiteral("�������ڣ�") + QString::number(method.cycle) + QStringLiteral("��"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ѹʱ�䣺") + QString::number(method.holdingTime) + QStringLiteral("����"));
		break;
	case 4:
		ui->textEdit_methodInfo->append(QStringLiteral("���Է�����") + QStringLiteral("��ˮ©ˮ"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ѹ���ʣ�") + QString::number(method.rate) + unit + QStringLiteral("/����"));
		ui->textEdit_methodInfo->append(QStringLiteral("��ʱʱ�䣺") + QString::number(method.timing) + QStringLiteral("����"));
		ui->textEdit_methodInfo->append(QStringLiteral("Ŀ��ѹǿ��") + QString::number(method.pressure) + unit);
		break;
	default:
		break;
	}
}


/*
* ������id:ѡ����Ŀ��id
* ���أ�
* ���ܣ�ɾ�����Խ���б���ѡ�е���Ŀ
*/
void Widget::DeleteReportInList(int id)
{
	size_t size = m_reportList.size();
	
	for (size_t i = 0; i < size; i++)
	{
		if (id != m_reportList[i].id)
		{
			continue;
		}
		else
		{
			m_reportList.removeAt(i);
			break;
		}
	}
}


/*
* ������
* ���أ�
* ���ܣ����²��Է�����ʾ
*/
void Widget::UpdateMethodInfoUI(UIState state)
{
	int index = 0;

	switch(state)
	{
	case New:
		ui->comboBox_plan->setEnabled(true);
		ui->comboBox_plan->setCurrentIndex(0);
		ui->comboBox_unit->setCurrentIndex(0);
		m_currentUnitIndex = 0;

		ui->lineEdit_methodName->clear();
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->clear();
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->clear();
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->textEdit_discription->clear();
		ui->textEdit_discription->setEnabled(true);

		OnCombMethodPlanChanged(0);
		break;
	case Editable:
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->textEdit_discription->setEnabled(true);

		ui->comboBox_plan->setEnabled(true);
		index = ui->comboBox_plan->currentIndex();
		OnCombMethodPlanChanged(index);
		break;
	case Disable:
		ui->comboBox_plan->setEnabled(false);

		ui->lineEdit_methodName->setEnabled(false);
		ui->lineEdit_standard->setEnabled(false);
		ui->lineEdit_pressureRate->setEnabled(false);
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->setEnabled(false);
		ui->textEdit_discription->setEnabled(false);
	default:
		break;
	}
}


/*
* ������
* ���أ�
* ���ܣ���С����ť�ۺ���
*/
void Widget::OnBtnMinClicked()
{
	this->showMinimized();
}


/*
* ������
* ���أ�
* ���ܣ��رհ�ť�ۺ���
*/
void Widget::OnBtnCloseClicked()
{
	this->close();
}


/*
* ������
* ���أ�
* ���ܣ�������¼�
*/
void Widget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_mousePos = event->pos();

		if ((m_mousePos.x() < DRAG_POS_MIN_X) || (m_mousePos.x() > DRAG_POS_MAX_X) || (m_mousePos.y() < DRAG_POS_MIN_Y) || (m_mousePos.y() > DRAG_POS_MAX_Y))
		{
			m_isMousePressed = false;
		}
		else
		{
			m_isMousePressed = true;
		}
	}
}


/*
* ������
* ���أ�
* ���ܣ�����ƶ��¼�
*/
void Widget::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isMousePressed)
	{
		QPoint pos = event->globalPos() - m_mousePos;

		move(pos);
	}
}


/*
* ������
* ���أ�
* ���ܣ�����ͷ��¼�
*/
void Widget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isMousePressed = false;
	}
}


/*
* ������
* ���أ�
* ���ܣ���������Խ��桱���ۺ���
*/
void Widget::OnBtnTestInterfaceClicked()
{
	m_interfaceIndex = TestInterface;
	SwitchInterface(m_interfaceIndex);
}


/*
* ������
* ���أ�
* ���ܣ���������Է��������ۺ���
*/
void Widget::OnBtnTestMethodClicked()
{
	m_interfaceIndex = TestMethod;
	SwitchInterface(m_interfaceIndex);
}



/*
* ������
* ���أ�
* ���ܣ�����������ѯ�����ۺ���
*/
void Widget::OnBtnReportQueryClicked()
{
	m_interfaceIndex = ReportQuery;
	SwitchInterface(m_interfaceIndex);
}


/*
* ������
* ���أ�
* ���ܣ�������߼����á����ۺ���
*/
void Widget::OnBtnAdvanceClicked()
{
	m_interfaceIndex = Advance;
	SwitchInterface(m_interfaceIndex);
}


/*
* ������
* ���أ�
* ���ܣ�������û����������ۺ���
*/
void Widget::OnBtnHelpClicked()
{
	m_interfaceIndex = Help;
	SwitchInterface(m_interfaceIndex);
}


/*
* ������
* ���أ�
* ���ܣ�ͼ����ʾѡ����Ƶ����
*/
void Widget::OnBtnChartVideoClicked()
{
	m_chartIndex = Video;
	SwitchChart(m_chartIndex);
}


/*
* ������
* ���أ�
* ���ܣ�ͼ����ʾѡ�����߹���
*/
void Widget::OnBtnChartCurveClicked()
{
	m_chartIndex = Curve;
	SwitchChart(m_chartIndex);
}


/*
* ������
* ���أ�
* ���ܣ�ͼ����ʾѡ�񱨸湦��
*/
void Widget::OnBtnChartReportClicked()
{
	m_chartIndex = Report;
	SwitchChart(m_chartIndex);
}


/*
* ������
* ���أ�
* ���ܣ�ͼ����ʾѡ���ӡ����
*/
void Widget::OnBtnChartPrintClicked()
{
	
}


/*
* ������
* ���أ�
* ���ܣ�����ѡ��Ĳ��Է�������ʾ���Է�����Ϣ
*/
void Widget::OnCombSelMethodChanged(int index)
{
	ui->textEdit_methodInfo->clear();

	if (index < 1)
	{
		return;
	}

	bool state = false;

	state = m_pMethodParam->GetMethodInfo(index - 1, m_methodParam);

	if (!state)
	{
		ui->label_testInterfaceMessage->setText(m_pMethodParam->GetMessageList()[0]);
		ui->comboBox_selMethod->setCurrentIndex(0);
	}
	else
	{
		ShowMethodParam(m_methodParam);
	}
}


/*
* ������id--�˺������ݿ���е�id��
* ���أ�
* ���ܣ���½�ɹ��󣬽���������ǰ��������
*/
void Widget::OnLoginAccepted(int id)
{
	bool state = false;
	state = m_pAccountDB->GetAccountInfo(id, m_account);

	if (!state)
	{
		
	}

	UpdateAccountList();

	UpdateTestMethodList();
}


/*
* ������
* ���أ�
* ���ܣ��򿪻�رմ��ڲۺ���
*/
void Widget::OnBtnComOpClicked()
{
	if (m_bIsComOpened)
	{
		m_pCom->Close();
		m_bIsComOpened = false;
		m_testState = Init;
		ui->textEdit_info->append(QStringLiteral("���豸�Ͽ����ӣ�"));
	}
	else
	{
		m_bIsComOpened = m_pCom->Open(ui->comboBox_selCom->currentText());

		if (m_bIsComOpened)
		{
			ui->textEdit_info->append(QStringLiteral("�򿪴��ڳɹ���"));
			m_pCom->TxReadState();
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("�򿪴���ʧ�ܣ����鴮���豸��"));
		}
	}

	UpdateComUI();
}


/*
* ������
* ���أ�
* ���ܣ���ˮ��ֹͣ��ˮ����ۺ���
*/
void Widget::OnBtnWaterInClicked()
{
	// ��ˮ����:0x07�� ��ʼ��ˮ��1��ֹͣ��ˮ��0
	if (m_bIsWaterIn)
	{
		m_pCom->TxCmd(0x07, 0x0, 0x0);
		m_bIsWaterIn = false;
		ui->pushButton_waterIn->setText(QStringLiteral("��ˮ"));
	}
	else
	{
		m_pCom->TxCmd(0x07, 0x01, 0x0);
		m_bIsWaterIn = true;
		ui->pushButton_waterIn->setText(QStringLiteral("ֹͣ��ˮ"));
	}
}


/*
* ������
* ���أ�
* ���ܣ���ˮ��ֹͣ��ˮ����ۺ���
*/
void Widget::OnBtnWaterOffClicked()
{
	// ��ˮ����:0x08�� ��ʼ��ˮ��1��ֹͣ��ˮ��0
	if (m_bIsWaterOut)
	{
		m_pCom->TxCmd(0x08, 0x0, 0x0);
		m_bIsWaterOut = false;
		ui->pushButton_waterOff->setText(QStringLiteral("��ˮ"));
	}
	else
	{
		m_pCom->TxCmd(0x08, 0x01, 0x0);
		m_bIsWaterOut = true;
		ui->pushButton_waterOff->setText(QStringLiteral("ֹͣ��ˮ"));
	}
}


/*
* ������
* ���أ�
* ���ܣ���ʼ��ֹͣ��������ۺ���
*/
void Widget::OnBtnStartTestClicked()
{
	if ((Connected == m_testState) || (End == m_testState))
	{
		// ��������״̬����Խ���״̬����ʼ�еĲ��ԣ������ò���
		if (0 == ui->comboBox_selMethod->currentIndex())
		{
			ui->textEdit_info->append(QStringLiteral("����ѡ����ȷ�Ĳ��Է�����"));
			return;
		}

		m_pCom->TxSetParam(m_methodParam);


	}
	else if (Start == m_testState)
	{
		// ����Ϊ����״̬��ֹͣ����
		m_pCom->TxCmd(0x01, 0x0, 0x0);
	}

}


/*
* ������
* ���أ�
* ���ܣ���ͣ�������������ۺ���
*/
void Widget::OnBtnPauseTestClicked()
{
	if (Start == m_testState)
	{
		// ����Ϊ����״̬����ͣ����
		m_pCom->TxCmd(0x01, 0xff, 0x0);
	}
	else if (Pause == m_testState)
	{
		// ������ͣ״̬�����Լ�������
		m_pCom->TxCmd(0x01, static_cast<quint8>(m_methodParam.plan), 0x0);
	}
}


/*
* ������rxBuf--���յ��Ĵ�������
* ���أ�
* ���ܣ���ʾ���յ���������
*/
void Widget::OnRxDataReceived(const QByteArray &rxBuf)
{
	QString str;
	HexToString(rxBuf, str);
	ui->textEdit_info->append(str);
}


/*
* ������handshake--���豸ͨ�ŵ�����״̬
* ���أ�
* ���ܣ��������豸ͨ�ŵ�����״̬������Ӧ����ʾ�봦��
*/
void Widget::OnHandShakeStateReceived(STRUCT_HandShake &handshake)
{
	switch (handshake.state)
	{
	case SetParamOk:
		// ֻ�ڿ�ʼ����ǰ�Ż����ò���
		ui->textEdit_info->append(QStringLiteral("���ò����ɹ���"));

		// ���Ͳ��Կ�ʼ����
		m_pCom->TxCmd(0x01, static_cast<quint8>(m_methodParam.plan), 0x0);
		break;
	case SetParamError:
		ui->textEdit_info->append(QStringLiteral("���ò���ʧ�ܣ�"));
		break;
	case SetParamAckTimeOut:
		ui->textEdit_info->append(QStringLiteral("���ò������ȴ�Ӧ��ʱ���������豸�����������"));
		break;
	case ReadParamOk:
		ui->textEdit_info->append(QStringLiteral("��ȡ�����ɹ���"));
		break;
	case ReadParamError:
		ui->textEdit_info->append(QStringLiteral("��ȡ����ʧ�ܣ�"));
		break;
	case ReadParamAckTimeOut:
		ui->textEdit_info->append(QStringLiteral("��ȡ�������ȴ�Ӧ��ʱ���������豸�����������"));
		break;
	case ReadStateOk:
		if (Init == m_testState)
		{
			ui->textEdit_info->append(QStringLiteral("���豸�����ɹ���"));
			m_testState = Connected;
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("��ȡ�豸ʵʱ������״̬�ɹ���"));
		//}	
		break;
	case ReadStateError:
		if (Init == m_testState)
		{
			ui->textEdit_info->append(QStringLiteral("���豸�����ɹ���"));
			m_testState = Connected;
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("��ȡ�豸ʵʱ������״̬ʧ�ܣ�"));
		//}
		break;
	case ReadStateAckTimeOut:
		if (Init == m_testState)
		{
			ui->textEdit_info->append(QStringLiteral("���豸����ʧ�ܣ��������豸�����������"));
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("��ȡ�豸ʵʱ������״̬���ȴ�Ӧ��ʱ���������豸�����������"));
		//}	
		break;
	case CmdOk:
		QString strCmd;
		switch(handshake.cmd)
		{
		case 0x01:
			if ((Connected == m_testState) || (End == m_testState))
			{
				// �������óɹ�����ʼ��������ɹ����µĲ��Կ�ʼ
				m_testState = Start;
				ui->textEdit_info->append(QStringLiteral("��ʼ����:"));
			}
			else if (Start == m_testState)
			{

			}
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0b:
			break;
		default:
			break;
		}
		ui->textEdit_info->append(QStringLiteral("���豸�Ĳ�������ɹ���"));
		break;
	case CmdError:
		ui->textEdit_info->append(QStringLiteral("���豸�Ĳ�������ʧ�ܣ�"));
		break;
	case CmdAckTimeOut:
		ui->textEdit_info->append(QStringLiteral("���豸�Ĳ�������ȴ�Ӧ��ʱ���������豸�����������"));
		break;
	default:
		break;
	}
}


/*
* ������index--ListView�б�ѡ�е�item����
* ���أ�
* ���ܣ�����ѡ�е��˺ţ���ʾ�˺���Ϣ
*/
void Widget::OnAccountListItemClicked(const QModelIndex &index)
{
	m_accountEditState = Disable;
	UpdateAcountInfoUI(m_accountEditState);
	int i = index.row();
	QString userName = m_pAccountListModel->stringList().at(i);
	STRUCT_Account account;
	bool state = false;

	state = m_pAccountDB->GetAccountInfo(userName, account);

	if (!state)
	{
		ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
	}
	else
	{
		ui->lineEdit_accountName->setText(account.userName);
		ui->lineEdit_accountPassword->setText(account.passward);
		ui->comboBox_accountType->setCurrentIndex(account.userType - 1);
	}
}


/*
* ������
* ���أ�
* ���ܣ������˻���ť�ۺ���
*/
void Widget::OnBtnNewAccountClicked()
{
	// ���������༭״̬ʱ���ٴε��������ť��Ч
	if (New == m_accountEditState)
	{
		return;
	}

	if (m_account.userType > static_cast<int>(Tester))
	{
		m_accountEditState = New;
		UpdateAcountInfoUI(m_accountEditState);
		
		// �������
		ui->lineEdit_accountName->clear();
		ui->lineEdit_accountPassword->clear();
		ui->comboBox_accountType->setCurrentIndex(0);
	}
}


/*
* ������
* ���أ�
* ���ܣ������˻���Ϣ
*/
void Widget::OnBtnSaveAccountClicked()
{
	STRUCT_Account account;
	account.userName = ui->lineEdit_accountName->text();
	account.passward = ui->lineEdit_accountPassword->text();
	account.userType = ui->comboBox_accountType->currentIndex() + 1;

	if (account.userName.isEmpty())
	{
		ui->label_advanceMessage->setText(QStringLiteral("�û�������Ϊ�գ�"));
		return;
	}

	if (account.passward.isEmpty())
	{
		ui->label_advanceMessage->setText(QStringLiteral("���벻��Ϊ�գ�"));
		return;
	}

	bool state = false;

	if (New == m_accountEditState)
	{
		state = m_pAccountDB->AddAccount(account);
	}
	else if (Editable == m_accountEditState)
	{
		state = m_pAccountDB->UpdateAccount(account);
	}

	if (!state)
	{
		ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
	}
	else
	{
		UpdateAccountList();
	}

	m_accountEditState = Disable;
	UpdateAcountInfoUI(m_accountEditState);
	ui->comboBox_accountType->setCurrentIndex(account.userType - 1);
}


/*
* ������
* ���أ�
* ���ܣ�ɾ���û��˻���ť�ۺ���
*/
void Widget::OnBtnDeleteAccountClicked()
{
	// �����༭ʱ��ɾ����ť��Ч
	if (New == m_accountEditState)
	{
		return;
	}

	QString userName = ui->lineEdit_accountName->text();

	// �˻���ϢΪ��ʱ��ɾ����ť��Ч
	if (userName.isEmpty())
	{
		return;
	}

	if (m_account.userType > static_cast<int>(Tester))
	{
		// ����ɾ���˻�����
		if (userName == m_account.userName)
		{
			ui->label_advanceMessage->setText(QStringLiteral("�޷�ɾ���ѵ�¼���˺ţ�"));
			return;
		}

		bool state = false;

		state = m_pAccountDB->DeleteAccount(userName);

		if (!state)
		{
			ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
		}
		else
		{
			UpdateAccountList();

			m_accountEditState = Disable;
			UpdateAcountInfoUI(m_accountEditState);
			ui->lineEdit_accountName->clear();
			ui->lineEdit_accountPassword->clear();
			ui->comboBox_accountType->setCurrentIndex(0);
		}
	}
}


/*
* ������
* ���أ�
* ���ܣ��޸��û��˻���ť�ۺ���
*/
void Widget::OnBtnModifyAccountClicked()
{
	// ���������༭ʱ������޸İ�ť��Ч
	if (New == m_accountEditState)
	{
		return;
	}

	// �����޸ı༭ʱ������޸İ�ť��Ч
	if (Editable == m_accountEditState)
	{
		return;
	}

	// �˻�����Ϊ��ʱ������޸İ�ť��Ч
	if (ui->lineEdit_accountName->text().isEmpty())
	{
		return;
	}

	if (m_account.userType > static_cast<int>(Tester))
	{
		m_accountEditState = Editable;
		UpdateAcountInfoUI(m_accountEditState);
	}
}


/*
* ������
* ���أ�
* ���ܣ�������Է����б�ʱ����ʾ��Ӧ�Ĳ��Է�������
*/
void Widget::OnMethodListItemClicked(const QModelIndex &index)
{
	bool state = false;
	m_methodListIndex = index.row();
	STRUCT_MethodParam method;
	
	// ����б����ݣ��л����Ǳ༭״̬
	m_methodEditState = Disable;
	UpdateMethodInfoUI(m_methodEditState);

	state = m_pMethodParam->GetMethodInfo(m_methodListIndex, method);

	if (!state)
	{
		ui->label_testMethodMessage->setText(m_pMethodParam->GetMessageList()[0]);
		return;
	}

	ui->lineEdit_methodName->setText(method.name);
	ui->comboBox_plan->setCurrentIndex(method.plan);
	ui->lineEdit_standard->setText(method.standard);
	ui->textEdit_discription->setText(method.discription);
	ui->comboBox_unit->setCurrentIndex(method.uint);
	m_currentUnitIndex = method.uint;

	// ���ݲ��Է���������ʾ���Է�������
	switch (method.plan)
	{
	case 0:	// ������ѹ
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->clear();
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case 1:	// ��ʱ��ѹ
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->setText(QString::number(method.timing));
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case 2:	// ��ʱ��ѹ
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->setText(QString::number(method.timing));
		ui->lineEdit_targetPressure->setText(QString::number(method.pressure));
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case 3:	// �����ɳ�
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->clear();
		ui->lineEdit_targetPressure->setText(QString::number(method.pressure));
		ui->lineEdit_pressureCycle->setText(QString::number(method.cycle));
		ui->lineEdit_pressureHolding->setText(QString::number(method.holdingTime));
		break;
	case 4:	// ��ˮ©ˮ
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->setText(QString::number(method.timing));
		ui->lineEdit_targetPressure->setText(QString::number(method.pressure));
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	default:
		ui->label_testMethodMessage->setText(QStringLiteral("���Է�����������"));
		break;
	}
}


/*
* ������
* ���أ�
* ���ܣ��������Է�����ť�ۺ���
*/
void Widget::OnBtnNewMethodClicked()
{
	if (New != m_methodEditState)
	{
		m_methodEditState = New;
		UpdateMethodInfoUI(m_methodEditState);
	}
}


/*
* ������index--ѡ���������������
* ���أ�
* ���ܣ����Է���������ѡ��ı�ۺ���
*/
void Widget::OnCombMethodPlanChanged(int index)
{
	// �Ǳ༭״̬��������±༭״̬
	if (Disable == m_methodEditState)
	{
		return;
	}

	switch (index)
	{
	case 0:	// ������ѹ
		ui->lineEdit_setTime->clear();
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	case 1:	// ��ʱ��ѹ
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	case 2:	// ��ʱ��ѹ
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	case 3:	// �����ɳ�
		ui->lineEdit_setTime->clear();
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->setEnabled(true);
		ui->lineEdit_pressureHolding->setEnabled(true);
		break;
	case 4:	// ��ˮ©ˮ
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	default:
		break;
	}
}


/*
* ������index--ѹǿ��λ����
* ���أ�
* ���ܣ�����ѹǿ��λ�ı䣬�Զ�ת��ѹ��ֵ
*/
void Widget::OnCombPressureUnitChanged(int index)
{
	QString strP1 = ui->lineEdit_pressureRate->text();
	QString strP2 = ui->lineEdit_targetPressure->text();

	if (!strP1.isEmpty())
	{
		double pressure1 = strP1.toDouble();
		ConvertPressureUnit(pressure1, static_cast<ENUM_PressureUnit>(m_currentUnitIndex), static_cast<ENUM_PressureUnit>(index));
		ui->lineEdit_pressureRate->setText(QString::number(pressure1));
	}

	if (!strP2.isEmpty())
	{
		double pressure2 = strP2.toDouble();
		ConvertPressureUnit(pressure2, static_cast<ENUM_PressureUnit>(m_currentUnitIndex), static_cast<ENUM_PressureUnit>(index));
		ui->lineEdit_targetPressure->setText(QString::number(pressure2));
	}

	m_currentUnitIndex = index;
}


/*
* ������
* ���أ�
* ���ܣ�������Է���������ť�ۺ���
*/
void Widget::OnBtnSaveMethodClicked()
{
	// �Ǳ༭״̬������Ҫ��������
	if (Disable == m_methodEditState)
	{
		return;
	}

	int index = ui->comboBox_plan->currentIndex();
	STRUCT_MethodParam method;
	bool state = false;

	method.name = ui->lineEdit_methodName->text();
	method.plan = index;
	method.standard = ui->lineEdit_standard->text();
	method.discription = ui->textEdit_discription->toPlainText();
	method.uint = ui->comboBox_unit->currentIndex();

	method.rate = ui->lineEdit_pressureRate->text().toDouble();
	method.timing = ui->lineEdit_setTime->text().toDouble();
	method.pressure = ui->lineEdit_targetPressure->text().toDouble();
	method.cycle = ui->lineEdit_pressureCycle->text().toInt();
	method.holdingTime = ui->lineEdit_pressureHolding->text().toDouble();

	if (method.name.isEmpty())
	{
		ui->label_testMethodMessage->setText(QStringLiteral("�������Ʋ���Ϊ�գ�"));
		return;
	}

	// �������Ƿ���ȷ
	switch (index)
	{
	case 0:	// ������ѹ
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ����ѹ����ֵ��"));
			return;
		}
		break;
	case 1:	// ��ʱ��ѹ
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ����ѹ����ֵ��"));
			return;
		}

		if (method.timing < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ�Ķ�ʱʱ��ֵ��"));
			return;
		}
		break;
	case 2:	// ��ʱ��ѹ
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ����ѹ����ֵ��"));
			return;
		}

		if (method.timing < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ�Ķ�ʱʱ��ֵ��"));
			return;
		}

		if (method.pressure < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ��Ŀ��ѹǿֵ��"));
			return;
		}
		break;
	case 3:	// �����ɳ�
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ����ѹ����ֵ��"));
			return;
		}

		if (method.pressure < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ��Ŀ��ѹǿֵ��"));
			return;
		}

		if (method.cycle < 1)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ����������ֵ��"));
			return;
		}

		if (method.holdingTime < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ�ı�ѹʱ��ֵ��"));
			return;
		}
		break;
	case 4:	// ��ˮ©ˮ
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ����ѹ����ֵ��"));
			return;
		}

		if (method.timing < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ�Ķ�ʱʱ��ֵ��"));
			return;
		}

		if (method.pressure < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("��������ȷ��Ŀ��ѹǿֵ��"));
			return;
		}
		break;
	default:
		break;
	}

	if (New == m_methodEditState)
	{
		state = m_pMethodParam->AddMethod(method);

		if (state)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("�������Է����ɹ���"));

			// �����б�
			UpdateTestMethodList();

			// �༭״̬�л�ΪDisable
			m_methodEditState = Disable;
			UpdateMethodInfoUI(m_methodEditState);
		}
		else
		{
			ui->label_testMethodMessage->setText(m_pMethodParam->GetMessageList()[0]);
		}
	}
	else if (Editable == m_methodEditState)
	{
		state = m_pMethodParam->UpdateMethodParam(m_methodListIndex, method);

		if (state)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("�޸Ĳ��Է����ɹ���"));

			// �����б�
			UpdateTestMethodList();

			// �༭״̬�л�ΪDisable
			m_methodEditState = Disable;
			UpdateMethodInfoUI(m_methodEditState);
		}
		else
		{
			ui->label_testMethodMessage->setText(m_pMethodParam->GetMessageList()[0]);
		}
	}
}


/*
* ������
* ���أ�
* ���ܣ������б���ѡ�е���Ŀ��ɾ��һ�����Է���
*/
void Widget::OnBtnDeleteMethodClicked()
{
	// �����༭ʱ��ɾ����ť��Ч
	if (New == m_methodEditState)
	{
		return;
	}

	if (m_methodListIndex >= 0)
	{
		bool state = false;

		state = m_pMethodParam->DeleteMethod(m_methodListIndex);

		if (state)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("ɾ�����Է����ɹ���"));

			// �����б�
			UpdateTestMethodList();

			// �����趨Ϊδѡ��״̬�������ԭ�еĲ�����ʾ
			m_methodListIndex = -1;

			ui->lineEdit_methodName->clear();
			ui->lineEdit_standard->clear();
			ui->lineEdit_pressureRate->clear();
			ui->lineEdit_setTime->clear();
			ui->lineEdit_targetPressure->clear();
			ui->lineEdit_pressureCycle->clear();
			ui->lineEdit_pressureHolding->clear();
			ui->textEdit_discription->clear();

			ui->comboBox_plan->setCurrentIndex(0);
			ui->comboBox_unit->setCurrentIndex(0);

			// �༭״̬�л�ΪDisable
			m_methodEditState = Disable;
			UpdateMethodInfoUI(m_methodEditState);
		}
		else
		{
			ui->label_testMethodMessage->setText(m_pMethodParam->GetMessageList()[0]);
		}
	}
	else
	{
		ui->label_testMethodMessage->setText(QStringLiteral("δѡ����Ч�Ĳ��Է�����Ŀ��"));
	}
}


/*
* ������
* ���أ�
* ���ܣ��޸Ĳ��Է���������ť�ۺ���
*/
void Widget::OnBtnModifyMethodClicked()
{
	if (m_methodListIndex < 0)
	{
		ui->label_testMethodMessage->setText(QStringLiteral("δѡ����Ч�Ĳ��Է�����Ŀ��"));
		return;
	}

	// �������޸ı༭״̬�£��޸İ�ť��Ч
	if ((New == m_methodEditState) || (Editable == m_methodEditState))
	{
		return;
	}

	m_methodEditState = Editable;
	UpdateMethodInfoUI(m_methodEditState);
}


/*
* ������
* ���أ�
* ���ܣ�����ѡ�����������ѯ���Խ�����ݿ�
*/
void Widget::OnBtnQueryClicked()
{
	int methodPlan = -1;
	int index = -1;
	QString methodName = "";
	QString userName = "";
	//QString startDate = ui->dateEdit_start->dateTime().toString("yyyy-MM-dd hh:mm:ss");
	//QString endDate = ui->dateEdit_end->dateTime().toString("yyyy-MM-dd hh:mm:ss");

	QDateTime startDate = ui->dateEdit_start->dateTime();
	QDateTime endDate = ui->dateEdit_end->dateTime();

	index = ui->comboBox_queryTestMethod->currentIndex();

	if (index > 0)
	{
		methodPlan = index - 1;
	}

	index = ui->comboBox_queryMethodName->currentIndex();

	if (index > 0)
	{
		methodName = ui->comboBox_queryMethodName->currentText();
	}

	index = ui->comboBox_queryUserName->currentIndex();

	if (index > 0)
	{
		userName = ui->comboBox_queryUserName->currentText();
	}

	bool state = m_pTestResult->GetPeportList(m_reportList, startDate, endDate, methodPlan, methodName, userName);
	

	if (!state)
	{
		ui->label_queryMessage->setText(m_pTestResult->GetMessageList()[0]);
	}
	else
	{
		UpdateReportQueryView(m_reportList);
	}
}


/*
* ������
* ���أ�
* ���ܣ�ɾ��ѡ�еĲ��Խ������
*/
void Widget::OnBtnDeleteReportListClicked()
{
	QModelIndexList selectedList = ui->tableView_reportQuery->selectionModel()->selectedIndexes();
	int i = 0, j = -1;
	int id = 0;
	bool state = false;
	QModelIndex selection;

	foreach(selection, selectedList)
	{
		i = selection.row();

		if (j == i)
		{
			continue;
		}
		else
		{
			j = i;
		}

		id = m_pReportQueryModel->item(i, 0)->text().toInt();

		state = m_pTestResult->DeleteReport(id);

		if (!state)
		{
			ui->label_queryMessage->setText(m_pTestResult->GetMessageList()[0]);
			break;
		}
		else
		{
			DeleteReportInList(id);
		}
	}

	if (state)
	{
		UpdateReportQueryView(m_reportList);
	}
}

/*
* ������
* ���أ�
* ���ܣ�
*/