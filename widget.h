/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-16
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

#include "common.h"
#include "useraccount.h"
#include "methodparam.h"
#include "ui_widget.h"

// �Զ����������С
#define TITLEBAR_WIDTH	1366
#define TITLEBAR_HEIGHT	130

// �Զ��������������ק��Χ
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
	void SwitchInterface(InterfaceIndex index);
	void SwitchChart(ChartIndex index);
	Widget::InterfaceIndex GetInterfaceIndex(void);

private:
	/*
	* ������Ϣ�б�
	*/
	void UpdateAccountList();
	void UpdateTestMethodList();
	
	/*
	* ����UI״̬
	*/
	void UpdateAcountInfoUI(UIState state);
	void UpdateMethodInfoUI(UIState state);

	/*
	* ����ֵ�����ж�
	*/
	bool IsPressureOverload(double p);

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
	void OnBtnChartPrintClicked();
	void OnCombSelMethodChanged(int index);

	/*
	* ���Է�������
	*/
	void OnMethodListItemClicked(const QModelIndex &index);
	void OnCombMethodPlanChanged(int index);
	void OnBtnNewMethodClicked();
	void OnBtnSaveMethodClicked();
	void OnBtnDeleteMethodClicked();
	void OnBtnModifyMethodClicked();

	/*
	* �ʺ���Ϣ����
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

	// �û��˺����
	QStringListModel	*m_pAccountListModel;
	STRUCT_Account		m_account;
	UserAccount				*m_pAccountDB;
	UIState						m_accountEditState;

	// ���Է������
	QStringListModel *m_pMethodListModel;
	MethodParam	*m_pMethodParam;
	UIState	m_methodEditState;
	int	m_methodListIndex;
};

#endif // WIDGET_H
