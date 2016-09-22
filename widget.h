/*
* 创建日期：2016-09-02
* 最后修改：2016-09-19
* 作      者：syf
* 描      述：
*/
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QDateTime>
#include <QCalendarWidget>
#include <QtSql/QSqlRelationalTableModel>
#include <QStringListModel>
#include <QThread>

#include "common.h"
#include "useraccount.h"
#include "methodparam.h"
#include "testresult.h"
#include "serialport.h"
#include "mercamera.h"
#include "ui_widget.h"

// 自定义标题栏大小
#define TITLEBAR_WIDTH	1366
#define TITLEBAR_HEIGHT	130

// 自定义标题栏鼠标可拖拽范围
#define DRAG_POS_MIN_X	5
#define DRAG_POS_MAX_X	1360
#define DRAG_POS_MIN_Y	5
#define DRAG_POS_MAX_Y	125





class Widget : public QWidget
{
	Q_OBJECT

public:
	enum InterfaceIndex{
		TestInterface = 0,
		TestMethod	= 1,
		ReportQuery = 2,
		Advance		= 3,
		Help				= 4
	};

	enum ChartIndex{
		Video	= 0,
		Curve	= 1,
		Report  = 2,
	};

	enum UIState{
		Disable		= 1,
		Editable	= 2,
		New			= 3
	};

	Widget(QWidget *parent = 0);
	~Widget();

	void CreateUi();
	void CreateReportViewTable();
	void SwitchInterface(InterfaceIndex index);
	void SwitchChart(ChartIndex index);
	Widget::InterfaceIndex GetInterfaceIndex(void);

private:
	/*
	* 显示图像
	*/
	void ShowImage(Mat &image);

	/*
	* 串口相关
	*/
	void InitSerialPort();
	void UpdateComUI();

	/*
	* 更新信息列表
	*/
	void UpdateAccountList();
	void UpdateTestMethodList();
	void UpdateReportQueryView(const QList<STRUCT_Reprot> &reportList);
	
	/*
	* 更新UI状态
	*/
	void UpdateAcountInfoUI(UIState state);
	void UpdateMethodInfoUI(UIState state);

	/*
	* 设置值超限判断
	*/
	bool IsPressureOverload(double p);

	/*
	* 压强单位转换
	*/
	void ConvertPressureUnit(double &pressure, ENUM_PressureUnit unit0, ENUM_PressureUnit unit1);

	/*
	* 显示测试方法参数信息
	*/
	void ShowMethodParam(const STRUCT_MethodParam &method);

	/*
	* 删除测试结果列表中选中的项目
	*/
	void DeleteReportInList(int id);

protected slots:
	void OnBtnMinClicked();
	void OnBtnCloseClicked();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

public slots:
	/*
	* 登录成功
	*/
	void OnLoginAccepted(int id);

	/*
	* 设备操作
	*/
	void OnBtnOpenCameraClicked();
	void OnBtnComOpClicked();
	void OnBtnWaterInClicked();
	void OnBtnStartTestClicked();
	void OnBtnPauseTestClicked();
	void OnBtnWaterOffClicked();
	void OnRxDataReceived(const QByteArray& rxBuf);
	void OnHandShakeStateReceived(STRUCT_HandShake &handshake);

	/*
	* 操作界面切换
	*/
	void OnBtnTestInterfaceClicked();
	void OnBtnTestMethodClicked();
	void OnBtnReportQueryClicked();
	void OnBtnAdvanceClicked();
	void OnBtnHelpClicked();

	/*
	* 测试界面操作
	*/
	void OnBtnChartVideoClicked();
	void OnBtnChartCurveClicked();
	void OnBtnChartReportClicked();
	void OnBtnChartPrintClicked();
	void OnCombSelMethodChanged(int index);

	/*
	* 测试方法操作
	*/
	void OnMethodListItemClicked(const QModelIndex &index);
	void OnCombMethodPlanChanged(int index);
	void OnCombPressureUnitChanged(int index);
	void OnBtnNewMethodClicked();
	void OnBtnSaveMethodClicked();
	void OnBtnDeleteMethodClicked();
	void OnBtnModifyMethodClicked();

	/*
	* 报告查询操作
	*/
	void OnBtnQueryClicked();
	void OnBtnDeleteReportListClicked();

	/*
	* 帐号信息操作
	*/
	void OnAccountListItemClicked(const QModelIndex &index);
	void OnBtnNewAccountClicked();
	void OnBtnSaveAccountClicked();
	void OnBtnDeleteAccountClicked();
	void OnBtnModifyAccountClicked();
private:
	Ui::Widget* ui;

	bool m_isMousePressed;
	QPoint m_mousePos;

	QStandardItemModel* m_pReportQueryModel;
	InterfaceIndex m_interfaceIndex;
	ChartIndex	      m_chartIndex;

	// 测试方法相关
	QStringListModel *m_pMethodListModel;
	MethodParam	*m_pMethodParam;
	UIState	m_methodEditState;
	STRUCT_MethodParam m_methodParam;
	int	m_methodListIndex;
	int	m_currentUnitIndex;

	// 测试结果查询相关
	QList<STRUCT_Reprot> m_reportList;
	TestResult	*m_pTestResult;

	// 用户账号相关
	QStringListModel	*m_pAccountListModel;
	STRUCT_Account	m_account;
	UserAccount			*m_pAccountDB;
	UIState					m_accountEditState;

	// 串口相关
	QThread  *m_pRxThread;
	SerialPort *m_pCom;
	bool m_bIsComOpened;

	// 相机
	MerCamera *m_pCamera;

	// 测试状态
	ENUM_TestState m_testState;

	// 设备操作
	bool m_bIsWaterIn;
	bool m_bIsWaterOut;
};

#endif // WIDGET_H
