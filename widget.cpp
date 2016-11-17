/*
* 创建日期：2016-09-02
* 最后修改：2016-11-17
* 作      者：syf
* 描      述：
*/
#include "widget.h"


/*
* 参数：
* 返回：
* 功能：构造函数
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
	, m_oldSize(0)
	, m_maxY(0)
	, m_avgY(0)
{
	CreateUi();
	// 初始化用户账号相关数据成员
	m_pAccountListModel = new QStringListModel(this);

	m_account.userType = 0;
	m_account.userName = "";
	m_account.passward = "";
	m_account.id = 0;

	m_pAccountDB = new UserAccount(this);

	// 初始化测试方法相关数据成员
	m_pMethodListModel = new QStringListModel(this);
	m_pMethodParam = new MethodParam(this);

	// 初始化测试结果查询相关成员
	m_pTestResult = new TestResult(this);

	// 初始化串口设备
	InitSerialPort();

	// 相机
	m_pCamera = new MerCamera(this);

	// 识别算法初始化
	InitImageProc();

	// 压力曲线
	InitCurve();

	// 初始化显示实验时间和设备当前压力值
	m_time = QTime(0, 0, 0);
	ui->lcdNumber_time->setDigitCount(8);
	ui->lcdNumber_pressure->setDigitCount(8);
	ui->lcdNumber_time->display(m_time.toString("hh:mm:ss"));
	ui->lcdNumber_pressure->display(QString::number(0));

	// 启动定时器
	m_pTimer = new QTimer(this);
	m_pTimer->start(200);

	// 可操作按钮
	SetDeviceOprateEnabled(ConnectDevice | Camera);

	// 设置双击不可编辑
	ui->listView_accountList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->listView_methodList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableView_reportQuery->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->textEdit_report->setReadOnly(true);
	ui->textEdit_info->setReadOnly(true);
	ui->textEdit_methodInfo->setReadOnly(true);

	/*
	* 链接信号与槽
	*/
	
	connect(ui->pushButton_min, &QPushButton::clicked, this, &Widget::OnBtnMinClicked);
	connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::OnBtnCloseClicked);

	// 设备操作
	connect(ui->pushButton_openCloseCamera, &QPushButton::clicked, this, &Widget::OnBtnOpenCameraClicked);
	connect(ui->pushButton_connectCom, &QPushButton::clicked, this, &Widget::OnBtnComOpClicked);
	connect(ui->pushButton_waterIn, &QPushButton::clicked, this, &Widget::OnBtnWaterInClicked);
	connect(ui->pushButton_waterOff, &QPushButton::clicked, this, &Widget::OnBtnWaterOffClicked);
	connect(ui->pushButton_startStop, &QPushButton::clicked, this, &Widget::OnBtnStartTestClicked);
	connect(ui->pushButton_pauseConti, &QPushButton::clicked, this, &Widget::OnBtnPauseTestClicked);
#ifdef DATA_DEBUG
	connect(m_pCom, &SerialPort::DataReceived, this, &Widget::OnRxDataReceived);
#endif
	//qRegisterMetaType<STRUCT_HandShake*>();
	connect(m_pCom, &SerialPort::HandshakeState, this, &Widget::OnHandShakeStateReceived);
	connect(ui->pushButton_print, &QPushButton::clicked, this, &Widget::OnBtnPrintReportClicked);

	// 界面切换
	connect(ui->pushButton_testInterface, &QPushButton::clicked, this, &Widget::OnBtnTestInterfaceClicked);
	connect(ui->pushButton_testMethod, &QPushButton::clicked, this, &Widget::OnBtnTestMethodClicked);
	connect(ui->pushButton_advance, &QPushButton::clicked, this, &Widget::OnBtnAdvanceClicked);
	connect(ui->pushButton_reportQuery, &QPushButton::clicked, this, &Widget::OnBtnReportQueryClicked);
	connect(ui->pushButton_help, &QPushButton::clicked, this, &Widget::OnBtnHelpClicked);
	connect(ui->pushButton_clearInfo, &QPushButton::clicked, ui->textEdit_info, &QTextEdit::clear);

	// 测试界面操作
	connect(ui->pushButton_video, &QPushButton::clicked, this, &Widget::OnBtnChartVideoClicked);
	connect(ui->pushButton_curve, &QPushButton::clicked, this, &Widget::OnBtnChartCurveClicked);
	connect(ui->pushButton_report, &QPushButton::clicked, this, &Widget::OnBtnChartReportClicked);
	connect(ui->comboBox_selMethod, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombSelMethodChanged);

	// 测试方法操作
	connect(ui->listView_methodList, &QListView::clicked, this, &Widget::OnMethodListItemClicked);
	connect(ui->pushButton_addMethod, &QPushButton::clicked, this, &Widget::OnBtnNewMethodClicked);
	connect(ui->comboBox_plan, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombMethodPlanChanged);
	connect(ui->comboBox_unit, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombPressureUnitChanged);
	connect(ui->pushButton_saveMethod, &QPushButton::clicked, this, &Widget::OnBtnSaveMethodClicked);
	connect(ui->pushButton_delMethod, &QPushButton::clicked, this, &Widget::OnBtnDeleteMethodClicked);
	connect(ui->pushButton_modifyMethod, &QPushButton::clicked, this, &Widget::OnBtnModifyMethodClicked);
	connect(ui->pushButton_saveCurve, &QPushButton::clicked, this, &Widget::OnBtnSaveCurveClicked);

	// 测试结果查询操作
	connect(ui->pushButton_query, &QPushButton::clicked, this, &Widget::OnBtnQueryClicked);
	connect(ui->pushButton_delReport, &QPushButton::clicked, this, &Widget::OnBtnDeleteReportListClicked);

	// 账户信息操作
	connect(ui->listView_accountList, &QListView::clicked, this, &Widget::OnAccountListItemClicked);
	connect(ui->pushButton_newAccount, &QPushButton::clicked, this, &Widget::OnBtnNewAccountClicked);
	connect(ui->pushButton_saveAccount, &QPushButton::clicked, this, &Widget::OnBtnSaveAccountClicked);
	connect(ui->pushButton_delAccount, &QPushButton::clicked, this, &Widget::OnBtnDeleteAccountClicked);
	connect(ui->pushButton_modifyAccount, &QPushButton::clicked, this, &Widget::OnBtnModifyAccountClicked);
	connect(ui->comboBox_pressureRange, static_cast<void (QComboBox::*)(int) >(&QComboBox::currentIndexChanged), this, &Widget::OnCombPressureRangeChanged);

	// 图像显示
	connect(m_pImgProc, &ImageProc::ImageProcessed, this, &Widget::OnImagePrepared);

	// 定时器
	connect(m_pTimer, &QTimer::timeout, this, &Widget::OnTimer);
	//connect(m_pCom, &SerialPort::DeviceStateReceived, this, &Widget::OnUpdateDeviceState);
#ifdef DATA_DEBUG
	connect(m_pCom, &SerialPort::CurrentPressure, this, &Widget::OnCurrentPressureReceived);
#endif
}


