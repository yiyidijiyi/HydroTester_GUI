/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-11-18
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
	, m_pCamera(NULL)
	, m_bIsComOpened(false)
	, m_pImgProc(NULL)
	, m_pImgProcThread(NULL)
	, m_dropNum(0)
	, m_testState(Init)
	, m_btnState(Init)
	, m_txData(TxNoData)
	, m_pTimer(NULL)
	, m_bIsParamsSet(false)
	, m_bIsWaterIn(false)
	, m_bIsWaterOff(false)
	, m_pCurve(NULL)
	, m_pGrid(NULL)
	, m_pVideoWriter(NULL)
	, m_oldSize(0)
	, m_maxY(0)
	, m_avgY(0)
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

	// ���
	m_pCamera = new MerCamera(this);

	// ʶ���㷨��ʼ��
	InitImageProc();

	// ѹ������
	InitCurve();

	// ��ʼ����ʾʵ��ʱ����豸��ǰѹ��ֵ
	m_time = QTime(0, 0, 0);
	ui->lcdNumber_time->setDigitCount(8);
	ui->lcdNumber_pressure->setDigitCount(8);
	ui->lcdNumber_time->display(m_time.toString("hh:mm:ss"));
	ui->lcdNumber_pressure->display(QString::number(0));

	// ������ʱ��
	m_pTimer = new QTimer(this);
	m_pTimer->start(200);

	// �ɲ�����ť
	SetDeviceOprateEnabled(ConnectDevice | Camera);

	// ����˫�����ɱ༭
	ui->listView_accountList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->listView_methodList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableView_reportQuery->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableView_reportQuery->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableView_reportQuery->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->textEdit_report->setReadOnly(true);
	ui->textEdit_info->setReadOnly(true);
	ui->textEdit_methodInfo->setReadOnly(true);

	/*
	* �����ź����
	*/
	
	connect(ui->pushButton_min, &QPushButton::clicked, this, &Widget::OnBtnMinClicked);
	connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::OnBtnCloseClicked);

	// �豸����
	connect(ui->pushButton_openCloseCamera, &QPushButton::clicked, this, &Widget::OnBtnOpenCameraClicked);
	connect(ui->pushButton_connectCom, &QPushButton::clicked, this, &Widget::OnBtnComOpClicked);
	connect(ui->pushButton_waterIn, &QPushButton::clicked, this, &Widget::OnBtnWaterInClicked);
	connect(ui->pushButton_waterOff, &QPushButton::clicked, this, &Widget::OnBtnWaterOffClicked);
	connect(ui->pushButton_startStop, &QPushButton::clicked, this, &Widget::OnBtnStartTestClicked);
	connect(ui->pushButton_pauseConti, &QPushButton::clicked, this, &Widget::OnBtnPauseTestClicked);
	connect(ui->pushButton_playback, &QPushButton::clicked, this, &Widget::OnBtnPlayBackClicked);
#ifdef DATA_DEBUG
	connect(m_pCom, &SerialPort::DataReceived, this, &Widget::OnRxDataReceived);
#endif
	//qRegisterMetaType<STRUCT_HandShake*>();
	connect(m_pCom, &SerialPort::HandshakeState, this, &Widget::OnHandShakeStateReceived);
	connect(ui->pushButton_print, &QPushButton::clicked, this, &Widget::OnBtnPrintReportClicked);

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

	// �߼����ò���
	connect(ui->pushButton_saveParams, &QPushButton::clicked, this, &Widget::OnBtnSaveAdvanceParamsClicked);
	connect(ui->comboBox_selMethod, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombSelMethodChanged);

	// ���Է�������
	connect(ui->listView_methodList, &QListView::clicked, this, &Widget::OnMethodListItemClicked);
	connect(ui->pushButton_addMethod, &QPushButton::clicked, this, &Widget::OnBtnNewMethodClicked);
	connect(ui->comboBox_plan, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombMethodPlanChanged);
	connect(ui->comboBox_unit, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombPressureUnitChanged);
	connect(ui->pushButton_saveMethod, &QPushButton::clicked, this, &Widget::OnBtnSaveMethodClicked);
	connect(ui->pushButton_delMethod, &QPushButton::clicked, this, &Widget::OnBtnDeleteMethodClicked);
	connect(ui->pushButton_modifyMethod, &QPushButton::clicked, this, &Widget::OnBtnModifyMethodClicked);
	connect(ui->pushButton_saveCurve, &QPushButton::clicked, this, &Widget::OnBtnSaveCurveClicked);

	// ���Խ����ѯ����
	connect(ui->pushButton_query, &QPushButton::clicked, this, &Widget::OnBtnQueryClicked);
	connect(ui->pushButton_delReport, &QPushButton::clicked, this, &Widget::OnBtnDeleteReportListClicked);

	// �˻���Ϣ����
	connect(ui->listView_accountList, &QListView::clicked, this, &Widget::OnAccountListItemClicked);
	connect(ui->pushButton_newAccount, &QPushButton::clicked, this, &Widget::OnBtnNewAccountClicked);
	connect(ui->pushButton_saveAccount, &QPushButton::clicked, this, &Widget::OnBtnSaveAccountClicked);
	connect(ui->pushButton_delAccount, &QPushButton::clicked, this, &Widget::OnBtnDeleteAccountClicked);
	connect(ui->pushButton_modifyAccount, &QPushButton::clicked, this, &Widget::OnBtnModifyAccountClicked);
	connect(ui->comboBox_pressureRange, static_cast<void (QComboBox::*)(int) >(&QComboBox::currentIndexChanged), this, &Widget::OnCombPressureRangeChanged);

	// ͼ����ʾ
	connect(m_pImgProc, &ImageProc::ImageProcessed, this, &Widget::OnImagePrepared);

	// ��ʱ��
	connect(m_pTimer, &QTimer::timeout, this, &Widget::OnTimer);
	//connect(m_pCom, &SerialPort::DeviceStateReceived, this, &Widget::OnUpdateDeviceState);
#ifdef DATA_DEBUG
	connect(m_pCom, &SerialPort::CurrentPressure, this, &Widget::OnCurrentPressureReceived);
