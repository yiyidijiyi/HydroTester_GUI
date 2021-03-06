/*
* 创建日期：2016-09-02
* 最后修改：2016-11-01
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
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QDebug>

#include "common.h"
#include "useraccount.h"
#include "methodparam.h"
#include "testresult.h"
#include "serialport.h"
#include "mercamera.h"
#include "imageproc.h"
#include "ui_widget.h"

#include "QWT/qwt_plot_curve.h"
//#include "QWT/qwt_plot_magnifier.h"
//#include "QWT/qwt_plot_panner.h"
#include "QWT/qwt_legend.h"
#include "QWT/qwt_plot_renderer.h"

// 自定义标题栏大小
#define TITLEBAR_WIDTH	1366
#define TITLEBAR_HEIGHT	130

// 自定义标题栏鼠标可拖拽范围
#define DRAG_POS_MIN_X	5
#define DRAG_POS_MAX_X	1360
#define DRAG_POS_MIN_Y	5
#define DRAG_POS_MAX_Y	125

// 压力曲线默认横坐标点数
#define DEFAUT_CURVE_X	60



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
	* 算法相关
	*/
	void InitImageProc();

	/*
	* 压力曲线相关
	*/
	void InitCurve();
	void ResetCurve();
	void DrawCurve();

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
	void SetDeviceOprateEnabled(quint8 state);

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

	/*
	* 判断水珠个数
	*/
	void GetDropNum();

	/*
	* 生成、打印报告
	*/
	void GenTestReport();
	void PrintReport();

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
	void OnCombSelMethodChanged(int index);
	void OnBtnPrintReportClicked();

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

	/*
	* 图像显示
	*/
	void OnImagePrepared();

	/*
	* 测试
	*/
	void OnTimer();
	void OnUpdateDeviceState(STRUCT_DeviceState &deviceState);
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

	// 算法
	QThread *m_pImgProcThread;
	ImageProc *m_pImgProc;
	int m_dropNum;

	// 相机
	MerCamera *m_pCamera;

	// 测试状态
	ENUM_TestState m_testState;
	QTime m_time;
	QTimer *m_pTimer;

	// 设备操作
	bool m_bIsWaterIn;
	bool m_bIsWaterOut;

	// 压力曲线
	QVector<double> m_vectorX;
	QVector<double> m_vectorY;
	QwtPlotCurve	*m_pCurve;
	size_t	m_oldSize;
	int m_maxY = 0;
};

#endif // WIDGET_H
