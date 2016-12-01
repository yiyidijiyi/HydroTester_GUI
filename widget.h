/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-11-30
* ��      �ߣ�syf
* ��      ����
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
#include <QDesktopServices>
#include <QDir>
#include <QFile>

#include "common.h"
#include "useraccount.h"
#include "methodparam.h"
#include "testresult.h"
#include "serialport.h"
#include "mercamera.h"
#include "imageproc.h"
#include "reportwindow.h"
#include "ui_widget.h"

#include "QWT/qwt_plot_curve.h"
#include "QWT/qwt_plot_grid.h"
//#include "QWT/qwt_plot_magnifier.h"
//#include "QWT/qwt_plot_panner.h"
#include "QWT/qwt_legend.h"
#include "QWT/qwt_plot_renderer.h"

// �Զ����������С
#define TITLEBAR_WIDTH	1366
#define TITLEBAR_HEIGHT	130

// �Զ��������������ק��Χ
#define DRAG_POS_MIN_X	5
#define DRAG_POS_MAX_X	1360
#define DRAG_POS_MIN_Y	5
#define DRAG_POS_MAX_Y	125

// ѹ������Ĭ�Ϻ��������
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

	enum buttonName{
		ConnectDevice = 0x20,
		WaterIn	= 0x10,
		WaterOff = 0x08,
		StartStop = 0x04,
		PauseConti = 0x02,
		Camera = 0x01
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
	* ��ʾͼ��
	*/
	void ShowImage(Mat &image);

	/*
	* �������
	*/
	void InitSerialPort();
	void UpdateComUI();

	/*
	* �㷨���
	*/
	void InitImageProc();

	/*
	* ѹ���������
	*/
	void InitCurve();
	void ResetCurve();
	void DrawCurve();

	/*
	* ������Ϣ�б�
	*/
	void UpdateAccountList();
	void UpdateTestMethodList();
	void UpdateReportQueryView(const QList<STRUCT_Report> &reportList);
	
	/*
	* ����UI״̬
	*/
	void UpdateAcountInfoUI(UIState state);
	void UpdateMethodInfoUI(UIState state);
	void SetDeviceOprateEnabled(quint8 state);
	void SetAdvanceSettingEnabled(bool state);
	void UpdateAdvanceParams(const STRUCT_AdvanceParams &params);

	/*
	* ����ֵ�����ж�
	*/
	bool IsPressureOverload(double p);

	/*
	* ѹǿ��λת��
	*/
	double ConvertPressureUnit(double pressure, ENUM_PressureUnit unit0, ENUM_PressureUnit unit1);

	/*
	* ��ʾ���Է���������Ϣ
	*/
	void ShowMethodParam(const STRUCT_MethodParam &method);

	/*
	* ɾ�����Խ���б���ѡ�е���Ŀ
	*/
	void DeleteReportInList(int id);

	/*
	* �ж�ˮ�����
	*/
	void GetDropNum();

	/*
	* ���ɡ���ӡ����
	*/
	void SaveLastImage(const QString &s);
	void SavePressureCurve(const QString &s);
	void GenTestReport();
	void GenTestReport(const STRUCT_Report &report);
	void PrintReport();

	/*
	* ��ʼ����������
	*/
	void StartTest();
	void StopTest();

protected slots:
	void OnBtnMinClicked();
	void OnBtnCloseClicked();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

public slots:
	/*
	* ��¼�ɹ�
	*/
	void OnLoginAccepted(int id);

	/*
	* �豸����
	*/
	void OnBtnOpenCameraClicked();
	void OnBtnComOpClicked();
	void OnBtnWaterInClicked();
	void OnBtnStartTestClicked();
	void OnBtnPauseTestClicked();
	void OnBtnWaterOffClicked();
	void OnRxDataReceived(const QByteArray& rxBuf);
	void OnHandShakeStateReceived(STRUCT_HandShake *handshake);
	void OnBtnSaveCurveClicked();
	void OnBtnPlayBackClicked();

	/*
	* ���������л�
	*/
	void OnBtnTestInterfaceClicked();
	void OnBtnTestMethodClicked();
	void OnBtnReportQueryClicked();
	void OnBtnAdvanceClicked();
	void OnBtnHelpClicked();

	/*
	* ���Խ������
	*/
	void OnBtnChartVideoClicked();
	void OnBtnChartCurveClicked();
	void OnBtnChartReportClicked();
	void OnCombSelMethodChanged(int index);
	void OnBtnPrintReportClicked();

	/*
	* ���Է�������
	*/
	void OnMethodListItemClicked(const QModelIndex &index);
	void OnCombMethodPlanChanged(int index);
	void OnCombPressureUnitChanged(int index);
	void OnBtnNewMethodClicked();
	void OnBtnSaveMethodClicked();
	void OnBtnDeleteMethodClicked();
	void OnBtnModifyMethodClicked();

	/*
	* �����ѯ����
	*/
	void OnBtnQueryClicked();
	void OnBtnDeleteReportListClicked();
	void OnBtnGenReportClicked();
	void OnItemDouleClicked(const QModelIndex &index);

	/*
	* �ʺ���Ϣ����
	*/
	void OnAccountListItemClicked(const QModelIndex &index);
	void OnBtnNewAccountClicked();
	void OnBtnSaveAccountClicked();
	void OnBtnDeleteAccountClicked();
	void OnBtnModifyAccountClicked();

	/*
	* �߼����ò���
	*/
	void OnCombPressureRangeChanged(int index);
	void OnBtnSaveAdvanceParamsClicked();

	/*
	* ͼ����ʾ
	*/
	void OnImagePrepared();

	/*
	* ����
	*/
	void OnTimer();
	void OnUpdateDeviceState(STRUCT_DeviceState &deviceState);
	void OnCurrentPressureReceived(int pressure);
private:
	Ui::Widget* ui;

	bool m_isMousePressed;
	QPoint m_mousePos;

	QStandardItemModel* m_pReportQueryModel;
	InterfaceIndex	m_interfaceIndex;
	ChartIndex	      m_chartIndex;

	// ���Է������
	QStringListModel *m_pMethodListModel;
	MethodParam	*m_pMethodParam;
	UIState	m_methodEditState;
	STRUCT_MethodParam m_methodParam;
	int	m_methodListIndex;
	int	m_currentUnitIndex;

	// ���Խ����ѯ���
	QList<STRUCT_Report> m_reportList;
	TestResult	 *m_pTestResult;
	VideoWriter *m_pVideoWriter;

	// �û��˺����
	QStringListModel	*m_pAccountListModel;
	STRUCT_Account	m_account;
	UserAccount		*m_pAccountDB;
	UIState					m_accountEditState;

	// �������
	QThread  *m_pRxThread;
	SerialPort *m_pCom;
	bool m_bIsComOpened;

	// �߼�����
	STRUCT_AdvanceParams m_advanceParams;

	// �㷨
	QThread *m_pImgProcThread;
	ImageProc *m_pImgProc;
	int m_dropNum;
	QStringList m_listAppearTime;
	QStringList m_listAppearPressure;

	// ���
	MerCamera *m_pCamera;

	// ����״̬
	ENUM_TestState m_testState;
	ENUM_TestState m_btnState;
	ENUM_TxData m_txData;
	QTime m_time;
	QTimer *m_pTimer;
	bool m_bIsParamsSet;

	// �豸����
	bool m_bIsWaterIn;
	bool m_bIsWaterOff;

	// ѹ������
	QVector<double> m_vectorX;
	QVector<double> m_vectorY;
	QwtPlotCurve	*m_pCurve;
	QwtPlotGrid *m_pGrid;
	QwtPlotRenderer m_curveRenderer;
	size_t	m_oldSize;
	int m_maxY;
	double m_avgY;
};

#endif // WIDGET_H
