/*
* 创建日期：2016-09-02
* 最后修改：2016-09-16
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
{
	CreateUi();

	// 初始化用户账号相关数据成员
	m_pAccountListModel = new QStringListModel(this);

	m_account.userType = 0;
	m_account.userName = "";
	m_account.passward = "";
	m_account.id = 0;

	m_pAccountDB = new UserAccount();

	// 初始化测试方法相关数据成员
	m_pMethodListModel = new QStringListModel(this);
	m_pMethodParam = new MethodParam();

	/*
	* 链接信号与槽
	*/
	
	connect(ui->pushButton_min, &QPushButton::clicked, this, &Widget::OnBtnMinClicked);
	connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::OnBtnCloseClicked);

	// 界面切换
	connect(ui->pushButton_testInterface, &QPushButton::clicked, this, &Widget::OnBtnTestInterfaceClicked);
	connect(ui->pushButton_testMethod, &QPushButton::clicked, this, &Widget::OnBtnTestMethodClicked);
	connect(ui->pushButton_advance, &QPushButton::clicked, this, &Widget::OnBtnAdvanceClicked);
	connect(ui->pushButton_reportQuery, &QPushButton::clicked, this, &Widget::OnBtnReportQueryClicked);
	connect(ui->pushButton_help, &QPushButton::clicked, this, &Widget::OnBtnHelpClicked);

	// 测试界面操作
	connect(ui->pushButton_video, &QPushButton::clicked, this, &Widget::OnBtnChartVideoClicked);
	connect(ui->pushButton_curve, &QPushButton::clicked, this, &Widget::OnBtnChartCurveClicked);
	connect(ui->pushButton_report, &QPushButton::clicked, this, &Widget::OnBtnChartReportClicked);
	connect(ui->pushButton_print, &QPushButton::clicked, this, &Widget::OnBtnChartPrintClicked);
	connect(ui->comboBox_selMethod, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombSelMethodChanged);

	// 测试方法操作
	connect(ui->listView_methodList, &QListView::clicked, this, &Widget::OnMethodListItemClicked);
	connect(ui->pushButton_addMethod, &QPushButton::clicked, this, &Widget::OnBtnNewMethodClicked);
	connect(ui->comboBox_plan, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombMethodPlanChanged);
	connect(ui->comboBox_unit, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::OnCombPressureUnitChanged);
	connect(ui->pushButton_saveMethod, &QPushButton::clicked, this, &Widget::OnBtnSaveMethodClicked);
	connect(ui->pushButton_delMethod, &QPushButton::clicked, this, &Widget::OnBtnDeleteMethodClicked);
	connect(ui->pushButton_modifyMethod, &QPushButton::clicked, this, &Widget::OnBtnModifyMethodClicked);

	// 账户信息操作
	connect(ui->listView_accountList, &QListView::clicked, this, &Widget::OnAccountListItemClicked);
	connect(ui->pushButton_newAccount, &QPushButton::clicked, this, &Widget::OnBtnNewAccountClicked);
	connect(ui->pushButton_saveAccount, &QPushButton::clicked, this, &Widget::OnBtnSaveAccountClicked);
	connect(ui->pushButton_delAccount, &QPushButton::clicked, this, &Widget::OnBtnDeleteAccountClicked);
	connect(ui->pushButton_modifyAccount, &QPushButton::clicked, this, &Widget::OnBtnModifyAccountClicked);
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
Widget::~Widget()
{
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

	// 设置通用样式
	this->setStyleSheet("QLabel{font-family:'Microsoft YaHei'; font-size:14px;color:#979797;}"
		"QPushButton{font-family:'Microsoft YaHei'; font-size:14px;}"
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; }"
		"QTextEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; }"
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

	// 设置视频回放按钮样式
	ui->pushButton_playback->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:16px; color:#979797}"
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
	m_pReportQueryModel = new QStandardItemModel();
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
	ui->textEdit_methodInfo->append(QStringLiteral("描述：") + method.discription);

	QString unit;

	switch (method.uint)
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
* 功能：最小化按钮槽函数
*/
void Widget::OnBtnMinClicked()
{
	showMinimized();
}


/*
* 参数：
* 返回：
* 功能：关闭按钮槽函数
*/
void Widget::OnBtnCloseClicked()
{
	close();
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
* 功能：图表显示选择打印功能
*/
void Widget::OnBtnChartPrintClicked()
{
	
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

	STRUCT_MethodParam method;
	bool state = false;

	state = m_pMethodParam->GetMethodInfo(index - 1, method);

	if (!state)
	{
		ui->label_testInterfaceMessage->setText(m_pMethodParam->GetMessageList()[0]);
	}
	else
	{
		ShowMethodParam(method);
	}
}


/*
* 参数：
* 返回：
* 功能：
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
	ui->textEdit_discription->setText(method.discription);
	ui->comboBox_unit->setCurrentIndex(method.uint);
	m_currentUnitIndex = method.uint;

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
	method.discription = ui->textEdit_discription->toPlainText();
	method.uint = ui->comboBox_unit->currentIndex();

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
* 功能：
*/