/*
* 参数：
* 返回：
* 功能：析构函数
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
* 参数：
* 返回：
* 功能：设计显示界面
*/
void Widget::CreateUi()
{
	ui->setupUi(this);

	// 固定登录窗口大小
	setFixedSize(1366, 768);

	// 无边框显示
	setWindowFlags(Qt::FramelessWindowHint);

	// 隐藏tabBar
	ui->tabWidget->tabBar()->hide();

	// 设置lcdnumber字体
	QPalette lcdtimepat = ui->lcdNumber_time->palette();
	lcdtimepat.setColor(QPalette::Normal, QPalette::WindowText, Qt::white);
	ui->lcdNumber_time->setPalette(lcdtimepat);

	QPalette lcdpressurepat = ui->lcdNumber_pressure->palette();
	lcdpressurepat.setColor(QPalette::Normal, QPalette::WindowText, Qt::white);
	ui->lcdNumber_pressure->setPalette(lcdpressurepat);

	// 设置通用样式
	this->setStyleSheet("QLabel{font-family:'Microsoft YaHei'; font-size:14px;color:#979797;}"
		"QPushButton{font-family:'Microsoft YaHei'; font-size:14px;}"
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; }"
		"QTextEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#000000;  background-color:#ffffff}"
		"QDateEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; background-color:#f7f7f7;}"
		"QComboBox{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; background-color:#f7f7f7}");

	// 设置状态栏Label样式
	ui->label_testState->setStyleSheet("QLabel{font-size:16px; color:#ffffff;}");
	ui->label_pressure->setStyleSheet("QLabel{font-size:16px; color:#ffffff;}");
	ui->label_holdingTime->setStyleSheet("QLabel{font-size:16px; color:#ffffff;}");

	// 设置底部版权标签样式
	ui->label_bottom->setStyleSheet("font-size:10px; color:#ffffff");

	// 用户操作界面
	SwitchInterface(m_interfaceIndex);

	/*
	* 设置检测界面样式
	*/
	ui->tabWidget_testInterface->tabBar()->hide();

	// 设置联机、进水、排水、开始、暂停、打开相机按钮样式
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

	// 设置视频回放、清除信息按钮样式
	ui->pushButton_playback->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:16px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn1.png);}");

	ui->pushButton_clearInfo->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:16px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn1.png);}");

	// 图表显示功能选择
	SwitchChart(m_chartIndex);

	// 设置提示信息样式
	ui->label_testInterfaceMessage->setStyleSheet("QLabel{color:#ff0000}");

	/*
	* 设置检测方法界面样式
	*/
	ui->label_methodList->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_methodParams->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_testMethodMessage->setStyleSheet("QLabel{color:#ff0000}");

	ui->listView_methodList->setStyleSheet("font-family:'Microsoft YaHei'; font-size:14px;");

	// 输入限制设定
	ui->lineEdit_pressureRate->setValidator(new QDoubleValidator(0, 1000000, 2, this));
	ui->lineEdit_setTime->setValidator(new QDoubleValidator(0, 1000000, 2, this));
	ui->lineEdit_targetPressure->setValidator(new QDoubleValidator(0, 1000000, 2, this));
	ui->lineEdit_pressureCycle->setValidator(new QIntValidator(0, 100, this));
	ui->lineEdit_pressureHolding->setValidator(new QDoubleValidator(0, 1000000, 2, this));

	ui->lineEdit_dropRec->setValidator(new QIntValidator(1, 10, this));
	ui->lineEdit_dropRec->setToolTip(QStringLiteral("建议设置范围值1~10"));
	ui->lineEdit_flowInTime->setValidator(new QIntValidator(1, 1000, this));
	ui->lineEdit_flowInTime->setToolTip(QStringLiteral("建议设置范围值1~1000"));
	ui->lineEdit_dropArea->setValidator(new QDoubleValidator(0.01, 10000.0, 2, this));
	ui->lineEdit_dropArea->setToolTip(QStringLiteral("建议设置范围值0.01~5.00"));
	ui->lineEdit_sensitivity->setValidator(new QIntValidator(1, 5, this));
	ui->lineEdit_sensitivity->setToolTip(QStringLiteral("建议设置范围值1~5"));
	ui->lineEdit_ignoreWidth->setValidator(new QIntValidator(0, 1024, this));
	ui->lineEdit_ignoreWidth->setToolTip(QStringLiteral("建议设置范围值0~100"));
	ui->lineEdit_centerX->setValidator(new QIntValidator(0, 1024, this));
	ui->lineEdit_centerX->setToolTip(QStringLiteral("建议设置范围值0~1024"));
	ui->lineEdit_centerY->setValidator(new QIntValidator(0, 1024, this));
	ui->lineEdit_centerY->setToolTip(QStringLiteral("建议设置范围值0~1024"));
	ui->lineEdit_r->setValidator(new QIntValidator(1, 1024, this));
	ui->lineEdit_r->setToolTip(QStringLiteral("建议设置范围值1~1024"));


	// 设置新增、删除、修改按钮样式
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
	* 设置结果查询界面样式
	*/
	m_pReportQueryModel = new QStandardItemModel(this);
	CreateReportViewTable();


	// 设置当前日期
	ui->dateEdit_start->setDate(QDate::currentDate().addDays(-1));
	ui->dateEdit_end->setDate(QDate::currentDate());

	// 设置调出日历窗口
	ui->dateEdit_start->setCalendarPopup(true);
	ui->dateEdit_end->setCalendarPopup(true);

	// 设置查询、删除、生成报告按钮样式
	ui->pushButton_query->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/main/resource/main/queryBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/main/resource/main/queryBtn1.png);}");
	ui->pushButton_delReport->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/main/resource/main/queryBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/main/resource/main/queryBtn1.png);}");
	ui->pushButton_genReport->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
		"QPushButton{border-image: url(:/main/resource/main/queryBtn0.png);}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/main/resource/main/queryBtn1.png);}");

	// 设置提示信息样式
	ui->label_queryMessage->setStyleSheet("QLabel{color:#ff0000}");

	/*
	* 设置高级设置界面样式
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

	// 用户名和密码只能有字母和数字构成
	ui->lineEdit_accountName->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));
	ui->lineEdit_accountPassword->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+$"), this));
}


/*
* 参数：
* 返回：
* 功能：生成报告查询表样式
*/
void Widget::CreateReportViewTable()
{
	m_pReportQueryModel->clear();
	m_pReportQueryModel->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("序号")));
	m_pReportQueryModel->setHorizontalHeaderItem(1, new QStandardItem(QStringLiteral("方法名称")));
	m_pReportQueryModel->setHorizontalHeaderItem(2, new QStandardItem(QStringLiteral("测试日期")));
	m_pReportQueryModel->setHorizontalHeaderItem(3, new QStandardItem(QStringLiteral("结束方式")));
	m_pReportQueryModel->setHorizontalHeaderItem(4, new QStandardItem(QStringLiteral("用户")));

	ui->tableView_reportQuery->setModel(m_pReportQueryModel);

	// 设置为列宽为265px
	ui->tableView_reportQuery->setColumnWidth(0, 265);
	ui->tableView_reportQuery->setColumnWidth(1, 265);
	ui->tableView_reportQuery->setColumnWidth(2, 265);
	ui->tableView_reportQuery->setColumnWidth(3, 265);
	ui->tableView_reportQuery->setColumnWidth(4, 265);

	// 隐藏行头
	ui->tableView_reportQuery->verticalHeader()->hide();
	// 设置整行选中
	ui->tableView_reportQuery->setSelectionBehavior(QAbstractItemView::SelectRows);
	// 设置单元格只读
	ui->tableView_reportQuery->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置horizontalHeader字体类型，字体大小
	m_pReportQueryModel->horizontalHeaderItem(0)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(1)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(2)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(3)->setFont(QFont("Microsoft YaHei", 12));
	m_pReportQueryModel->horizontalHeaderItem(4)->setFont(QFont("Microsoft YaHei", 12));

	// 设置horizontalHeader文字颜色
	m_pReportQueryModel->horizontalHeaderItem(0)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(1)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(2)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(3)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
	m_pReportQueryModel->horizontalHeaderItem(4)->setForeground(QBrush(QColor(0x97, 0x97, 0x97)));
}


/*
* 参数：
* 返回：
* 功能：切换用户操作界面
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
* 参数：
* 返回：
* 功能：切换图表显示
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
* 参数：
* 返回：
* 功能：返回正在操作的用户界面
*/
Widget::InterfaceIndex Widget::GetInterfaceIndex()
{
	return m_interfaceIndex;
}


/*
* 参数：image:显示的图像数据
* 返回：
* 功能：显示图像
*/
void Widget::ShowImage(Mat &image)
{
	QImage showImage;
	Mat resizeImage;
	Mat rgbImage;

	cv::resize(image, resizeImage, Size(427, 342));

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
* 参数：
* 返回：
* 功能：更新系统串口列表
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
* 参数：
* 返回：
* 功能：初始化压力曲线
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
	

	ui->qwtPlot->setTitle(QStringLiteral("压力曲线"));
	ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, QStringLiteral("时间（S）"));
	ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, QStringLiteral("压力（Pa）"));
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
* 参数：
* 返回：
* 功能：重置压力曲线
*/
void Widget::ResetCurve()
{
	m_vectorX.clear();
	m_vectorY.clear();

	m_pCurve->setSamples(m_vectorX.data(), m_vectorY.data(), m_vectorX.size());
	ui->qwtPlot->replot();
}