#endif
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

	if (m_pCamera)
	{
		delete m_pCamera;
	}

	if (m_pImgProcThread)
	{
		m_pImgProcThread->terminate();
		m_pImgProcThread->wait();
		delete m_pImgProcThread;
	}

	if (m_pImgProc)
	{
		delete m_pImgProc;
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

	if (m_pCurve)
	{
		delete m_pCurve;
	}

	if (m_pGrid)
	{
		delete m_pGrid;
	}

	if (m_pTimer)
	{
		m_pTimer->stop();
		delete m_pTimer;
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

	// ����lcdnumber����
	QPalette lcdtimepat = ui->lcdNumber_time->palette();
	lcdtimepat.setColor(QPalette::Normal, QPalette::WindowText, Qt::white);
	ui->lcdNumber_time->setPalette(lcdtimepat);

	QPalette lcdpressurepat = ui->lcdNumber_pressure->palette();
	lcdpressurepat.setColor(QPalette::Normal, QPalette::WindowText, Qt::white);
	ui->lcdNumber_pressure->setPalette(lcdpressurepat);

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
	//ui->pushButton_saveCurve->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
	//	"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn2.png);}");

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

	ui->lineEdit_dropRec->setValidator(new QIntValidator(1, 10, this));
	ui->lineEdit_dropRec->setToolTip(QStringLiteral("�������÷�Χֵ1~10"));
	ui->lineEdit_flowInTime->setValidator(new QIntValidator(1, 1000, this));
	ui->lineEdit_flowInTime->setToolTip(QStringLiteral("�������÷�Χֵ1~1000"));
	ui->lineEdit_dropArea->setValidator(new QDoubleValidator(0.01, 10000.0, 2, this));
	ui->lineEdit_dropArea->setToolTip(QStringLiteral("�������÷�Χֵ0.01~5.00"));
	ui->lineEdit_sensitivity->setValidator(new QIntValidator(1, 5, this));
	ui->lineEdit_sensitivity->setToolTip(QStringLiteral("�������÷�Χֵ1~5"));
	ui->lineEdit_ignoreWidth->setValidator(new QIntValidator(0, 1024, this));
	ui->lineEdit_ignoreWidth->setToolTip(QStringLiteral("�������÷�Χֵ0~100"));
	ui->lineEdit_centerX->setValidator(new QIntValidator(0, 1024, this));
	ui->lineEdit_centerX->setToolTip(QStringLiteral("�������÷�Χֵ0~1024"));
	ui->lineEdit_centerY->setValidator(new QIntValidator(0, 1024, this));
	ui->lineEdit_centerY->setToolTip(QStringLiteral("�������÷�Χֵ0~1024"));
	ui->lineEdit_r->setValidator(new QIntValidator(1, 1024, this));
	ui->lineEdit_r->setToolTip(QStringLiteral("�������÷�Χֵ1~1024"));


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
* ������image:��ʾ��ͼ������
* ���أ�
* ���ܣ���ʾͼ��
*/
void Widget::ShowImage(Mat &image)
{
	QImage showImage;
	Mat resizeImage;
	Mat rgbImage;

	

	cv::resize(image, resizeImage, Size(ui->label_video->width(), ui->label_video->height()));

	if (m_pVideoWriter)
	{
		if (m_pVideoWriter->isOpened())
		{
			*m_pVideoWriter << resizeImage;
		}
	}

	if (3 == resizeImage.channels())
	{
		cvtColor(resizeImage, rgbImage, CV_BGR2RGB);
		showImage = QImage((const uchar*)(rgbImage.data),
			rgbImage.cols, rgbImage.rows,
			rgbImage.step,
			QImage::Format_RGB888);
	}
	else
	{
		showImage = QImage(resizeImage.data,
			resizeImage.cols, resizeImage.rows,
			resizeImage.step,
			QImage::Format_Indexed8);
	}

	ui->label_video->setPixmap(QPixmap::fromImage(showImage));
}


/*
* ������
* ���أ�
* ���ܣ�����ϵͳ�����б�
*/
void Widget::InitSerialPort()
{
	m_pCom = new SerialPort;
	//m_pRxThread = new QThread;

	//m_pCom->moveToThread(m_pRxThread);
	//m_pRxThread->start();

	QStringList comList = m_pCom->GetComList();
	
	ui->comboBox_selCom->addItems(comList);
}


/*
* ������
* ���أ�
* ���ܣ���ʼ��ѹ������
*/
void Widget::InitCurve()
{
	if (NULL == m_pCurve)
	{
		m_pCurve = new QwtPlotCurve("Pressure");
	}

	if (NULL == m_pGrid)
	{
		m_pGrid = new QwtPlotGrid();
	}
	m_pGrid->attach(ui->qwtPlot);
	m_pGrid->enableX(true);
	m_pGrid->enableY(true);
	m_pGrid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
	//m_pGrid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
	m_pGrid->setMinorPen(QPen(Qt::gray, 0, Qt::DotLine));
	

	ui->qwtPlot->setTitle(QStringLiteral("ѹ������"));
	ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, QStringLiteral("ʱ�䣨S��"));
	ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, QStringLiteral("ѹ����Pa��"));
	ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 60.0, 5.0);
	ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 100000.0);

	m_maxY = 0;
	m_avgY = 0;
	m_vectorX.clear();
	m_vectorY.clear();

	m_pCurve->setStyle(QwtPlotCurve::Lines);
	m_pCurve->setCurveAttribute(QwtPlotCurve::Fitted, true);
	m_pCurve->setPen(QPen(Qt::red));

	m_pCurve->attach(ui->qwtPlot);
}


/*
* ������
* ���أ�
* ���ܣ�����ѹ������
*/
void Widget::ResetCurve()
{
	m_vectorX.clear();
	m_vectorY.clear();

	m_pCurve->setSamples(m_vectorX.data(), m_vectorY.data(), m_vectorX.size());
	ui->qwtPlot->replot();
}


