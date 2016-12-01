/*
* �������ڣ�2016-11-30
* ����޸ģ�2016-11-30
* ��      �ߣ�syf
* ��      ����
*/
#include "reportwindow.h"

/*
* ������
* ���أ�
* ���ܣ����캯��
*/
ReportWindow::ReportWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton_print, &QPushButton::clicked, this, &ReportWindow::OnBtnPrintClicked);
	connect(ui.pushButton_save, &QPushButton::clicked, this, &ReportWindow::OnBtnSaveClicked);
}

/*
* ������
* ���أ�
* ���ܣ���������
*/
ReportWindow::~ReportWindow()
{

}

/*
* ������report-���鱨����ص�����
* ���أ�
* ���ܣ��������鱨��
*/
void ReportWindow::GenReport(const STRUCT_Report &report)
{
	m_fileName = report.fileName;
	QString htmlStr = QStringLiteral("<p align = \"center\"><font size = \"6\">��ˮѹ�������ϵͳ</font></p>");
	htmlStr += QStringLiteral("<p align = \"center\"><font size = \"6\">���Ա���</font></p> <p> </p> <p> </p>");

	htmlStr += QStringLiteral("<p><font size = \"4\"><b>���Է��������:</b></font></p>");
	htmlStr += QStringLiteral("<p>�������ƣ�%1</p>").arg(report.methodName);
	htmlStr += QStringLiteral("<p>Ӧ�ñ�׼��%1</p>").arg(report.standard);

	QString unit;
	switch (report.unit)
	{
	case 0:
		unit = QStringLiteral("Pa(��)");
		break;
	case 1:
		unit = QStringLiteral("kPa(ǧ��)");
		break;
	case 2:
		unit = QStringLiteral("mBar(����)");
		break;
		unit = QStringLiteral("mmH2O(����ˮ��)");
	case 3:
		break;
	default:
		break;
	}

	switch (report.methodPlan)
	{
	case 0:
		htmlStr += QStringLiteral("<p>���Է�����������ѹ</p>");
		htmlStr += QStringLiteral("<p>������ѹ���ʣ�%1/����</p>").arg(QString::number(report.rate) + unit);
		break;
	case 1:
		htmlStr += QStringLiteral("<p>���Է�������ʱ��ѹ</p>");
		htmlStr += QStringLiteral("<p>��ʱ��ѹ���ʣ�%1/����</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>��ʱ��ѹ��ʱʱ�䣺%1����</p>").arg(QString::number(report.timing));
		break;
	case 2:
		htmlStr += QStringLiteral("<p>���Է�������ʱ��ѹ</p>");
		htmlStr += QStringLiteral("<p>��ʱ��ѹ���ʣ�%1/����</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>��ʱ��ѹĿ��ѹǿ��%1</p>").arg(QString::number(report.pressure) + unit);
		htmlStr += QStringLiteral("<p>��ʱ��ѹ��ʱʱ�䣺%1����</p>").arg(QString::number(report.timing));
		break;
	case 3:
		htmlStr += QStringLiteral("<p>���Է����������ɳ�</p>");
		htmlStr += QStringLiteral("<p>�����ɳ�ˮ���ʣ�%1/����</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>�����ɳ�Ŀ��ѹǿ��%1</p>").arg(QString::number(report.pressure) + unit);
		htmlStr += QStringLiteral("<p>�����ɳ��������ڣ�%1��</p>").arg(QString::number(report.cycle));
		htmlStr += QStringLiteral("<p>�����ɳ�ˮѹ����ʱ�䣺%1����</p>").arg(QString::number(report.holdingTime));
		break;
	case 4:
		htmlStr += QStringLiteral("<p>���Է�������ˮ©ˮ</p>");
		htmlStr += QStringLiteral("<p>��ˮ©ˮ���ʣ�%1/����</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>��ˮ©ˮĿ��ѹǿ��%1</p>").arg(QString::number(report.pressure) + unit);
		htmlStr += QStringLiteral("<p>��ˮ©ˮ��ʱʱ�䣺%1����</p>").arg(QString::number(report.timing));
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p>������%1</p>").arg(report.description);
	htmlStr += QStringLiteral("<p>   </p><p>    </p><p><b>��������ʱ�䣺</b>%1</p>").arg(report.testDate);


	QString strEndType;

	switch (report.endMode)
	{
	case EndAuto:
		strEndType = QStringLiteral("��⵽ˮ���Զ�����");
		break;
	case EndByDevice:
		strEndType = QStringLiteral("���豸���ֶ�����");
		break;
	case EndBySoftware:
		strEndType = QStringLiteral("��������ֶ�����");
		break;
	case EndBurst:
		strEndType = QStringLiteral("��ƥ���ƽ���");
		break;
	case EndTimeOut:
		strEndType = QStringLiteral("��ʱʱ�䵽����");
		break;
	case EndPressureOverRange:
		strEndType = QStringLiteral("ѹ�������̽���");
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p><b>���Խ�����ʽ��</b>%1</p>").arg(strEndType);


	htmlStr += QStringLiteral("<p><b>��ƥ�ϵ�ˮ���������</b></p>");

	if (report.decTime.length() > 1)
	{
		QStringList listAppearTime = report.decTime.split(",");
		QStringList listAppearPressure = report.decPressure.split(",");
		int n = listAppearTime.size();

		for (int i = 0; i < n; i++)
		{
			htmlStr += QStringLiteral("<p>��⵽��%1��ˮ���ʱ��Ϊ��%2</p>").arg(i + 1).arg(listAppearTime[i]);
			htmlStr += QStringLiteral("<p>��⵽��%1��ˮ���ѹ��ֵΪ��%2</p>").arg(i + 1).arg(listAppearPressure[i]);
		}
	}
	else
	{
		htmlStr += QStringLiteral("<p>δ��⵽�����ϳ���ˮ�飡</p>");
	}

	// ��ȡ�����ʵ����ͼƬ
	QString imgPath;
	if (EndAuto == report.endMode)
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/report") + report.fileName);
	}
	else
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/last") + report.fileName);
	}

	htmlStr += QStringLiteral("<p> <br></br> </p> <p align = \"center\"><b>�������ͼ</b></p>");
	htmlStr += imgPath;
	htmlStr += QStringLiteral("<p> <br></br><br></br> </p> <p align = \"center\"><b>ѹ������</b></p>");
	htmlStr += QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/curve") + report.fileName);

	ui.textEdit->insertHtml(htmlStr);
}