/*
* 参数：
* 返回：
* 功能：绘制压力曲线
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
* 参数：
* 返回：
* 功能：串口操作按钮提示
*/
void Widget::UpdateComUI()
{
	if (m_bIsComOpened)
	{
		ui->pushButton_connectCom->setText(QStringLiteral("断开"));
	}
	else
	{
		ui->pushButton_connectCom->setText(QStringLiteral("联机"));
	}
}


/*
* 参数：
* 返回：
* 功能：识别算法初始化
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
* 参数：
* 返回：
* 功能：更新账户信息列表
*/
void Widget::UpdateAccountList()
{
	QStringList accountList;
	QStringList testerList;
	QStringList adminList;
	bool state = false;

	// 测试员无权修改账户信息，不显示账户列表
	if (static_cast<int>(Tester) == m_account.userType)
	{
		return;
	}

	if (static_cast<int>(Developer) == m_account.userType)
	{
		state = m_pAccountDB->GetAccountList(adminList, Admin);

		// 错误处理
		if (!state)
		{
			ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
			return;
		}

		state = m_pAccountDB->GetAccountList(testerList, Tester);

		// 错误处理
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

		// 错误处理
		if (!state)
		{
			ui->label_advanceMessage->setText(m_pAccountDB->GetMessageList()[0]);
			return;
		}

		// 先添加管理员账号本身
		accountList.append(m_account.userName);

		// 添加测试员账号列表
		accountList += testerList;
	}

	m_pAccountListModel->setStringList(accountList);
	ui->listView_accountList->setModel(m_pAccountListModel);

	ui->comboBox_queryUserName->clear();
	ui->comboBox_queryUserName->addItem(QStringLiteral("请选择用户"));
	ui->comboBox_queryUserName->addItems(accountList);
	
}


/*
* 参数：
* 返回：
* 功能：更新测试方法列表
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
	ui->comboBox_selMethod->addItem(QStringLiteral("请选择测试方法"));
	ui->comboBox_selMethod->addItems(methodList);
	ui->comboBox_selMethod->setCurrentIndex(0);

	ui->comboBox_queryMethodName->clear();
	ui->comboBox_queryMethodName->addItem(QStringLiteral("请选择方法名称"));
	ui->comboBox_queryMethodName->addItems(methodList);
}


/*
* 参数：reportList:查询到的报告列表
* 返回：
* 功能：更新测试结果查询表
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
			strMode = QStringLiteral("自动结束");
			break;
		case EndBySoftware:
			strMode = QStringLiteral("软件手动结束");
			break;
		case EndByDevice:
			strMode = QStringLiteral("设备手动结束");
			break;
		case EndTimeOut:
			strMode = QStringLiteral("定时时间到");
			break;
		case EndPressureOverRange:
			strMode = QStringLiteral("压力超量程");
			break;
		case EndBurst:
			strMode = QStringLiteral("布匹涨破");
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
* 参数：state-编辑状态
* 返回：
* 功能：更新账号信息显示
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
			ui->comboBox_accountType->addItem(QStringLiteral("测试员"));
			ui->comboBox_accountType->addItem(QStringLiteral("管理员"));
		}
		else if (static_cast<int>(Admin) == m_account.userType)
		{
			ui->comboBox_accountType->addItem(QStringLiteral("测试员"));
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
		ui->comboBox_accountType->addItem(QStringLiteral("测试员"));
		ui->comboBox_accountType->addItem(QStringLiteral("管理员"));
		ui->comboBox_accountType->addItem(QStringLiteral("开发者"));
		break;
	}
}


/*
* 参数：
* 返回：
* 功能：判断目标压力值是否超量程
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
* 参数：method--测试方法参数结合
* 返回：
* 功能：根据测试方案，显示参数信息
*/
void Widget::ShowMethodParam(const STRUCT_MethodParam &method)
{
	ui->textEdit_methodInfo->append(QStringLiteral("方法名称：") + method.name);
	ui->textEdit_methodInfo->append(QStringLiteral("应用标准：") + method.standard);
	

	QString unit;

	switch (method.unit)
	{
	case 0:
		unit = QStringLiteral("Pa(帕)");
		break;
	case 1:
		unit = QStringLiteral("kPa(千帕)");
		break;
	case 2:
		unit = QStringLiteral("mBar(毫巴)");
		break;
		unit = QStringLiteral("mmH2O(毫米水柱)");
	case 3:
		break;
	default:
		break;
	}

	switch (method.plan)
	{
	case 0:
		ui->textEdit_methodInfo->append(QStringLiteral("测试方法：") + QStringLiteral("持续增压"));
		ui->textEdit_methodInfo->append(QStringLiteral("增压速率：") + QString::number(method.rate) + unit + QStringLiteral("/分钟"));
		break;
	case 1:
		ui->textEdit_methodInfo->append(QStringLiteral("测试方法：") + QStringLiteral("定时计压"));
		ui->textEdit_methodInfo->append(QStringLiteral("增压速率：") + QString::number(method.rate) + unit + QStringLiteral("/分钟"));
		ui->textEdit_methodInfo->append(QStringLiteral("定时时间：") + QString::number(method.timing) +  QStringLiteral("分钟"));
		break;
	case 2:
		ui->textEdit_methodInfo->append(QStringLiteral("测试方法：") + QStringLiteral("定时定压"));
		ui->textEdit_methodInfo->append(QStringLiteral("增压速率：") + QString::number(method.rate) + unit + QStringLiteral("/分钟"));
		ui->textEdit_methodInfo->append(QStringLiteral("定时时间：") + QString::number(method.timing) + QStringLiteral("分钟"));
		ui->textEdit_methodInfo->append(QStringLiteral("目标压强：") + QString::number(method.pressure) + unit);
		break;
	case 3:
		ui->textEdit_methodInfo->append(QStringLiteral("测试方法：") + QStringLiteral("绕曲松弛"));
		ui->textEdit_methodInfo->append(QStringLiteral("增压速率：") + QString::number(method.rate) + unit + QStringLiteral("/分钟"));
		ui->textEdit_methodInfo->append(QStringLiteral("目标压强：") + QString::number(method.pressure) + unit);
		ui->textEdit_methodInfo->append(QStringLiteral("松绕周期：") + QString::number(method.cycle) + QStringLiteral("次"));
		ui->textEdit_methodInfo->append(QStringLiteral("保压时间：") + QString::number(method.holdingTime) + QStringLiteral("分钟"));
		break;
	case 4:
		ui->textEdit_methodInfo->append(QStringLiteral("测试方法：") + QStringLiteral("渗水漏水"));
		ui->textEdit_methodInfo->append(QStringLiteral("增压速率：") + QString::number(method.rate) + unit + QStringLiteral("/分钟"));
		ui->textEdit_methodInfo->append(QStringLiteral("定时时间：") + QString::number(method.timing) + QStringLiteral("分钟"));
		ui->textEdit_methodInfo->append(QStringLiteral("目标压强：") + QString::number(method.pressure) + unit);
		break;
	default:
		break;
	}

	ui->textEdit_methodInfo->append(QStringLiteral("描述：") + method.description);
}


/*
* 参数：id:选中项目的id
* 返回：
* 功能：删除测试结果列表中选中的项目
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
* 参数：
* 返回：
* 功能：根据识别到的数珠个数，输出相应信息
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
		ui->textEdit_info->append(QStringLiteral("第%1个水珠出现时间：").arg(m_dropNum + i + 1) + strTime);
		ui->textEdit_info->append(QStringLiteral("当前压力值为：") + strPressure + QString("\n"));
	}

	m_dropNum = num;

	if (m_dropNum >= 3)
	{
		// 发送测试停止命令
		//m_pCom->TxCmd(0x01, 0x0, 0x0);
		m_txData = TxStopTest;
		m_testState = EndAuto;

		// 生成测试报告
		//GenTestReport();
	}
}

/*
* 参数：
* 返回：
* 功能：保存最后一帧图像
*/
void Widget::SaveLastImage()
{
	QPixmap pic(*ui->label_video->pixmap());
	pic.save("./report/last.bmp");
}


/*
* 参数：
* 返回：
* 功能：保存压力曲线
*/
void Widget::SavePressureCurve()
{
	m_curveRenderer.renderDocument(ui->qwtPlot, "./report/curve.bmp", QSizeF(200, 100));
}

