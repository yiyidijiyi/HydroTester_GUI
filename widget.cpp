/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-08
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
	: QWidget(parent),
	ui(new Ui::Widget),
	m_isMousePressed(false),
	m_mousePos(QPoint(0, 0)),
	m_pReportQueryModel(NULL),
	m_interfaceIndex(TestInterface),
	m_chartIndex(Video)
{
	CreateUi();

	connect(ui->pushButton_min, &QPushButton::clicked, this, &Widget::OnBtnMinClicked);
	connect(ui->pushButton_close, &QPushButton::clicked, this, &Widget::OnBtnCloseClicked);

	connect(ui->pushButton_testInterface, &QPushButton::clicked, this, &Widget::OnBtnTestInterfaceClicked);
	connect(ui->pushButton_testMethod, &QPushButton::clicked, this, &Widget::OnBtnTestMethodClicked);
	connect(ui->pushButton_advance, &QPushButton::clicked, this, &Widget::OnBtnAdvanceClicked);
	connect(ui->pushButton_reportQuery, &QPushButton::clicked, this, &Widget::OnBtnReportQueryClicked);
	connect(ui->pushButton_help, &QPushButton::clicked, this, &Widget::OnBtnHelpClicked);

	connect(ui->pushButton_video, &QPushButton::clicked, this, &Widget::OnBtnChartVideoClicked);
	connect(ui->pushButton_curve, &QPushButton::clicked, this, &Widget::OnBtnChartCurveClicked);
	connect(ui->pushButton_chart, &QPushButton::clicked, this, &Widget::OnBtnChartChartClicked);
	connect(ui->pushButton_report, &QPushButton::clicked, this, &Widget::OnBtnChartReportClicked);
	connect(ui->pushButton_print, &QPushButton::clicked, this, &Widget::OnBtnChartPrintClicked);
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
Widget::~Widget()
{
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
		"QLineEdit{font-family:'Microsoft YaHei'; font-size:14px; color:#979797; background-color:#f7f7f7;}"
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

	// ������Ƶ�طŰ�ť��ʽ
	ui->pushButton_playback->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:16px; color:#979797}"
		"QPushButton:hover{color:#ffffff; border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn1.png);}");

	// ͼ����ʾ����ѡ��
	SwitchChart(m_chartIndex);

	/*
	* ���ü�ⷽ��������ʽ
	*/
	ui->label_methodList->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_methodParams->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");

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
	m_pReportQueryModel = new QStandardItemModel();
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

	/*
	* ���ø߼����ý�����ʽ
	*/
	ui->label_accountList->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");
	ui->label_accountInfo->setStyleSheet("QLabel{background-color:#53a4ff; color:#ffffff}");

	ui->pushButton_addAccount->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
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
	ui->pushButton_chart->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';font-size:14px; color:#979797}"
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
	case Chart:
		ui->pushButton_chart->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
		break;
	case Report:
		ui->pushButton_report->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
		break;
	case Print:
		ui->pushButton_print->setStyleSheet("QPushButton{border-image: url(:/testInterface/resource/testInterface/testInterfaceBtn3.png); color:#ffffff}");
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
* ���ܣ���С����ť�ۺ���
*/
void Widget::OnBtnMinClicked()
{
	showMinimized();
}


/*
* ������
* ���أ�
* ���ܣ��رհ�ť�ۺ���
*/
void Widget::OnBtnCloseClicked()
{
	close();
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
* ���ܣ�ͼ����ʾѡ��ͼ����
*/
void Widget::OnBtnChartChartClicked()
{
	m_chartIndex = Chart;
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
	m_chartIndex = Print;
	SwitchChart(m_chartIndex);
}




/*
* ������
* ���أ�
* ���ܣ�
*/