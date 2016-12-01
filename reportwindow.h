/*
* �������ڣ�2016-11-30
* ����޸ģ�2016-11-30
* ��      �ߣ�syf
* ��      ����
*/
#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include <QDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QFileDialog>
#include <QTextCodec>
#include "ui_reportwindow.h"
#include "common.h"

class ReportWindow : public QDialog
{
	Q_OBJECT

public:
	ReportWindow(QWidget *parent = 0);
	~ReportWindow();

	void GenReport(const STRUCT_Report &report);
	void PrintReport();
	void SaveReport();

public slots:
	void OnBtnPrintClicked();
	void OnBtnSaveClicked();
private:
	Ui::ReportWindow ui;
	QString m_fileName;
};

#endif // REPORTWINDOW_H