/*
* ������
* ���أ�
* ���ܣ�����ѹ������
*/
void Widget::DrawCurve()
{
	double x = m_time.second() + m_time.minute() * 60 + m_time.hour() * 3600;
	double y = ui->lcdNumber_pressure->intValue();
	size_t ySize = m_vectorY.size();

	if (ySize < 10)
	{
		m_avgY = m_avgY * ySize + y;
		m_avgY /= ySize + 1;
	}
	else
	{
		m_avgY = m_avgY * 9 + y;
		m_avgY /= 10;
	}

	m_vectorX.append(x);
	m_vectorY.append(m_avgY);

	if (m_avgY > m_maxY)
	{
		m_maxY = m_avgY;
	}
	
	int count = m_vectorX.size();

	if (count < 60)
	{
		ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 60.0, 5.0);
	}
	else
	{
		ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, count + 15, ((count + 15) / 25 / 5 + 1) * 5);
	}
	
	ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, m_maxY * 1.1);

	m_pCurve->setSamples(m_vectorX.data(), m_vectorY.data(), count);
	ui->qwtPlot->replot();
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
* ���ܣ�ʶ���㷨��ʼ��
*/
void Widget::InitImageProc()
{
	m_pImgProc = new ImageProc;
	m_pImgProcThread = new QThread;

	m_pImgProc->SetCamera(m_pCamera);
	m_pImgProc->moveToThread(m_pImgProcThread);
	m_pImgProcThread->start();
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
void Widget::UpdateReportQueryView(const QList<STRUCT_Report> &reportList)
{
	CreateReportViewTable();

	size_t size = m_reportList.size();

	for (size_t i = 0; i < size; i++)
	{
		QString strMode = "";
		switch (reportList[i].endMode)
		{
		case EndAuto:
			strMode = QStringLiteral("�Զ�����");
			break;
		case EndBySoftware:
			strMode = QStringLiteral("����ֶ�����");
			break;
		case EndByDevice:
			strMode = QStringLiteral("�豸�ֶ�����");
			break;
		case EndTimeOut:
			strMode = QStringLiteral("��ʱʱ�䵽");
			break;
		case EndPressureOverRange:
			strMode = QStringLiteral("ѹ��������");
			break;
		case EndBurst:
			strMode = QStringLiteral("��ƥ����");
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
	

	QString unit;

	switch (method.unit)
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

	ui->textEdit_methodInfo->append(QStringLiteral("������") + method.description);
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
* ���ܣ�����ʶ�𵽵���������������Ӧ��Ϣ
*/
void Widget::GetDropNum()
{
	int num = m_pImgProc->GetDropNum();

	//if ((num > 0) && (m_dropNum == 0))
	//{
	//	ui->textEdit_info->clear();
	//}

	int n = num - m_dropNum;

	for (int i = 0; i < n; i++)
	{
		QString strTime = m_time.toString("hh:mm:ss");
		QString strPressure = QString::number(m_pCom->GetCurrentPressure());
		m_listAppearTime.push_back(strTime);
		m_listApperPressure.push_back(strPressure);
		ui->textEdit_info->append(QStringLiteral("��%1��ˮ�����ʱ�䣺").arg(m_dropNum + i + 1) + strTime);
		ui->textEdit_info->append(QStringLiteral("��ǰѹ��ֵΪ��") + strPressure + QString("\n"));
	}

	m_dropNum = num;

	if (m_dropNum >= 3)
	{
		// ���Ͳ���ֹͣ����
		//m_pCom->TxCmd(0x01, 0x0, 0x0);
		m_txData = TxStopTest;
		m_testState = EndAuto;

		// ���ɲ��Ա���
		//GenTestReport();
	}
}

/*
* ������
* ���أ�
* ���ܣ��������һ֡ͼ��
*/
void Widget::SaveLastImage()
{
	QPixmap pic(*ui->label_video->pixmap());
	pic.save("./report/last.bmp");
}


/*
* ������
* ���أ�
* ���ܣ�����ѹ������
*/
void Widget::SavePressureCurve()
{
	m_curveRenderer.renderDocument(ui->qwtPlot, "./report/curve.bmp", QSizeF(150, 75));
}

/*
* ������
* ���أ�
* ���ܣ����ɲ��Ա���
*/
void Widget::GenTestReport(void)
{
	STRUCT_Report report;

	SaveLastImage();
	SavePressureCurve();
	ui->textEdit_report->clear();

	QString htmlStr = QStringLiteral("<p align = \"center\"><font size = \"6\">��ˮѹ�������ϵͳ</font></p>");
	htmlStr += QStringLiteral("<p align = \"center\"><font size = \"6\">���Ա���</font></p> <p> </p> <p> </p>");

	report.methodName = m_methodParam.name;
	report.methodPlan = m_methodParam.plan;
	report.endMode = m_testState;
	report.userName = m_account.userName;
	report.fileName = "";
	report.rate = m_methodParam.rate;
	report.timing = m_methodParam.timing;
	report.pressure = m_methodParam.pressure;
	report.cycle = m_methodParam.cycle;
	report.holdingTime = m_methodParam.holdingTime;
	report.unit = m_methodParam.unit;
	report.description = m_methodParam.description;
	report.standard = m_methodParam.standard;

	htmlStr += QStringLiteral("<p><font size = \"4\"><b>���Է��������:</b></font></p>");
	htmlStr += QStringLiteral("<p>�������ƣ�%1</p>").arg(m_methodParam.name);
	htmlStr += QStringLiteral("<p>Ӧ�ñ�׼��%1</p>").arg(m_methodParam.standard);

	QString unit;
	switch (m_methodParam.unit)
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

	switch (m_methodParam.plan)
	{
	case 0:
		htmlStr += QStringLiteral("<p>���Է�����������ѹ</p>");
		htmlStr += QStringLiteral("<p>������ѹ���ʣ�%1/����</p>").arg(QString::number(m_methodParam.rate)  + unit);
		break;
	case 1:
		htmlStr += QStringLiteral("<p>���Է�������ʱ��ѹ</p>");
		htmlStr += QStringLiteral("<p>��ʱ��ѹ���ʣ�%1/����</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>��ʱ��ѹ��ʱʱ�䣺%1����</p>").arg(QString::number(m_methodParam.timing));
		break;
	case 2:
		htmlStr += QStringLiteral("<p>���Է�������ʱ��ѹ</p>");
		htmlStr += QStringLiteral("<p>��ʱ��ѹ���ʣ�%1/����</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>��ʱ��ѹĿ��ѹǿ��%1</p>").arg(QString::number(m_methodParam.pressure) + unit);
		htmlStr += QStringLiteral("<p>��ʱ��ѹ��ʱʱ�䣺%1����</p>").arg(QString::number(m_methodParam.timing));
		break;
	case 3:
		htmlStr += QStringLiteral("<p>���Է����������ɳ�</p>");
		htmlStr += QStringLiteral("<p>�����ɳ�ˮ���ʣ�%1/����</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>�����ɳ�Ŀ��ѹǿ��%1</p>").arg(QString::number(m_methodParam.pressure) + unit);
		htmlStr += QStringLiteral("<p>�����ɳ��������ڣ�%1��</p>").arg(QString::number(m_methodParam.cycle));
		htmlStr += QStringLiteral("<p>�����ɳ�ˮѹ����ʱ�䣺%1����</p>").arg(QString::number(m_methodParam.holdingTime));
		break;
	case 4:
		htmlStr += QStringLiteral("<p>���Է�������ˮ©ˮ</p>");
		htmlStr += QStringLiteral("<p>��ˮ©ˮ���ʣ�%1/����</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>��ˮ©ˮĿ��ѹǿ��%1</p>").arg(QString::number(m_methodParam.pressure) + unit);
		htmlStr += QStringLiteral("<p>��ˮ©ˮ��ʱʱ�䣺%1����</p>").arg(QString::number(m_methodParam.timing));
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p>������%1</p>").arg(m_methodParam.description);
	report.testDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	htmlStr += QStringLiteral("<p>   </p><p>    </p><p><b>��������ʱ�䣺</b>%1</p>").arg(report.testDate);

	
	QString strEndType;

	switch (m_testState)
	{
	case EndAuto:
		strEndType = QStringLiteral("��⵽ˮ���Զ�����");
		break;
	case EndByDevice:
		strEndType = QStringLiteral("���豸���ֶ�����");
		break;
	case EndBySoftware:
		strEndType = QStringLiteral("��������ֶ�����");
		break;
	case EndBurst:
		strEndType = QStringLiteral("��ƥ���ƽ���");
		break;
	case EndTimeOut:
		strEndType = QStringLiteral("��ʱʱ�䵽����");
		break;
	case EndPressureOverRange:
		strEndType = QStringLiteral("ѹ�������̽���");
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p><b>���Խ�����ʽ��</b>%1</p>").arg(strEndType);

	int n = m_listAppearTime.size();
	htmlStr += QStringLiteral("<p><b>��ƥ�ϵ�ˮ���������</b></p>");

	if (n > 0)
	{
		for (int i = 0; i < n; i++)
		{
			htmlStr += QStringLiteral("<p>��⵽��%1��ˮ���ʱ��Ϊ��%2</p>").arg(i + 1).arg(m_listAppearTime[i]);
			htmlStr += QStringLiteral("<p>��⵽��%1��ˮ���ѹ��ֵΪ��%2</p>").arg(i + 1).arg(m_listApperPressure[i]);
		}
	}
	else
	{
		htmlStr += QStringLiteral("<p>δ��⵽�����ϳ���ˮ�飡</p>");
	}

	// ��ȡ�����ʵ����ͼƬ
	QString imgPath;
	if (EndAuto == m_testState)
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/report.bmp"));
	}
	else
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/last.bmp"));
	}

	htmlStr += QStringLiteral("<p> </p> <p align = \"center\"><b>�������ͼ</b></p>");
	htmlStr += imgPath;
	htmlStr += QStringLiteral("<p> </p> <p align = \"center\"><b>ѹ������</b></p>");
	htmlStr += QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/curve.bmp"));

	ui->textEdit_report->insertHtml(htmlStr);
	if (!m_pTestResult->AddReport(report))
	{
		ui->label_testInterfaceMessage->setText(QStringLiteral("�����ݿ�д�����鱨�����"));
	}
}

/*
* ������
* ���أ�
* ���ܣ���ӡ���Ա���
*/
void Widget::PrintReport()
{
	QPrinter printer;

	QPrintDialog printDialog(&printer, this);

	if (printDialog.exec())
	{
		QTextDocument textDoc;
		textDoc.setHtml(ui->textEdit_report->toHtml());
		textDoc.print(&printer);

	}
}

/*
* ������
* ���أ�
* ���ܣ����Կ�ʼ
*/
void Widget::StartTest()
{
	m_testState = Start;
	m_listAppearTime.clear();
	m_listApperPressure.clear();
	m_time = QTime(0, 0, 0);
	ui->lcdNumber_time->display(m_time.toString("hh:mm:ss"));
	ui->lcdNumber_pressure->display(QString::number(0));
	ResetCurve();
	m_pCom->ResetHandshakeState();
	m_pImgProc->InitCalc();
	m_pImgProc->StartCalc();
	ui->textEdit_info->append(QStringLiteral("���鿪ʼ������"));
	ui->pushButton_startStop->setText(QStringLiteral("����"));
	ui->label_testState->setText(QStringLiteral("��������"));
	SetDeviceOprateEnabled(StartStop | PauseConti);
	ui->comboBox_selMethod->setEnabled(false);
	ui->comboBox_pressureRange->setEnabled(false);
	ui->textEdit_report->clear();

	// ������Ƶ
	QFile file("./report/record.avi");
	if (file.exists())
	{
		file.remove();
	}

	m_pVideoWriter = new VideoWriter("./report/record.avi", CV_FOURCC('M', 'J', 'P', 'G'), 5.0, Size(ui->label_video->width(), ui->label_video->height()), false);
	//m_pVideoWriter = new VideoWriter("./report/record.avi", -1, 5.0, Size(ui->label_video->width(), ui->label_video->height()), false);
}


/*
* ������
* ���أ�
* ���ܣ����Խ���
*/
void Widget::StopTest()
{
	//switch (m_testState)
	//{
	//case EndAuto:	// �����⵽ˮ���Զ�����
	//	break;
	//case EndBySoftware:	// ͨ����λ����ť��������
	//	break;
	//case EndByDevice:	// ͨ���豸��ť��������
	//	break;
	//case EndBurst:	// ����
	//	break;
	//case EndPressureOverRange:	// ѹ��������
	//	break;
	//case EndTimeOut:	// ��ʱʱ�䵽
	//	break;
	//default:
	//	break;
	//}

	//m_testState = End;
	m_dropNum = 0;
	m_txData = TxNoData;
	m_pImgProc->StopCalc();
	ui->textEdit_info->append(QStringLiteral("���������"));
	ui->pushButton_startStop->setText(QStringLiteral("��ʼ"));
	ui->pushButton_pauseConti->setText(QStringLiteral("��ͣ"));
	ui->label_testState->setText(QStringLiteral("�������"));
	SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
	ui->comboBox_selMethod->setEnabled(true);
	ui->comboBox_pressureRange->setEnabled(true);

	if (m_pVideoWriter)
	{
		m_pVideoWriter->release();
		delete m_pVideoWriter;
		m_pVideoWriter = NULL;
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
* ���ܣ����ö��豸�Ĳ���״̬
*/
void Widget::SetDeviceOprateEnabled(quint8 state)
{
	if (ConnectDevice & state)
	{
		ui->pushButton_connectCom->setEnabled(true);
	}
	else
	{
		ui->pushButton_connectCom->setEnabled(false);
	}

	if (WaterIn & state)
	{
		ui->pushButton_waterIn->setEnabled(true);
	}
	else
	{
		ui->pushButton_waterIn->setEnabled(false);
	}

	if (WaterOff & state)
	{
		ui->pushButton_waterOff->setEnabled(true);
	}
	else
	{
		ui->pushButton_waterOff->setEnabled(false);
	}

	if (StartStop & state)
	{
		ui->pushButton_startStop->setEnabled(true);
	}
	else
	{
		ui->pushButton_startStop->setEnabled(false);
	}

	if (PauseConti & state)
	{
		ui->pushButton_pauseConti->setEnabled(true);
	}
	else
	{
		ui->pushButton_pauseConti->setEnabled(false);
	}

	if (Camera & state)
	{
		ui->pushButton_openCloseCamera->setEnabled(true);
	}
	else
	{
		ui->pushButton_openCloseCamera->setEnabled(false);
	}
}


/*
* ������flag:�߼����ÿɱ༭״̬
* ���أ�
* ���ܣ����ø߼����õĲ���״̬
*/
void Widget::SetAdvanceSettingEnabled(bool flag)
{
	if (flag)
	{
		ui->lineEdit_dropRec->setReadOnly(false);
		ui->lineEdit_flowInTime->setReadOnly(false);
		ui->lineEdit_dropArea->setReadOnly(false);
		ui->lineEdit_sensitivity->setReadOnly(false);
		ui->lineEdit_ignoreWidth->setReadOnly(false);
		ui->comboBox_pressureRange->setEnabled(true);
		ui->lineEdit_centerX->setReadOnly(false);
		ui->lineEdit_centerY->setReadOnly(false);
		ui->lineEdit_r->setReadOnly(false);
	}
	else
	{
		ui->lineEdit_dropRec->setReadOnly(true);
		ui->lineEdit_flowInTime->setReadOnly(true);
		ui->lineEdit_dropArea->setReadOnly(true);
		ui->lineEdit_sensitivity->setReadOnly(true);
		ui->lineEdit_ignoreWidth->setReadOnly(true);
		ui->comboBox_pressureRange->setEnabled(false);
		ui->lineEdit_centerX->setReadOnly(true);
		ui->lineEdit_centerY->setReadOnly(true);
		ui->lineEdit_r->setReadOnly(true);
	}
}


/*
* ������params���߼����ò���
* ���أ�
* ���ܣ����¸߼����ò���
*/
void Widget::UpdateAdvanceParams(const STRUCT_AdvanceParams &params)
{
	ui->lineEdit_dropRec->setText(QString::number(params.density));
	ui->lineEdit_flowInTime->setText(QString::number(params.waterInTime));
	ui->lineEdit_dropArea->setText(QString::number(params.dropletArea));
	ui->lineEdit_sensitivity->setText(QString::number(params.sensitivity));
	ui->lineEdit_ignoreWidth->setText(QString::number(params.ignorePixes));
	ui->lineEdit_centerX->setText(QString::number(params.x));
	ui->lineEdit_centerY->setText(QString::number(params.y));
	ui->lineEdit_r->setText(QString::number(params.r));

	ui->comboBox_pressureRange->setCurrentIndex(params.pressureRange);
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
	switch (m_testState)
	{
	case WaterInStart:
		ui->textEdit_info->append(QStringLiteral("��ʾ������ֹͣ��ˮ���ٹر������"));
		break;
	case WaterOffStart:
		ui->textEdit_info->append(QStringLiteral("��ʾ������ֹͣ��ˮ���ٹر������"));
		break;
	case Start:
	case Pause:
		ui->textEdit_info->append(QStringLiteral("��ʾ������ֹͣ���飬�ٹر������"));
		break;
	default:
		this->close();
		break;
	}
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

	state = m_pMethodParam->GetMethodInfo(index - 1,  m_methodParam);

	if (!state)
	{
		ui->label_testInterfaceMessage->setText(m_pMethodParam->GetMessageList()[0]);
		ui->comboBox_selMethod->setCurrentIndex(0);
	}
	else
	{
		ShowMethodParam(m_methodParam);
		if (m_bIsComOpened)
		{
			m_testState = SetParams;
			m_txData = TxSetParams;
			ui->comboBox_selMethod->setEnabled(false);
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("��ʾ���豸δ���ӣ��޷����ò��Բ�����"));
		}
	}
}


/*
* ������
* ���أ�
* ���ܣ���Ӧʵ�鱨�棬��ť�ۺ���
*/
void Widget::OnBtnPrintReportClicked()
{
	PrintReport();
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

	if (Tester == m_account.userType)
	{
		SetAdvanceSettingEnabled(false);
	}
	else
	{
		SetAdvanceSettingEnabled(true);
	}

	m_pMethodParam->GetAdvanceParams(1, m_advanceParams);
	UpdateAdvanceParams(m_advanceParams);
}


/*
* ������
* ���أ�
* ���ܣ��򿪻�ر�����ۺ���
*/
void Widget::OnBtnOpenCameraClicked()
{
	BOOL state = FALSE;

	if ((m_pCamera != NULL) && (FALSE == m_pCamera->IsCameraOpened()))
	{
		// �����
		state = m_pCamera->OpenCamera();

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("�����ʧ�ܣ�"));
			return;
		}

		// ���òɼ�
		state = m_pCamera->StartSnap(MerCamera::ContinuousMode);

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("��ʼ������ɼ�ʧ�ܣ�"));
			m_pCamera->CloseCamera();
			return;
		}

		// �����Զ��ع�
		state = m_pCamera->SetAutoExposure(GX_EXPOSURE_AUTO_CONTINUOUS);

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("��������Զ��ع�ʧ�ܣ�"));
			m_pCamera->CloseCamera();
			return;
		}

		// �趨���պ������Ҷ�
		state = m_pCamera->SetExpectedGray(64);

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("������������Ҷ�ʧ�ܣ�"));
			m_pCamera->CloseCamera();
			return;
		}

		ui->textEdit_info->append(QStringLiteral("����Ѵ򿪣�"));
		ui->pushButton_openCloseCamera->setText(QStringLiteral("�ر����"));
	}
	else if ((m_pCamera != NULL) && (TRUE == m_pCamera->IsCameraOpened()))
	{
		state = m_pCamera->CloseCamera();

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("�ر����ʧ�ܣ��븴λ�����ԣ�"));
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("����ѹرգ�"));
		}

		ui->pushButton_openCloseCamera->setText(QStringLiteral("�����"));
	}
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
		//m_pRxThread->quit();
		//m_pRxThread->wait();

		m_pCom->Close();
		m_bIsComOpened = false;
		if (Connected == m_testState)
		{
			ui->textEdit_info->append(QStringLiteral("���豸�Ͽ����ӣ�"));
			ui->pushButton_connectCom->setText(QStringLiteral("����"));
		}	
		ui->label_testState->setText(QStringLiteral("�豸δ����"));
		m_testState = Init;
		SetDeviceOprateEnabled(ConnectDevice | Camera);
		ui->comboBox_selCom->setEnabled(true);
	}
	else
	{
		ui->comboBox_selCom->setEnabled(false);
		m_bIsComOpened = m_pCom->Open(ui->comboBox_selCom->currentText());

		if (m_bIsComOpened) 
		{
			//ui->textEdit_info->append(QStringLiteral("�򿪴��ڳɹ���"));
			//m_pCom->TxReadState();
			m_txData = TxConnect;
			//SetDeviceOprateEnabled(false);

			//m_pCom->moveToThread(m_pRxThread);
			//m_pRxThread->start();
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("���󣺴򿪴���ʧ�ܣ����鴮���豸��"));
		}
	}

	//UpdateComUI();
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
		//m_pCom->TxCmd(0x07, 0x0, 0x0);
		m_txData = TxWaterInStop;
		m_btnState = WaterInStop;
		//m_bIsWaterIn = false;
		//ui->pushButton_waterIn->setText(QStringLiteral("��ˮ"));
	}
	else
	{
		//m_pCom->TxCmd(0x07, 0x01, 0x0);
		m_txData = TxWaterInStart;
		m_btnState = WaterInStart;
		//m_bIsWaterIn = true;
		//ui->pushButton_waterIn->setText(QStringLiteral("ֹͣ��ˮ"));
	}

	SetDeviceOprateEnabled(0);
}