/*
* 参数：
* 返回：
* 功能：生成测试报告
*/
void Widget::GenTestReport(void)
{
	STRUCT_Report report;

	SaveLastImage();
	SavePressureCurve();
	ui->textEdit_report->clear();

	QString htmlStr = QStringLiteral("<p align = \"center\"><font size = \"6\">耐水压成像测试系统</font></p>");
	htmlStr += QStringLiteral("<p align = \"center\"><font size = \"6\">测试报告</font></p> <p> </p> <p> </p>");

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

	htmlStr += QStringLiteral("<p><font size = \"4\"><b>测试方法与参数:</b></font></p>");
	htmlStr += QStringLiteral("<p>方法名称：%1</p>").arg(m_methodParam.name);
	htmlStr += QStringLiteral("<p>应用标准：%1</p>").arg(m_methodParam.standard);

	QString unit;
	switch (m_methodParam.unit)
	{
	case 0:
		unit = QStringLiteral("Pa(帕)");
		break;
	case 1:
		unit = QStringLiteral("kPa(千帕)");
		break;
	case 2:
		unit = QStringLiteral("mBar(毫巴)");
		break;
		unit = QStringLiteral("mmH2O(毫米水柱)");
	case 3:
		break;
	default:
		break;
	}

	switch (m_methodParam.plan)
	{
	case 0:
		htmlStr += QStringLiteral("<p>测试方法：持续增压</p>");
		htmlStr += QStringLiteral("<p>持续增压速率：%1/分钟</p>").arg(QString::number(m_methodParam.rate)  + unit);
		break;
	case 1:
		htmlStr += QStringLiteral("<p>测试方法：定时计压</p>");
		htmlStr += QStringLiteral("<p>定时计压速率：%1/分钟</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>定时计压定时时间：%1分钟</p>").arg(QString::number(m_methodParam.timing));
		break;
	case 2:
		htmlStr += QStringLiteral("<p>测试方法：定时定压</p>");
		htmlStr += QStringLiteral("<p>定时定压速率：%1/分钟</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>定时定压目标压强：%1</p>").arg(QString::number(m_methodParam.pressure) + unit);
		htmlStr += QStringLiteral("<p>定时定压定时时间：%1分钟</p>").arg(QString::number(m_methodParam.timing));
		break;
	case 3:
		htmlStr += QStringLiteral("<p>测试方法：绕曲松驰</p>");
		htmlStr += QStringLiteral("<p>绕曲松驰水速率：%1/分钟</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>绕曲松驰目标压强：%1</p>").arg(QString::number(m_methodParam.pressure) + unit);
		htmlStr += QStringLiteral("<p>绕曲松驰松绕周期：%1次</p>").arg(QString::number(m_methodParam.cycle));
		htmlStr += QStringLiteral("<p>绕曲松驰水压保持时间：%1分钟</p>").arg(QString::number(m_methodParam.holdingTime));
		break;
	case 4:
		htmlStr += QStringLiteral("<p>测试方法：渗水漏水</p>");
		htmlStr += QStringLiteral("<p>渗水漏水速率：%1/分钟</p>").arg(QString::number(m_methodParam.rate) + unit);
		htmlStr += QStringLiteral("<p>渗水漏水目标压强：%1</p>").arg(QString::number(m_methodParam.pressure) + unit);
		htmlStr += QStringLiteral("<p>渗水漏水定时时间：%1分钟</p>").arg(QString::number(m_methodParam.timing));
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p>描述：%1</p>").arg(m_methodParam.description);
	report.testDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	htmlStr += QStringLiteral("<p>   </p><p>    </p><p><b>报告生成时间：</b>%1</p>").arg(report.testDate);

	
	QString strEndType;

	switch (m_testState)
	{
	case EndAuto:
		strEndType = QStringLiteral("检测到水珠自动结束");
		break;
	case EndByDevice:
		strEndType = QStringLiteral("在设备上手动结束");
		break;
	case EndBySoftware:
		strEndType = QStringLiteral("在软件上手动结束");
		break;
	case EndBurst:
		strEndType = QStringLiteral("布匹涨破结束");
		break;
	case EndTimeOut:
		strEndType = QStringLiteral("定时时间到结束");
		break;
	case EndPressureOverRange:
		strEndType = QStringLiteral("压力超量程结束");
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p><b>测试结束方式：</b>%1</p>").arg(strEndType);

	int n = m_listAppearTime.size();
	htmlStr += QStringLiteral("<p><b>布匹上的水珠检测情况：</b></p>");

	if (n > 0)
	{
		for (int i = 0; i < n; i++)
		{
			htmlStr += QStringLiteral("<p>检测到第%1滴水珠的时间为：%2</p>").arg(i + 1).arg(m_listAppearTime[i]);
			htmlStr += QStringLiteral("<p>检测到第%1滴水珠的压力值为：%2</p>").arg(i + 1).arg(m_listApperPressure[i]);
		}
	}
	else
	{
		htmlStr += QStringLiteral("<p>未检测到布面上出现水珠！</p>");
	}

	// 获取保存的实验结果图片
	QString imgPath;
	if (EndAuto == m_testState)
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/report.bmp"));
	}
	else
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/last.bmp"));
	}

	htmlStr += QStringLiteral("<p> </p> <p align = \"center\"><b>检测结果截图</b></p>");
	htmlStr += imgPath;
	htmlStr += QStringLiteral("<p> </p> <p align = \"center\"><b>压力曲线</b></p>");
	htmlStr += QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/curve.bmp"));

	ui->textEdit_report->insertHtml(htmlStr);
	if (!m_pTestResult->AddReport(report))
	{
		ui->label_testInterfaceMessage->setText(QStringLiteral("向数据库写入试验报告错误！"));
	}
}

/*
* 参数：
* 返回：
* 功能：打印测试报告
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
* 参数：
* 返回：
* 功能：测试开始
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
	ui->textEdit_info->append(QStringLiteral("试验开始。。。"));
	ui->pushButton_startStop->setText(QStringLiteral("结束"));
	ui->label_testState->setText(QStringLiteral("正在试验"));
	SetDeviceOprateEnabled(StartStop | PauseConti);
	ui->comboBox_selMethod->setEnabled(false);
	ui->textEdit_report->clear();
}


/*
* 参数：
* 返回：
* 功能：测试结束
*/
void Widget::StopTest()
{
	//switch (m_testState)
	//{
	//case EndAuto:	// 软件检测到水珠自动结束
	//	break;
	//case EndBySoftware:	// 通过上位机按钮结束测试
	//	break;
	//case EndByDevice:	// 通过设备按钮结束测试
	//	break;
	//case EndBurst:	// 涨破
	//	break;
	//case EndPressureOverRange:	// 压力超量程
	//	break;
	//case EndTimeOut:	// 定时时间到
	//	break;
	//default:
	//	break;
	//}

	//m_testState = End;
	m_dropNum = 0;
	m_txData = TxNoData;
	m_pImgProc->StopCalc();
	ui->textEdit_info->append(QStringLiteral("试验结束！"));
	ui->pushButton_startStop->setText(QStringLiteral("开始"));
	ui->pushButton_pauseConti->setText(QStringLiteral("暂停"));
	ui->label_testState->setText(QStringLiteral("试验结束"));
	SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
	ui->comboBox_selMethod->setEnabled(true);
}


/*
* 参数：
* 返回：
* 功能：更新测试方案显示
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
* 参数：
* 返回：
* 功能：设置对设备的操作状态
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
* 参数：flag:高级设置可编辑状态
* 返回：
* 功能：设置高级设置的操作状态
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
* 参数：params：高级设置参数
* 返回：
* 功能：更新高级设置参数
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
* 参数：
* 返回：
* 功能：最小化按钮槽函数
*/
void Widget::OnBtnMinClicked()
{
	this->showMinimized();
}


/*
* 参数：
* 返回：
* 功能：关闭按钮槽函数
*/
void Widget::OnBtnCloseClicked()
{
	switch (m_testState)
	{
	case WaterInStart:
		ui->textEdit_info->append(QStringLiteral("提示：请先停止进水，再关闭软件！"));
		break;
	case WaterOffStart:
		ui->textEdit_info->append(QStringLiteral("提示：请先停止排水，再关闭软件！"));
		break;
	case Start:
	case Pause:
		ui->textEdit_info->append(QStringLiteral("提示：请先停止试验，再关闭软件！"));
		break;
	default:
		this->close();
		break;
	}
}