/*
* ������
* ���أ�
* ���ܣ���ӡ���Ա���
*/
void ReportWindow::PrintReport()
{
	QPrinter printer;

	QPrintDialog printDialog(&printer, this);

	if (printDialog.exec())
	{
		QTextDocument textDoc;
		textDoc.setHtml(ui.textEdit->toHtml());
		textDoc.print(&printer);
	}
}

/*
* ������
* ���أ�
* ���ܣ��������鱨��Ϊpdf
*/
void ReportWindow::SaveReport()
{
	QPrinter printer;
	QString path1 = QFileDialog::getSaveFileName(this, QStringLiteral("��������"),
		"d:/" + m_fileName + ".pdf",
		"*.pdf");
	//QTextCodec* pCodec = QTextCodec::codecForName("gb18030");
	//QString path = pCodec->fromUnicode(path1).data();
	//path += "/" + m_fileName + ".pdf";
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(path1);
	QTextDocument textDoc;
	textDoc.setHtml(ui.textEdit->toHtml());
	textDoc.print(&printer);
	//textDoc.end();
}

/*
* ������
* ���أ�
* ���ܣ�
*/
void ReportWindow::OnBtnPrintClicked()
{
	PrintReport();
}

/*
* ������
* ���أ�
* ���ܣ�
*/
void ReportWindow::OnBtnSaveClicked()
{
	SaveReport();
}

/*
* ������
* ���أ�
* ���ܣ�
*/
