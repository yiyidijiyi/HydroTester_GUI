/*
* 创建日期：2016-09-02
* 最后修改：2016-09-14
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

#include "common.h"
#include "useraccount.h"
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
	void SwitchInterface(InterfaceIndex index);
	void SwitchChart(ChartIndex index);
	Widget::InterfaceIndex GetInterfaceIndex(void);

private:
	void UpdateAccountList();
	

	// 更新UI状态
	void UpdateAcountInfoUI(UIState state);
	void UpdateMethodInfoUI(UIState state);
	void UpdateEditableMethodInfoUI(int index);

protected slots:
	void OnBtnMinClicked();
	void OnBtnCloseClicked();
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

public slots:
	void OnBtnTestInterfaceClicked();
	void OnBtnTestMethodClicked();
	void OnBtnReportQueryClicked();
	void OnBtnAdvanceClicked();
	void OnBtnHelpClicked();

	void OnBtnChartVideoClicked();
	void OnBtnChartCurveClicked();
	void OnBtnChartReportClicked();
	void OnBtnChartPrintClicked();
	
	void OnLoginAccepted(int id);

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

	// 用户账号相关
	QStringListModel	*m_pAccountListModel;
	STRUCT_Account	m_account;
	UserAccount			*m_pAccountDB;
	UIState					m_accountEditState;
};

#endif // WIDGET_H