/*
* 参数：
* 返回：
* 功能：鼠标点击事件
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
* 参数：
* 返回：
* 功能：鼠标移动事件
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
* 参数：
* 返回：
* 功能：鼠标释放事件
*/
void Widget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_isMousePressed = false;
	}
}


/*
* 参数：
* 返回：
* 功能：点击“测试界面”，槽函数
*/
void Widget::OnBtnTestInterfaceClicked()
{
	m_interfaceIndex = TestInterface;
	SwitchInterface(m_interfaceIndex);
}


/*
* 参数：
* 返回：
* 功能：点击“测试方法”，槽函数
*/
void Widget::OnBtnTestMethodClicked()
{
	m_interfaceIndex = TestMethod;
	SwitchInterface(m_interfaceIndex);
}



/*
* 参数：
* 返回：
* 功能：点击“报告查询”，槽函数
*/
void Widget::OnBtnReportQueryClicked()
{
	m_interfaceIndex = ReportQuery;
	SwitchInterface(m_interfaceIndex);
}


/*
* 参数：
* 返回：
* 功能：点击“高级设置”，槽函数
*/
void Widget::OnBtnAdvanceClicked()
{
	m_interfaceIndex = Advance;
	SwitchInterface(m_interfaceIndex);
}


/*
* 参数：
* 返回：
* 功能：点击“用户帮助”，槽函数
*/
void Widget::OnBtnHelpClicked()
{
	m_interfaceIndex = Help;
	SwitchInterface(m_interfaceIndex);
}


/*
* 参数：
* 返回：
* 功能：图表显示选择视频功能
*/
void Widget::OnBtnChartVideoClicked()
{
	m_chartIndex = Video;
	SwitchChart(m_chartIndex);
}


/*
* 参数：
* 返回：
* 功能：图表显示选择曲线功能
*/
void Widget::OnBtnChartCurveClicked()
{
	m_chartIndex = Curve;
	SwitchChart(m_chartIndex);
}


/*
* 参数：
* 返回：
* 功能：图表显示选择报告功能
*/
void Widget::OnBtnChartReportClicked()
{
	m_chartIndex = Report;
	SwitchChart(m_chartIndex);
}



/*
* 参数：
* 返回：
* 功能：根据选择的测试方法，显示测试方法信息
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
			ui->textEdit_info->append(QStringLiteral("提示：设备未连接，无法设置测试参数！"));
		}
	}
}


/*
* 参数：
* 返回：
* 功能：答应实验报告，按钮槽函数
*/
void Widget::OnBtnPrintReportClicked()
{
	PrintReport();
}


/*
* 参数：id--账号在数据库表中的id号
* 返回：
* 功能：登陆成功后，进入主界面前的事务处理
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
* 参数：
* 返回：
* 功能：打开或关闭相机槽函数
*/
void Widget::OnBtnOpenCameraClicked()
{
	BOOL state = FALSE;

	if ((m_pCamera != NULL) && (FALSE == m_pCamera->IsCameraOpened()))
	{
		// 打开相机
		state = m_pCamera->OpenCamera();

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("打开相机失败！"));
			return;
		}

		// 设置采集
		state = m_pCamera->StartSnap(MerCamera::ContinuousMode);

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("初始化相机采集失败！"));
			m_pCamera->CloseCamera();
			return;
		}

		// 设置自动曝光
		state = m_pCamera->SetAutoExposure(GX_EXPOSURE_AUTO_CONTINUOUS);

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("相机设置自动曝光失败！"));
			m_pCamera->CloseCamera();
			return;
		}

		// 设定光照和期望灰度
		state = m_pCamera->SetExpectedGray(64);

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("相机设置期望灰度失败！"));
			m_pCamera->CloseCamera();
			return;
		}

		ui->textEdit_info->append(QStringLiteral("相机已打开！"));
		ui->pushButton_openCloseCamera->setText(QStringLiteral("关闭相机"));
	}
	else if ((m_pCamera != NULL) && (TRUE == m_pCamera->IsCameraOpened()))
	{
		state = m_pCamera->CloseCamera();

		if (FALSE == state)
		{
			ui->textEdit_info->append(QStringLiteral("关闭相机失败，请复位后重试！"));
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("相机已关闭！"));
		}

		ui->pushButton_openCloseCamera->setText(QStringLiteral("打开相机"));
	}
}


/*
* 参数：
* 返回：
* 功能：打开或关闭串口槽函数
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
			ui->textEdit_info->append(QStringLiteral("与设备断开连接！"));
			ui->pushButton_connectCom->setText(QStringLiteral("联机"));
		}	
		ui->label_testState->setText(QStringLiteral("设备未连接"));
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
			//ui->textEdit_info->append(QStringLiteral("打开串口成功！"));
			//m_pCom->TxReadState();
			m_txData = TxConnect;
			//SetDeviceOprateEnabled(false);

			//m_pCom->moveToThread(m_pRxThread);
			//m_pRxThread->start();
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("错误：打开串口失败，请检查串口设备！"));
		}
	}

	//UpdateComUI();
}


/*
* 参数：
* 返回：
* 功能：进水或停止进水命令槽函数
*/
void Widget::OnBtnWaterInClicked()
{
	// 进水命令:0x07， 开始进水：1，停止进水：0
	if (m_bIsWaterIn)
	{
		//m_pCom->TxCmd(0x07, 0x0, 0x0);
		m_txData = TxWaterInStop;
		m_btnState = WaterInStop;
		//m_bIsWaterIn = false;
		//ui->pushButton_waterIn->setText(QStringLiteral("进水"));
	}
	else
	{
		//m_pCom->TxCmd(0x07, 0x01, 0x0);
		m_txData = TxWaterInStart;
		m_btnState = WaterInStart;
		//m_bIsWaterIn = true;
		//ui->pushButton_waterIn->setText(QStringLiteral("停止进水"));
	}

	SetDeviceOprateEnabled(0);
}


/*
* 参数：
* 返回：
* 功能：排水或停止排水命令槽函数
*/
void Widget::OnBtnWaterOffClicked()
{
	// 排水命令:0x08， 开始排水：1，停止排水：0
	if (m_bIsWaterOff)
	{
		//m_pCom->TxCmd(0x08, 0x0, 0x0);
		m_txData = TxWaterOffStop;
		m_btnState = WaterOffStop;
		//m_bIsWaterOff = false;
		//ui->pushButton_waterOff->setText(QStringLiteral("排水"));
	}
	else
	{
		//m_pCom->TxCmd(0x08, 0x01, 0x0);
		m_txData = TxWaterOffStart;
		m_btnState = WaterOffStart;
		//m_bIsWaterOff = true;
		//ui->pushButton_waterOff->setText(QStringLiteral("停止排水"));
	}

	SetDeviceOprateEnabled(0);
}


/*
* 参数：
* 返回：
* 功能：开始或停止测试命令槽函数
*/
void Widget::OnBtnStartTestClicked()
{
	if ((Connected == m_testState) || (End == m_testState))
	{
		// 相机状态
		if (!m_pCamera->IsCameraOpened())
		{
			ui->textEdit_info->append(QStringLiteral("提示：请先打开相机！"));
			return;
		}

		// 串口连接状态或测试结束状态，开始新的测试，先设置参数
		if (!m_bIsParamsSet)
		{
			ui->textEdit_info->append(QStringLiteral("提示：请先选择正确的测试方法！"));
			return;
		}

		//m_testState = StartBySoftware;
		m_btnState = StartBySoftware;
		m_txData = TxStartTest;
	}
	else if ((Start == m_testState) || (Pause == m_testState))
	{
		// 测试为开始或暂停状态，停止测试
		//m_pCom->TxCmd(0x01, 0x0, 0x0);
		m_btnState = EndBySoftware;
		m_txData = TxStopTest;
		//m_testState = EndBySoftware;
	}

	// 禁止其他操作
	SetDeviceOprateEnabled(0);
	ui->comboBox_selMethod->setEnabled(false);
}


