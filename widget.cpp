/*
* 创建日期：2016-09-02
* 最后修改：2016-09-14
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
	,ui(new Ui::Widget)
	,m_isMousePressed(false)
	,m_mousePos(QPoint(0, 0))
	,m_pReportQueryModel(NULL)
	,m_interfaceIndex(TestInterface)
	,m_chartIndex(Video)
	, m_accountEditState(Disable)
{
	CreateUi();

	// 初始化用户账号相关数据成员
	m_pAccountListModel = new QStringListModel(this);

	m_account.userType = 0;
	m_account.userName = "";
	m_account.passward = "";
	m_account.id = 0;

	m_pAccountDB = new UserAccount();


	// 链接信号与槽
	connect(ui->pushButton_min, &QPushButton::clicked, this, &Widget::OnBtnMinClicked);
	connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::OnBtnCloseClicked);

	connect(ui->pushButton_testInterface, &QPushButton::clicked, this, &Widget::OnBtnTestInterfaceClicked);
	connect(ui->pushButton_testMethod, &QPushButton::clicked, this, &Widget::OnBtnTestMethodClicked);
	connect(ui->pushButton_advance, &QPushButton::clicked, this, &Widget::OnBtnAdvanceClicked);
	connect(ui->pushButton_reportQuery, &QPushButton::clicked, this, &Widget::OnBtnReportQueryClicked);
	connect(ui->pushButton_help, &QPushButton::clicked, this, &Widget::OnBtnHelpClicked);

	connect(ui->pushButton_video, &QPushButton::clicked, this, &Widget::OnBtnChartVideoClicked);
	connect(ui->pushButton_curve, &QPushButton::clicked, this, &Widget::OnBtnChartCurveClicked);
	connect(ui->pushButton_report, &QPushButton::clicked, this, &Widget::OnBtnChartReportClicked);
	connect(ui->pushButton_print, &QPushButton::clicked, this, &Widget::OnBtnChartPrintClicked);

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
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; background-color:#f7f7f7;}"
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

	/*
	* 设置检测方法界面样式
	*/
	ui->label_methodList->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_methodParams->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");

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

	ui->listView_accountList->setStyleSheet("font-family:'Microsoft YaHei'; font-size:14px; color:#979797");

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
* 功能：最小化按钮槽函数
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
* 功能：更新测试方案显示
*/
void Widget::UpdateMethodInfoUI(UIState state)
{
	ui->comboBox_plan->setEnabled(true);

	int index = 0;

	switch(state)
	{
	case New:
		ui->comboBox_plan->setEnabled(true);
		ui->comboBox_unit->setEnabled(true);
		index = 0;
		UpdateEditableMethodInfoUI(index);

		ui->comboBox_plan->setCurrentIndex(0);
		ui->comboBox_unit->setCurrentIndex(0);

		ui->lineEdit_methodName->clear();
		ui->lineEdit_standard->clear();
		ui->lineEdit_pressureRate->clear();
		ui->lineEdit_setTime->clear();
		ui->lineEdit_targetPressure->clear();
		ui->lineEdit_pressureCycle->clear();
		ui->lineEdit_pressureHolding->clear();
		break;
	case Editable:
		ui->comboBox_plan->setEnabled(true);
		ui->comboBox_unit->setEnabled(true);
		index = ui->comboBox_plan->currentIndex();
		UpdateEditableMethodInfoUI(index);
		break;
	case Disable:
		ui->comboBox_plan->setEnabled(false);
		ui->comboBox_unit->setEnabled(false);

		ui->lineEdit_methodName->setEnabled(false);
		ui->lineEdit_standard->setEnabled(false);
		ui->lineEdit_pressureRate->setEnabled(false);
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->setEnabled(false);
	default:
		break;
	}
}


/*
* 参数：index--选择的测试方法
* 返回：
* 功能：根据选择的测试方法，更新可编辑的测试信息UI
*/
void Widget::UpdateEditableMethodInfoUI(int index)
{
	switch (index)
	{
	case 0:	// 持续增压		
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->setEnabled(false);
		ui->textEdit_discription->setEnabled(true);
		break;
	case 1:	// 定时计压
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->setEnabled(false);
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->setEnabled(false);
		ui->textEdit_discription->setEnabled(true);
		break;
	case 2:	// 定时定压
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->setEnabled(false);
		ui->textEdit_discription->setEnabled(true);
		break;
	case 3:	// 绕曲松弛
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->lineEdit_setTime->setEnabled(false);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->setEnabled(true);
		ui->lineEdit_pressureHolding->setEnabled(true);
		ui->textEdit_discription->setEnabled(true);
		break;
	case 4:	// 渗水漏水
		ui->lineEdit_methodName->setEnabled(true);
		ui->lineEdit_standard->setEnabled(true);
		ui->lineEdit_pressureRate->setEnabled(true);
		ui->lineEdit_setTime->setEnabled(true);
		ui->lineEdit_targetPressure->setEnabled(true);
		ui->lineEdit_pressureCycle->setEnabled(false);
		ui->lineEdit_pressureHolding->setEnabled(false);
		ui->textEdit_discription->setEnabled(true);
		break;
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
* 功能：
*/