/*
* ������
* ���أ�
* ���ܣ���ˮ��ֹͣ��ˮ����ۺ���
*/
void Widget::OnBtnWaterOffClicked()
{
	// ��ˮ����:0x08�� ��ʼ��ˮ��1��ֹͣ��ˮ��0
	if (m_bIsWaterOff)
	{
		//m_pCom->TxCmd(0x08, 0x0, 0x0);
		m_txData = TxWaterOffStop;
		m_btnState = WaterOffStop;
		//m_bIsWaterOff = false;
		//ui->pushButton_waterOff->setText(QStringLiteral("��ˮ"));
	}
	else
	{
		//m_pCom->TxCmd(0x08, 0x01, 0x0);
		m_txData = TxWaterOffStart;
		m_btnState = WaterOffStart;
		//m_bIsWaterOff = true;
		//ui->pushButton_waterOff->setText(QStringLiteral("ֹͣ��ˮ"));
	}

	SetDeviceOprateEnabled(0);
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
		// ���״̬
		if (!m_pCamera->IsCameraOpened())
		{
			ui->textEdit_info->append(QStringLiteral("��ʾ�����ȴ������"));
			return;
		}

		// ��������״̬����Խ���״̬����ʼ�µĲ��ԣ������ò���
		if (!m_bIsParamsSet)
		{
			ui->textEdit_info->append(QStringLiteral("��ʾ������ѡ����ȷ�Ĳ��Է�����"));
			return;
		}

		//m_testState = StartBySoftware;
		m_btnState = StartBySoftware;
		m_txData = TxStartTest;
	}
	else if ((Start == m_testState) || (Pause == m_testState))
	{
		// ����Ϊ��ʼ����ͣ״̬��ֹͣ����
		//m_pCom->TxCmd(0x01, 0x0, 0x0);
		m_btnState = EndBySoftware;
		m_txData = TxStopTest;
		//m_testState = EndBySoftware;
	}

	// ��ֹ��������
	SetDeviceOprateEnabled(0);
	ui->comboBox_selMethod->setEnabled(false);
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
		// ���Կ�ʼ״̬����ͣ����
		//m_txData = TxPauseTest;
		//m_testState = PauseBySoftware;
		m_btnState = PauseBySoftware;
	}
	else if (Pause == m_testState)
	{
		// ������ͣ״̬�����Լ�������
		//m_txData = TxStartTest;
		//m_testState = ContinueBySoftware;
		m_btnState = ContinueBySoftware;
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
void Widget::OnHandShakeStateReceived(STRUCT_HandShake *handshake)
{
	switch (handshake->state)
	{
	case Busy:
		ui->textEdit_info->append(QStringLiteral("ͨ����æ�����Ժ��ٳ��ԣ�"));
		break;
	case SetParamOk:
		if (5 == handshake->data)
		{
			ui->textEdit_info->append(QStringLiteral("ѹ���������óɹ���"));
			ui->label_advanceMessage->setText(QStringLiteral("ѹ���������óɹ���"));
			ui->comboBox_pressureRange->setEnabled(true);
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("���Բ������óɹ���"));
			m_testState = Connected;
			m_bIsParamsSet = true;
			ui->comboBox_selMethod->setEnabled(true);
		}
		break;
	case SetParamError:
		if (5 == handshake->data)
		{
			ui->textEdit_info->append(QStringLiteral("ѹ����������ʧ�ܣ�"));
			ui->label_advanceMessage->setText(QStringLiteral("ѹ����������ʧ�ܣ�"));
			ui->comboBox_pressureRange->setEnabled(true);
		}
		else
		{
		ui->textEdit_info->append(QStringLiteral("���󣺲��Բ���������ʧ�ܣ�"));
		m_testState = Connected;
		m_bIsParamsSet = false;
		ui->comboBox_selMethod->setEnabled(true);
		}	
		//SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);	
		break;
	case SetParamAckTimeOut:
		if (5 == handshake->data)
		{
			ui->textEdit_info->append(QStringLiteral("ѹ���������ã�Ӧ��ʱ��"));
			ui->label_advanceMessage->setText(QStringLiteral("ѹ���������ã�Ӧ��ʱ��"));
			ui->comboBox_pressureRange->setEnabled(true);
		}
		else
		{
		ui->textEdit_info->append(QStringLiteral("�������ò��Բ������ȴ�Ӧ��ʱ��"));
		m_testState = Connected;
		m_bIsParamsSet = false;
		//SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
		ui->comboBox_selMethod->setEnabled(true);
		}
		break;
	case ReadParamOk:
		ui->textEdit_info->append(QStringLiteral("��ȡ�����ɹ���"));
		break;
	case ReadParamError:
		ui->textEdit_info->append(QStringLiteral("���󣺶�ȡ����ʧ�ܣ�"));
		break;
	case ReadParamAckTimeOut:
		ui->textEdit_info->append(QStringLiteral("���󣺶�ȡ�������ȴ�Ӧ��ʱ���������豸�����������"));
		break;
	case ReadStateOk:
		if (Init == m_testState)
		{
			ui->textEdit_info->append(QStringLiteral("���豸�����ɹ���"));
			ui->label_testState->setText(QStringLiteral("�豸������"));
			ui->pushButton_connectCom->setText(QStringLiteral("�Ͽ�"));
			m_testState = Connected;
			SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("��ȡ�豸ʵʱ������״̬�ɹ���"));
		//}	
		break;
	case ReadStateError:
		if (Init == m_testState)
		{
			m_pCom->Close();
			m_bIsComOpened = false;
			ui->textEdit_info->append(QStringLiteral("�������豸����ʧ�ܣ��������豸�����������"));
			ui->comboBox_selCom->setEnabled(true);
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("��ȡ�豸ʵʱ������״̬ʧ�ܣ�"));
		//}
		break;
	case ReadStateAckTimeOut:
		if (Init == m_testState)
		{
			m_pCom->Close();
			m_bIsComOpened = false;
			ui->textEdit_info->append(QStringLiteral("�������豸����ʧ�ܣ��������豸�����������"));
			ui->comboBox_selCom->setEnabled(true);
			//SetDeviceOprateEnabled(true);
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("��ȡ�豸ʵʱ������״̬���ȴ�Ӧ��ʱ���������豸�����������"));
		//}	
		break;
	case CmdOk:
		switch (handshake->cmd)
		{
		case 0x01:	// ���Կ�ʼ����ͣ���������Ӧ��ɹ�
			if (0 == handshake->data)
			{
				// ��λ������ֹͣ��������ɹ����µĲ��Կ�ʼ
				StopTest();

				if (EndBySoftware == m_btnState)
				{
					m_testState = EndBySoftware;
					m_btnState = Init;
				}
				else
				{
					m_testState = EndAuto;
				}
			}
			else if (0xff == handshake->data)
			{
				// ��λ��������ͣ��������ɹ���������ͣ
				m_testState = Pause;
				m_btnState = Init;
				m_pImgProc->StopCalc();
				ui->textEdit_info->append(QStringLiteral("������ͣ������"));
				ui->pushButton_pauseConti->setText(QStringLiteral("����"));
				ui->label_testState->setText(QStringLiteral("������ͣ"));
			}
			else
			{
				// ��λ��������ʼ��������ɹ����µĲ��Կ�ʼ
				if (StartBySoftware == m_btnState)
				{
					StartTest();
				}
				else if (ContinueBySoftware == m_btnState)
				{
					// ������ͣ����λ��������ʼ����ɹ�����������
					m_testState = Start;
					ui->textEdit_info->append(QStringLiteral("�������������"));
					ui->pushButton_pauseConti->setText(QStringLiteral("��ͣ"));
					ui->label_testState->setText(QStringLiteral("��������"));
				}
				m_btnState = Init;				
			}
			break;
		case 0x02:	// ѹͷ���ƣ�Ӧ��ɹ�
			break;
		case 0x03:	// �ƹ���ƣ�Ӧ��ɹ�
			break;
		case 0x04:	// ������Ӧ��ɹ�
			break;
		case 0x05:	// ȥƤ��Ӧ��ɹ�
			break;
		case 0x06:	// �ָ��������ƣ�Ӧ��ɹ�
			break;
		case 0x07:	// ��ˮ���ƣ�Ӧ��ɹ�
			if (1 == handshake->data)
			{
				m_bIsWaterIn = true;
				ui->pushButton_waterIn->setText(QStringLiteral("ֹͣ��ˮ"));
				ui->textEdit_info->append(QStringLiteral("��ʼ��ˮ��"));
				SetDeviceOprateEnabled(WaterIn);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterIn = false;
				m_testState = Connected;
				ui->pushButton_waterIn->setText(QStringLiteral("��ˮ"));
				ui->textEdit_info->append(QStringLiteral("ֹͣ��ˮ��"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x08:	// ��ˮ���ƣ�Ӧ��ɹ�
			if (1 == handshake->data)
			{
				m_bIsWaterOff = true;
				ui->textEdit_info->append(QStringLiteral("��ʼ��ˮ��"));
				ui->pushButton_waterOff->setText(QStringLiteral("ֹͣ��ˮ"));
				SetDeviceOprateEnabled(WaterOff);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterOff = false;
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("ֹͣ��ˮ��"));
				ui->pushButton_waterOff->setText(QStringLiteral("��ˮ"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x09:	// ѹ���궨��Ӧ��ɹ�
			break;
		case 0x0b:	// ֪ͨ�豸��ǰѡ������Ӧ��ɹ�
			break;
		default:
			break;
		}
		break;
	case CmdError:
		switch (handshake->cmd)
		{
		case 0x01:
			if (0 == handshake->data)
			{
				if (EndBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("����ֹͣ�����������ʧ�ܣ�"));
					ui->textEdit_info->append(QStringLiteral("�����³��ԣ���ʹ���豸�ϵİ�ťֹͣ���ԣ�"));
					m_btnState = Init;
				}
				else
				{
					//m_testState = EndAuto;
					ui->textEdit_info->append(QStringLiteral("����ֹͣ�����Զ�����ʧ�ܣ�"));
					ui->textEdit_info->append(QStringLiteral("�Ѽ�⵽3�λ�����ˮ�飬���ֶ�ֹͣ���ԣ�"));
				}
				
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else if (0xff == handshake->data)
			{
				ui->textEdit_info->append(QStringLiteral("������ͣ�����������ʧ�ܣ�"));
				ui->textEdit_info->append(QStringLiteral("�����³��ԣ�"));
				m_btnState = Init;
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else
			{
				if (StartBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("���󣺿�ʼ�����������ʧ�ܣ�"));
					ui->textEdit_info->append(QStringLiteral("�����豸�����³��ԣ�"));
					m_btnState = Init;
					SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
				}
				else if (ContinueBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("���󣺼��������������ʧ�ܣ�"));
					ui->textEdit_info->append(QStringLiteral("�����³��ԣ�"));
					m_btnState = Init;
					SetDeviceOprateEnabled(StartStop | PauseConti);
				}
			}
			break;
		case 0x07:	// ��ˮ��Ӧ�����
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("���󣺽�ˮ�������ʧ�ܣ�"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterIn = false;
				m_testState = Connected;
				ui->pushButton_waterIn->setText(QStringLiteral("��ˮ"));
				ui->textEdit_info->append(QStringLiteral("����ֹͣ��ˮ�������ʧ�ܣ�"));
				ui->textEdit_info->append(QStringLiteral("----���������豸���ֶ�ֹͣ��ˮ��"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x08:	// ��ˮ��Ӧ�����
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("������ˮ�������ʧ�ܣ�"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterOff = false;
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("����ֹͣ��ˮ�������ʧ�ܣ�"));
				ui->textEdit_info->append(QStringLiteral("----�����豸���ֶ�ֹͣ��ˮ��"));
				ui->pushButton_waterOff->setText(QStringLiteral("��ˮ"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		default:
			break;
		}
		break;
	case CmdAckTimeOut:
		switch (handshake->cmd)
		{
		case 0x01:	// ���Կ�ʼ����ͣ���������Ӧ��ʱ
			if (0 == handshake->data)
			{
				if (EndBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("����ֹͣ�������Ӧ��ʱ��"));
					ui->textEdit_info->append(QStringLiteral("�����³��ԣ���ʹ���豸�ϵİ�ťֹͣ���ԣ�"));
					m_btnState = Init;
				}
				else
				{
					ui->textEdit_info->append(QStringLiteral("�����Զ�����ֹͣ�������Ӧ��ʱ��"));
					ui->textEdit_info->append(QStringLiteral("�Ѽ�⵽3�λ�����ˮ�飬���ֶ�ֹͣ���飡"));
				}
				
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else if (0xff == handshake->data)
			{
				ui->textEdit_info->append(QStringLiteral("������ͣ�������Ӧ��ʱ��"));
				ui->textEdit_info->append(QStringLiteral("�����³��ԣ�"));
				m_btnState = Init;
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else
			{
				if (StartBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("���󣺿�ʼ�������Ӧ��ʱ��"));
					ui->textEdit_info->append(QStringLiteral("�����³��ԣ�"));
					m_btnState = Init;
					SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
				}
				else if (ContinueBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("���󣺿�ʼ�������Ӧ��ʱ��"));
					ui->textEdit_info->append(QStringLiteral("�����³��ԣ�"));
					m_btnState = Init;
					SetDeviceOprateEnabled(StartStop | PauseConti);
				}			
			}
			break;
		case 0x02:	// ѹͷ���ƣ�Ӧ��ʱ
			break;
		case 0x03:	// �ƹ���ƣ�Ӧ��ʱ
			break;
		case 0x04:	// ������Ӧ��ʱ
			break;
		case 0x05:	// ȥƤ��Ӧ��ʱ
			break;
		case 0x06:	// Ӧ��������ƣ�Ӧ��ʱ
			break;
		case 0x07:	// ��ˮ���ƣ�Ӧ��ʱ
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("���󣺽�ˮ���Ӧ��ʱ��"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterIn = false;
				m_testState = Connected;
				ui->pushButton_waterIn->setText(QStringLiteral("��ˮ"));
				ui->textEdit_info->append(QStringLiteral("����ֹͣ��ˮ���Ӧ��ʱ��"));
				ui->textEdit_info->append(QStringLiteral("----���������豸���ֶ�ֹͣ��ˮ��"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x08:	// ��ˮ���ƣ�Ӧ��ʱ
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("������ˮ���Ӧ��ʱ��"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterOff = false;
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("����ֹͣ��ˮ���Ӧ��ʱ��"));
				ui->textEdit_info->append(QStringLiteral("----�����豸���ֶ�ֹͣ��ˮ��"));
				ui->pushButton_waterOff->setText(QStringLiteral("��ˮ"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x09:	// ѹ���궨��Ӧ��ʱ
			break;
		case 0x0b:	// ֪ͨ�豸��ǰѡ������Ӧ��ʱ
			break;
		default:
			break;
		}
		//ui->textEdit_info->append(QStringLiteral("���豸�Ĳ������Ӧ��ʱ���������豸�����������"));
		//SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
		break;
	case DeviceReport:
		switch (handshake->cmd)
		{
		case 0x01:	// ѹ��������
			break;
		case 0x02:	// ����
			break;
		case 0x03:	// ��ʱʱ�䵽
			break;
		case 0x04:	// �豸����
			break;
		case 0x05:	// �豸�ϰ����԰�ť��֪ͨ��λ�����Կ�ʼ
			break;
		case 0x06:	// �豸�ϰ����԰�ť��֪ͨ��λ�����Խ���
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
* ������
* ���أ�
* ���ܣ���������ͼ
*/
void Widget::OnBtnSaveCurveClicked()
{
	m_curveRenderer.exportTo(ui->qwtPlot, "curve.pdf");
}


/*
* ������
* ���أ�
* ���ܣ��򿪱�����Ƶ�ļ���
*/
void Widget::OnBtnPlayBackClicked()
{
	QString path = QDir::currentPath();
	path += "/report";
	//QProcess::startDetached("explorer ./report");
	QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
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
* ������index��ѹ������ѡ��
* ���أ�
* ���ܣ�����ѹ������
*/
void Widget::OnCombPressureRangeChanged(int index)
{
	if (Init == m_testState)
	{
		return;
	}

	ui->comboBox_pressureRange->setEnabled(false);
	m_methodParam.plan = 5;
	m_methodParam.range = index;
	m_txData = TxSetParams;

	m_advanceParams.pressureRange = index;
	m_pMethodParam->UpdateAdvanceParams(2, m_advanceParams);
}


/*
* ������
* ���أ�
* ���ܣ�����߼����ò���
*/
void Widget::OnBtnSaveAdvanceParamsClicked()
{
	int val1;
	double val2;

	val1 = ui->lineEdit_dropRec->text().toInt();
	if ((val1 < 1) || (val1 > 10))
	{
		ui->label_advanceMessage->setText(QStringLiteral("��γ�ܶ�����ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.density = val1;
	}

	val1 = ui->lineEdit_flowInTime->text().toInt();
	if ((val1 < 0) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("��ˮʱ������ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.waterInTime = val1;
	}

	val2 = ui->lineEdit_dropArea->text().toDouble();
	if ((val2 < 0.01) || (val1 > 10000.0))
	{
		ui->label_advanceMessage->setText(QStringLiteral("ˮ���������ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.dropletArea = val2;
	}

	val1 = ui->lineEdit_sensitivity->text().toInt();
	if ((val1 < 1) || (val1 > 5))
	{
		ui->label_advanceMessage->setText(QStringLiteral("����������ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.sensitivity = val1;
	}

	val1 = ui->label_ignoreWidth->text().toInt();
	if ((val1 < 0) || (val1 > 500))
	{
		ui->label_advanceMessage->setText(QStringLiteral("���Ա�Ե�������ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.ignorePixes = val1;
	}

	val1 = ui->lineEdit_centerX->text().toInt();
	if ((val1 < 1) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("��Ұ����X����ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.x = val1;
	}

	val1 = ui->lineEdit_centerY->text().toInt();
	if ((val1 < 1) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("��Ұ����Y����ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.y = val1;
	}

	val1 = ui->lineEdit_r->text().toInt();
	if ((val1 < 1) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("��Ұֱ������ֵ������Ҫ�����޸ģ�"));
		return;
	}
	else
	{
		m_advanceParams.r = val1;
	}

	m_advanceParams.pressureRange = ui->comboBox_pressureRange->currentIndex();

	if (m_pMethodParam->UpdateAdvanceParams(2, m_advanceParams))
	{
		ui->label_advanceMessage->setText(QStringLiteral("�߼����ò����޸ĳɹ���"));
		m_pImgProc->UpdateParams(m_advanceParams);
	}
	else
	{
		ui->label_advanceMessage->setText(QStringLiteral("�߼����ò����޸Ĵ��������´�������ԣ�"));
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
	ui->textEdit_discription->setText(method.description);
	ui->comboBox_unit->setCurrentIndex(method.unit);
	m_currentUnitIndex = method.unit;

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
	method.description = ui->textEdit_discription->toPlainText();
	method.unit = ui->comboBox_unit->currentIndex();

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

	bool state = m_pTestResult->GetReportList(m_reportList, startDate, endDate, methodPlan, methodName, userName);
	

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
* ���ܣ���ʾͼ��
*/
void Widget::OnImagePrepared()
{
	ShowImage(m_pImgProc->GetImage());
	m_pCamera->ResetImgSnapedFlag();
}


/*
* ������
* ���أ�
* ���ܣ���ʱ���ۺ���
*/
void Widget::OnTimer()
{
	static int tick = 0;
	tick++;

	if ((m_testState >= EndAuto) && (m_testState <= EndBurst))
	{
		GenTestReport();
		m_testState = End;
	}

	if ((Start == m_testState) &&  (tick >= 5))
	{
		tick = 0;
		// ��ʾ����ʱ�����豸��ǰѹ��ֵ
		m_time = m_time.addSecs(1);
		ui->lcdNumber_time->display(m_time.toString("hh:mm:ss"));
		ui->lcdNumber_pressure->display(m_pCom->GetCurrentPressure());

		// ��ȡ��⵽��ˮ����
		GetDropNum();

		if (m_txData == TxNoData)
		{
			m_txData = TxReadDeviceState;
		}		

		DrawCurve();
	}

	if (m_txData != TxNoData)
	{
		if (m_pCom->IsIdle())
		{
			switch (m_txData)
			{
			case TxConnect:	
				m_pCom->TxReadState();
				break;
			case TxWaterInStop:
				m_pCom->TxCmd(0x07, 0x0, 0x0);
				break;
			case TxWaterInStart:
				m_pCom->TxCmd(0x07, 0x01, 0x0);
				break;
			case TxWaterOffStop:
				m_pCom->TxCmd(0x08, 0x0, 0x0);
				break;
			case TxWaterOffStart:
				m_pCom->TxCmd(0x08, 0x01, 0x0);
				break;
			case TxSetParams:
				m_pCom->TxSetParam(m_methodParam);
				break;
			case TxReadDeviceState:
				m_pCom->TxReadState();
				break;
			case TxStartTest:
				m_pCom->TxCmd(0x01, static_cast<quint8>(m_methodParam.plan + 1), 0x0);
				break;
			case TxStopTest:
				m_pCom->TxCmd(0x01, 0x0, 0x0);
				break;
			case TxPauseTest:
				m_pCom->TxCmd(0x01, 0xff, 0x0);
				break;
			case TxAck:
				m_pCom->TxAck(0x0, 0x03);
			default:
				break;
			}

			m_txData = TxNoData;
		}
	}
}


/*
* ������
* ���أ�
* ���ܣ�����״̬��Ϣ
*/
void Widget::OnUpdateDeviceState(STRUCT_DeviceState &deviceState)
{
	ui->lcdNumber_pressure->display(QString::number(deviceState.pressure));
}

/*
* ������
* ���أ�
* ���ܣ�����ѹ����Ϣ
*/
void Widget::OnCurrentPressureReceived(int pressure)
{
	ui->textEdit_info->append(QString::number(pressure));
}

/*
* ������
* ���أ�
* ���ܣ�
*/


/*
* ������
* ���أ�
* ���ܣ�
*/


/*
* ������
* ���أ�
* ���ܣ�
*/


/*
* ������
* ���أ�
* ���ܣ�
*/

/*
* ������
* ���أ�
* ���ܣ�
*/

/*
* ������
* ���أ�
* ���ܣ�
*/

/*
* ������
* ���أ�
* ���ܣ�
*/