/*
* 参数：
* 返回：
* 功能：暂停或继续测试命令槽函数
*/
void Widget::OnBtnPauseTestClicked()
{
	if (Start == m_testState)
	{
		// 测试开始状态，暂停测试
		//m_txData = TxPauseTest;
		//m_testState = PauseBySoftware;
		m_btnState = PauseBySoftware;
	}
	else if (Pause == m_testState)
	{
		// 测试暂停状态，测试继续进行
		//m_txData = TxStartTest;
		//m_testState = ContinueBySoftware;
		m_btnState = ContinueBySoftware;
	}
}


/*
* 参数：rxBuf--接收到的串口数据
* 返回：
* 功能：显示接收到串口数据
*/
void Widget::OnRxDataReceived(const QByteArray &rxBuf)
{
	QString str;
	HexToString(rxBuf, str);
	ui->textEdit_info->append(str);
}


/*
* 参数：handshake--与设备通信的握手状态
* 返回：
* 功能：根据与设备通信的握手状态，做相应的提示与处理
*/
void Widget::OnHandShakeStateReceived(STRUCT_HandShake *handshake)
{
	switch (handshake->state)
	{
	case Busy:
		ui->textEdit_info->append(QStringLiteral("通信正忙，请稍后再尝试！"));
		break;
	case SetParamOk:
		if (5 == handshake->data)
		{
			ui->textEdit_info->append(QStringLiteral("压力量程设置成功！"));
			ui->label_advanceMessage->setText(QStringLiteral("压力量程设置成功！"));
			ui->comboBox_pressureRange->setEnabled(true);
		}
		else
		{
			ui->textEdit_info->append(QStringLiteral("测试参数设置成功！"));
			m_testState = Connected;
			m_bIsParamsSet = true;
			ui->comboBox_selMethod->setEnabled(true);
		}
		break;
	case SetParamError:
		if (5 == handshake->data)
		{
			ui->textEdit_info->append(QStringLiteral("压力量程设置失败！"));
			ui->label_advanceMessage->setText(QStringLiteral("压力量程设置失败！"));
			ui->comboBox_pressureRange->setEnabled(true);
		}
		else
		{
		ui->textEdit_info->append(QStringLiteral("错误：测试参数，设置失败！"));
		m_testState = Connected;
		m_bIsParamsSet = false;
		ui->comboBox_selMethod->setEnabled(true);
		}	
		//SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);	
		break;
	case SetParamAckTimeOut:
		if (5 == handshake->data)
		{
			ui->textEdit_info->append(QStringLiteral("压力量程设置，应答超时！"));
			ui->label_advanceMessage->setText(QStringLiteral("压力量程设置，应答超时！"));
			ui->comboBox_pressureRange->setEnabled(true);
		}
		else
		{
		ui->textEdit_info->append(QStringLiteral("错误：设置测试参数，等待应答超时！"));
		m_testState = Connected;
		m_bIsParamsSet = false;
		//SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
		ui->comboBox_selMethod->setEnabled(true);
		}
		break;
	case ReadParamOk:
		ui->textEdit_info->append(QStringLiteral("读取参数成功！"));
		break;
	case ReadParamError:
		ui->textEdit_info->append(QStringLiteral("错误：读取参数失败！"));
		break;
	case ReadParamAckTimeOut:
		ui->textEdit_info->append(QStringLiteral("错误：读取参数，等待应答超时，请检查与设备的连接情况！"));
		break;
	case ReadStateOk:
		if (Init == m_testState)
		{
			ui->textEdit_info->append(QStringLiteral("与设备联机成功！"));
			ui->label_testState->setText(QStringLiteral("设备已连接"));
			ui->pushButton_connectCom->setText(QStringLiteral("断开"));
			m_testState = Connected;
			SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("读取设备实时数据与状态成功！"));
		//}	
		break;
	case ReadStateError:
		if (Init == m_testState)
		{
			m_pCom->Close();
			m_bIsComOpened = false;
			ui->textEdit_info->append(QStringLiteral("错误：与设备联机失败，请检查与设备的连接情况！"));
			ui->comboBox_selCom->setEnabled(true);
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("读取设备实时数据与状态失败！"));
		//}
		break;
	case ReadStateAckTimeOut:
		if (Init == m_testState)
		{
			m_pCom->Close();
			m_bIsComOpened = false;
			ui->textEdit_info->append(QStringLiteral("错误：与设备联机失败，请检查与设备的连接情况！"));
			ui->comboBox_selCom->setEnabled(true);
			//SetDeviceOprateEnabled(true);
		}
		//else
		//{
		//	ui->textEdit_info->append(QStringLiteral("读取设备实时数据与状态，等待应答超时，请检查与设备的连接情况！"));
		//}	
		break;
	case CmdOk:
		switch (handshake->cmd)
		{
		case 0x01:	// 测试开始，暂停，结束命令，应答成功
			if (0 == handshake->data)
			{
				// 上位机操作停止命令操作成功，新的测试开始
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
				// 上位机操作暂停命令操作成功，测试暂停
				m_testState = Pause;
				m_btnState = Init;
				m_pImgProc->StopCalc();
				ui->textEdit_info->append(QStringLiteral("试验暂停。。。"));
				ui->pushButton_pauseConti->setText(QStringLiteral("继续"));
				ui->label_testState->setText(QStringLiteral("试验暂停"));
			}
			else
			{
				// 上位机操作开始命令操作成功，新的测试开始
				if (StartBySoftware == m_btnState)
				{
					StartTest();
				}
				else if (ContinueBySoftware == m_btnState)
				{
					// 测试暂停后，上位机操作开始命令成功，继续测试
					m_testState = Start;
					ui->textEdit_info->append(QStringLiteral("试验继续。。。"));
					ui->pushButton_pauseConti->setText(QStringLiteral("暂停"));
					ui->label_testState->setText(QStringLiteral("正在试验"));
				}
				m_btnState = Init;				
			}
			break;
		case 0x02:	// 压头控制，应答成功
			break;
		case 0x03:	// 灯光控制，应答成功
			break;
		case 0x04:	// 保留，应答成功
			break;
		case 0x05:	// 去皮，应答成功
			break;
		case 0x06:	// 恢复出厂控制，应答成功
			break;
		case 0x07:	// 进水控制，应答成功
			if (1 == handshake->data)
			{
				m_bIsWaterIn = true;
				ui->pushButton_waterIn->setText(QStringLiteral("停止进水"));
				ui->textEdit_info->append(QStringLiteral("开始进水！"));
				SetDeviceOprateEnabled(WaterIn);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterIn = false;
				m_testState = Connected;
				ui->pushButton_waterIn->setText(QStringLiteral("进水"));
				ui->textEdit_info->append(QStringLiteral("停止进水！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x08:	// 排水控制，应答成功
			if (1 == handshake->data)
			{
				m_bIsWaterOff = true;
				ui->textEdit_info->append(QStringLiteral("开始排水！"));
				ui->pushButton_waterOff->setText(QStringLiteral("停止排水"));
				SetDeviceOprateEnabled(WaterOff);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterOff = false;
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("停止排水！"));
				ui->pushButton_waterOff->setText(QStringLiteral("排水"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x09:	// 压力标定，应答成功
			break;
		case 0x0b:	// 通知设备当前选择测试项，应答成功
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
					ui->textEdit_info->append(QStringLiteral("错误：停止试验命令，设置失败！"));
					ui->textEdit_info->append(QStringLiteral("请重新尝试，或使用设备上的按钮停止测试！"));
					m_btnState = Init;
				}
				else
				{
					//m_testState = EndAuto;
					ui->textEdit_info->append(QStringLiteral("错误：停止命令自动设置失败！"));
					ui->textEdit_info->append(QStringLiteral("已检测到3滴或以上水珠，请手动停止测试！"));
				}
				
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else if (0xff == handshake->data)
			{
				ui->textEdit_info->append(QStringLiteral("错误：暂停试验命令，设置失败！"));
				ui->textEdit_info->append(QStringLiteral("请重新尝试！"));
				m_btnState = Init;
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else
			{
				if (StartBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("错误：开始试验命令，设置失败！"));
					ui->textEdit_info->append(QStringLiteral("请检查设备后重新尝试！"));
					m_btnState = Init;
					SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
				}
				else if (ContinueBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("错误：继续试验命令，设置失败！"));
					ui->textEdit_info->append(QStringLiteral("请重新尝试！"));
					m_btnState = Init;
					SetDeviceOprateEnabled(StartStop | PauseConti);
				}
			}
			break;
		case 0x07:	// 进水，应答错误
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("错误：进水命令，设置失败！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterIn = false;
				m_testState = Connected;
				ui->pushButton_waterIn->setText(QStringLiteral("进水"));
				ui->textEdit_info->append(QStringLiteral("错误：停止进水命令，设置失败！"));
				ui->textEdit_info->append(QStringLiteral("----请立刻在设备上手动停止进水！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x08:	// 排水，应答错误
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("错误：排水命令，设置失败！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterOff = false;
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("错误：停止排水命令，设置失败！"));
				ui->textEdit_info->append(QStringLiteral("----请在设备上手动停止排水！"));
				ui->pushButton_waterOff->setText(QStringLiteral("排水"));
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
		case 0x01:	// 测试开始，暂停，结束命令，应答超时
			if (0 == handshake->data)
			{
				if (EndBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("错误：停止试验命令，应答超时！"));
					ui->textEdit_info->append(QStringLiteral("请重新尝试，或使用设备上的按钮停止测试！"));
					m_btnState = Init;
				}
				else
				{
					ui->textEdit_info->append(QStringLiteral("错误：自动设置停止试验命令，应答超时！"));
					ui->textEdit_info->append(QStringLiteral("已检测到3滴或以上水珠，请手动停止试验！"));
				}
				
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else if (0xff == handshake->data)
			{
				ui->textEdit_info->append(QStringLiteral("错误：暂停测试命令，应答超时！"));
				ui->textEdit_info->append(QStringLiteral("请重新尝试！"));
				m_btnState = Init;
				SetDeviceOprateEnabled(StartStop | PauseConti);
			}
			else
			{
				if (StartBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("错误：开始测试命令，应答超时！"));
					ui->textEdit_info->append(QStringLiteral("请重新尝试！"));
					m_btnState = Init;
					SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
				}
				else if (ContinueBySoftware == m_btnState)
				{
					ui->textEdit_info->append(QStringLiteral("错误：开始测试命令，应答超时！"));
					ui->textEdit_info->append(QStringLiteral("请重新尝试！"));
					m_btnState = Init;
					SetDeviceOprateEnabled(StartStop | PauseConti);
				}			
			}
			break;
		case 0x02:	// 压头控制，应答超时
			break;
		case 0x03:	// 灯光控制，应答超时
			break;
		case 0x04:	// 保留，应答超时
			break;
		case 0x05:	// 去皮，应答超时
			break;
		case 0x06:	// 应答出厂控制，应答超时
			break;
		case 0x07:	// 进水控制，应答超时
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("错误：进水命令，应答超时！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterIn = false;
				m_testState = Connected;
				ui->pushButton_waterIn->setText(QStringLiteral("进水"));
				ui->textEdit_info->append(QStringLiteral("错误：停止进水命令，应答超时！"));
				ui->textEdit_info->append(QStringLiteral("----请立刻在设备上手动停止进水！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x08:	// 排水控制，应答超时
			if (1 == handshake->data)
			{
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("错误：排水命令，应答超时！"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			else if (0 == handshake->data)
			{
				m_bIsWaterOff = false;
				m_testState = Connected;
				ui->textEdit_info->append(QStringLiteral("错误：停止排水命令，应答超时！"));
				ui->textEdit_info->append(QStringLiteral("----请在设备上手动停止排水！"));
				ui->pushButton_waterOff->setText(QStringLiteral("排水"));
				SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
			}
			break;
		case 0x09:	// 压力标定，应答超时
			break;
		case 0x0b:	// 通知设备当前选择测试项，应答超时
			break;
		default:
			break;
		}
		//ui->textEdit_info->append(QStringLiteral("对设备的操作命令，应答超时，请检查与设备的连接情况！"));
		//SetDeviceOprateEnabled(ConnectDevice | WaterIn | WaterOff | StartStop | PauseConti | Camera);
		break;
	case DeviceReport:
		switch (handshake->cmd)
		{
		case 0x01:	// 压力超量程
			break;
		case 0x02:	// 涨破
			break;
		case 0x03:	// 定时时间到
			break;
		case 0x04:	// 设备就绪
			break;
		case 0x05:	// 设备上按测试按钮，通知上位机测试开始
			break;
		case 0x06:	// 设备上按测试按钮，通知上位机测试结束
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
* 参数：
* 返回：
* 功能：导出曲线图
*/
void Widget::OnBtnSaveCurveClicked()
{
	m_curveRenderer.exportTo(ui->qwtPlot, "curve.pdf");
}


/*
* 参数：index--ListView中被选中的item索引
* 返回：
* 功能：根据选中的账号，显示账号信息
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
* 参数：
* 返回：
* 功能：新增账户按钮槽函数
*/
void Widget::OnBtnNewAccountClicked()
{
	// 处于新增编辑状态时，再次点击新增按钮无效
	if (New == m_accountEditState)
	{
		return;
	}

	if (m_account.userType > static_cast<int>(Tester))
	{
		m_accountEditState = New;
		UpdateAcountInfoUI(m_accountEditState);
		
		// 清除内容
		ui->lineEdit_accountName->clear();
		ui->lineEdit_accountPassword->clear();
		ui->comboBox_accountType->setCurrentIndex(0);
	}
}


/*
* 参数：
* 返回：
* 功能：保存账户信息
*/
void Widget::OnBtnSaveAccountClicked()
{
	STRUCT_Account account;
	account.userName = ui->lineEdit_accountName->text();
	account.passward = ui->lineEdit_accountPassword->text();
	account.userType = ui->comboBox_accountType->currentIndex() + 1;

	if (account.userName.isEmpty())
	{
		ui->label_advanceMessage->setText(QStringLiteral("用户名不能为空！"));
		return;
	}

	if (account.passward.isEmpty())
	{
		ui->label_advanceMessage->setText(QStringLiteral("密码不能为空！"));
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
* 参数：
* 返回：
* 功能：删除用户账户按钮槽函数
*/
void Widget::OnBtnDeleteAccountClicked()
{
	// 新增编辑时，删除按钮无效
	if (New == m_accountEditState)
	{
		return;
	}

	QString userName = ui->lineEdit_accountName->text();

	// 账户信息为空时，删除按钮无效
	if (userName.isEmpty())
	{
		return;
	}

	if (m_account.userType > static_cast<int>(Tester))
	{
		// 不能删除账户自身
		if (userName == m_account.userName)
		{
			ui->label_advanceMessage->setText(QStringLiteral("无法删除已登录的账号！"));
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
* 参数：
* 返回：
* 功能：修改用户账户按钮槽函数
*/
void Widget::OnBtnModifyAccountClicked()
{
	// 处于新增编辑时，点击修改按钮无效
	if (New == m_accountEditState)
	{
		return;
	}

	// 处于修改编辑时，点击修改按钮无效
	if (Editable == m_accountEditState)
	{
		return;
	}

	// 账户名称为空时，点击修改按钮无效
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
* 参数：index：压力量程选项
* 返回：
* 功能：设置压力量程
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
* 参数：
* 返回：
* 功能：保存高级设置参数
*/
void Widget::OnBtnSaveAdvanceParamsClicked()
{
	int val1;
	double val2;

	val1 = ui->lineEdit_dropRec->text().toInt();
	if ((val1 < 1) || (val1 > 10))
	{
		ui->label_advanceMessage->setText(QStringLiteral("经纬密度设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.density = val1;
	}

	val1 = ui->lineEdit_flowInTime->text().toInt();
	if ((val1 < 0) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("进水时间设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.waterInTime = val1;
	}

	val2 = ui->lineEdit_dropArea->text().toDouble();
	if ((val2 < 0.01) || (val1 > 10000.0))
	{
		ui->label_advanceMessage->setText(QStringLiteral("水珠面积设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.dropletArea = val2;
	}

	val1 = ui->lineEdit_sensitivity->text().toInt();
	if ((val1 < 1) || (val1 > 5))
	{
		ui->label_advanceMessage->setText(QStringLiteral("灵敏度设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.sensitivity = val1;
	}

	val1 = ui->label_ignoreWidth->text().toInt();
	if ((val1 < 0) || (val1 > 500))
	{
		ui->label_advanceMessage->setText(QStringLiteral("忽略边缘宽度设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.ignorePixes = val1;
	}

	val1 = ui->lineEdit_centerX->text().toInt();
	if ((val1 < 1) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("视野中心X设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.x = val1;
	}

	val1 = ui->lineEdit_centerY->text().toInt();
	if ((val1 < 1) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("视野中心Y设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.y = val1;
	}

	val1 = ui->lineEdit_r->text().toInt();
	if ((val1 < 1) || (val1 > 1000))
	{
		ui->label_advanceMessage->setText(QStringLiteral("视野直径设置值不符合要求，请修改！"));
		return;
	}
	else
	{
		m_advanceParams.r = val1;
	}

	m_advanceParams.pressureRange = ui->comboBox_pressureRange->currentIndex();

	if (m_pMethodParam->UpdateAdvanceParams(2, m_advanceParams))
	{
		ui->label_advanceMessage->setText(QStringLiteral("高级设置参数修改成功！"));
	}
	else
	{
		ui->label_advanceMessage->setText(QStringLiteral("高级设置参数修改错误，请重新打开软件重试！"));
	}
}


/*
* 参数：
* 返回：
* 功能：点击测试方法列表时，显示对应的测试方法参数
*/
void Widget::OnMethodListItemClicked(const QModelIndex &index)
{
	bool state = false;
	m_methodListIndex = index.row();
	STRUCT_MethodParam method;
	
	// 点击列表内容，切换到非编辑状态
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

	// 根据测试方法类型显示测试方法参数
	switch (method.plan)
	{
	case 0:	// 持续增压
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->clear();
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case 1:	// 定时计压
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->setText(QString::number(method.timing));
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case 2:	// 定时定压
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->setText(QString::number(method.timing));
		ui->lineEdit_targetPressure->setText(QString::number(method.pressure));
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case 3:	// 绕曲松弛
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->clear();
		ui->lineEdit_targetPressure->setText(QString::number(method.pressure));
		ui->lineEdit_pressureCycle->setText(QString::number(method.cycle));
		ui->lineEdit_pressureHolding->setText(QString::number(method.holdingTime));
		break;
	case 4:	// 渗水漏水
		ui->lineEdit_pressureRate->setText(QString::number(method.rate));
		ui->lineEdit_setTime->setText(QString::number(method.timing));
		ui->lineEdit_targetPressure->setText(QString::number(method.pressure));
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	default:
		ui->label_testMethodMessage->setText(QStringLiteral("测试方法参数错误！"));
		break;
	}
}


/*
* 参数：
* 返回：
* 功能：新增测试方法按钮槽函数
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
* 参数：index--选择的下拉框项索引
* 返回：
* 功能：测试方案下拉框选择改变槽函数
*/
void Widget::OnCombMethodPlanChanged(int index)
{
	// 非编辑状态，无需更新编辑状态
	if (Disable == m_methodEditState)
	{
		return;
	}

	switch (index)
	{
	case 0:	// 持续增压
		ui->lineEdit_setTime->clear();
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	case 1:	// 定时计压
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	case 2:	// 定时定压
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->clear();
		ui->lineEdit_pressureHolding->setEnabled(false);
		break;
	case 3:	// 绕曲松弛
		ui->lineEdit_setTime->clear();
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->setEnabled(true);
		ui->lineEdit_pressureHolding->setEnabled(true);
		break;
	case 4:	// 渗水漏水
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
* 参数：index--压强单位索引
* 返回：
* 功能：根据压强单位改变，自动转换压力值
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
* 参数：
* 返回：
* 功能：保存测试方法参数按钮槽函数
*/
void Widget::OnBtnSaveMethodClicked()
{
	// 非编辑状态，不需要保存数据
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
		ui->label_testMethodMessage->setText(QStringLiteral("方法名称不能为空！"));
		return;
	}

	// 检查参数是否正确
	switch (index)
	{
	case 0:	// 持续增压
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的增压速率值！"));
			return;
		}
		break;
	case 1:	// 定时计压
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的增压速率值！"));
			return;
		}

		if (method.timing < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的定时时间值！"));
			return;
		}
		break;
	case 2:	// 定时定压
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的增压速率值！"));
			return;
		}

		if (method.timing < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的定时时间值！"));
			return;
		}

		if (method.pressure < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的目标压强值！"));
			return;
		}
		break;
	case 3:	// 绕曲松弛
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的增压速率值！"));
			return;
		}

		if (method.pressure < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的目标压强值！"));
			return;
		}

		if (method.cycle < 1)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的松绕周期值！"));
			return;
		}

		if (method.holdingTime < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的保压时间值！"));
			return;
		}
		break;
	case 4:	// 渗水漏水
		if (method.rate < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的增压速率值！"));
			return;
		}

		if (method.timing < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的定时时间值！"));
			return;
		}

		if (method.pressure < 0.01)
		{
			ui->label_testMethodMessage->setText(QStringLiteral("请设置正确的目标压强值！"));
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
			ui->label_testMethodMessage->setText(QStringLiteral("新增测试方法成功！"));

			// 更新列表
			UpdateTestMethodList();

			// 编辑状态切换为Disable
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
			ui->label_testMethodMessage->setText(QStringLiteral("修改测试方法成功！"));

			// 更新列表
			UpdateTestMethodList();

			// 编辑状态切换为Disable
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
* 参数：
* 返回：
* 功能：根据列表中选中的条目，删除一条测试方法
*/
void Widget::OnBtnDeleteMethodClicked()
{
	// 新增编辑时，删除按钮无效
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
			ui->label_testMethodMessage->setText(QStringLiteral("删除测试方法成功！"));

			// 更新列表
			UpdateTestMethodList();

			// 索引设定为未选中状态，并清除原有的参数显示
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

			// 编辑状态切换为Disable
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
		ui->label_testMethodMessage->setText(QStringLiteral("未选中有效的测试方法条目！"));
	}
}


/*
* 参数：
* 返回：
* 功能：修改测试方法参数按钮槽函数
*/
void Widget::OnBtnModifyMethodClicked()
{
	if (m_methodListIndex < 0)
	{
		ui->label_testMethodMessage->setText(QStringLiteral("未选中有效的测试方法条目！"));
		return;
	}

	// 新增或修改编辑状态下，修改按钮无效
	if ((New == m_methodEditState) || (Editable == m_methodEditState))
	{
		return;
	}

	m_methodEditState = Editable;
	UpdateMethodInfoUI(m_methodEditState);
}


/*
* 参数：
* 返回：
* 功能：根据选择的条件，查询测试结果数据库
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
* 参数：
* 返回：
* 功能：删除选中的测试结果报告
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
* 参数：
* 返回：
* 功能：显示图像
*/
void Widget::OnImagePrepared()
{
	ShowImage(m_pImgProc->GetImage());
	m_pCamera->ResetImgSnapedFlag();
}


/*
* 参数：
* 返回：
* 功能：定时器槽函数
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
		// 显示测试时间与设备当前压力值
		m_time = m_time.addSecs(1);
		ui->lcdNumber_time->display(m_time.toString("hh:mm:ss"));
		ui->lcdNumber_pressure->display(m_pCom->GetCurrentPressure());

		// 获取检测到的水珠数
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
* 参数：
* 返回：
* 功能：更新状态信息
*/
void Widget::OnUpdateDeviceState(STRUCT_DeviceState &deviceState)
{
	ui->lcdNumber_pressure->display(QString::number(deviceState.pressure));
}

/*
* 参数：
* 返回：
* 功能：更新压力信息
*/
void Widget::OnCurrentPressureReceived(int pressure)
{
	ui->textEdit_info->append(QString::number(pressure));
}

/*
* 参数：
* 返回：
* 功能：
*/


/*
* 参数：
* 返回：
* 功能：
*/


/*
* 参数：
* 返回：
* 功能：
*/


/*
* 参数：
* 返回：
* 功能：
*/

/*
* 参数：
* 返回：
* 功能：
*/

/*
* 参数：
* 返回：
* 功能：
*/

/*
* 参数：
* 返回：
* 功能：
*/