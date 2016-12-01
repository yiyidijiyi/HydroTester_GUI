/*
* 创建日期：2016-11-30
* 最后修改：2016-11-30
* 作      者：syf
* 描      述：
*/
#include "reportwindow.h"

/*
* 参数：
* 返回：
* 功能：构造函数
*/
ReportWindow::ReportWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton_print, &QPushButton::clicked, this, &ReportWindow::OnBtnPrintClicked);
	connect(ui.pushButton_save, &QPushButton::clicked, this, &ReportWindow::OnBtnSaveClicked);
}

/*
* 参数：
* 返回：
* 功能：析构函数
*/
ReportWindow::~ReportWindow()
{

}

/*
* 参数：report-试验报告相关的数据
* 返回：
* 功能：生成试验报告
*/
void ReportWindow::GenReport(const STRUCT_Report &report)
{
	m_fileName = report.fileName;
	QString htmlStr = QStringLiteral("<p align = \"center\"><font size = \"6\">耐水压成像测试系统</font></p>");
	htmlStr += QStringLiteral("<p align = \"center\"><font size = \"6\">测试报告</font></p> <p> </p> <p> </p>");

	htmlStr += QStringLiteral("<p><font size = \"4\"><b>测试方法与参数:</b></font></p>");
	htmlStr += QStringLiteral("<p>方法名称：%1</p>").arg(report.methodName);
	htmlStr += QStringLiteral("<p>应用标准：%1</p>").arg(report.standard);

	QString unit;
	switch (report.unit)
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

	switch (report.methodPlan)
	{
	case 0:
		htmlStr += QStringLiteral("<p>测试方法：持续增压</p>");
		htmlStr += QStringLiteral("<p>持续增压速率：%1/分钟</p>").arg(QString::number(report.rate) + unit);
		break;
	case 1:
		htmlStr += QStringLiteral("<p>测试方法：定时计压</p>");
		htmlStr += QStringLiteral("<p>定时计压速率：%1/分钟</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>定时计压定时时间：%1分钟</p>").arg(QString::number(report.timing));
		break;
	case 2:
		htmlStr += QStringLiteral("<p>测试方法：定时定压</p>");
		htmlStr += QStringLiteral("<p>定时定压速率：%1/分钟</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>定时定压目标压强：%1</p>").arg(QString::number(report.pressure) + unit);
		htmlStr += QStringLiteral("<p>定时定压定时时间：%1分钟</p>").arg(QString::number(report.timing));
		break;
	case 3:
		htmlStr += QStringLiteral("<p>测试方法：绕曲松驰</p>");
		htmlStr += QStringLiteral("<p>绕曲松驰水速率：%1/分钟</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>绕曲松驰目标压强：%1</p>").arg(QString::number(report.pressure) + unit);
		htmlStr += QStringLiteral("<p>绕曲松驰松绕周期：%1次</p>").arg(QString::number(report.cycle));
		htmlStr += QStringLiteral("<p>绕曲松驰水压保持时间：%1分钟</p>").arg(QString::number(report.holdingTime));
		break;
	case 4:
		htmlStr += QStringLiteral("<p>测试方法：渗水漏水</p>");
		htmlStr += QStringLiteral("<p>渗水漏水速率：%1/分钟</p>").arg(QString::number(report.rate) + unit);
		htmlStr += QStringLiteral("<p>渗水漏水目标压强：%1</p>").arg(QString::number(report.pressure) + unit);
		htmlStr += QStringLiteral("<p>渗水漏水定时时间：%1分钟</p>").arg(QString::number(report.timing));
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p>描述：%1</p>").arg(report.description);
	htmlStr += QStringLiteral("<p>   </p><p>    </p><p><b>报告生成时间：</b>%1</p>").arg(report.testDate);


	QString strEndType;

	switch (report.endMode)
	{
	case EndAuto:
		strEndType = QStringLiteral("检测到水珠自动结束");
		break;
	case EndByDevice:
		strEndType = QStringLiteral("在设备上手动结束");
		break;
	case EndBySoftware:
		strEndType = QStringLiteral("在软件上手动结束");
		break;
	case EndBurst:
		strEndType = QStringLiteral("布匹涨破结束");
		break;
	case EndTimeOut:
		strEndType = QStringLiteral("定时时间到结束");
		break;
	case EndPressureOverRange:
		strEndType = QStringLiteral("压力超量程结束");
		break;
	default:
		break;
	}
	htmlStr += QStringLiteral("<p><b>测试结束方式：</b>%1</p>").arg(strEndType);


	htmlStr += QStringLiteral("<p><b>布匹上的水珠检测情况：</b></p>");

	if (report.decTime.length() > 1)
	{
		QStringList listAppearTime = report.decTime.split(",");
		QStringList listAppearPressure = report.decPressure.split(",");
		int n = listAppearTime.size();

		for (int i = 0; i < n; i++)
		{
			htmlStr += QStringLiteral("<p>检测到第%1滴水珠的时间为：%2</p>").arg(i + 1).arg(listAppearTime[i]);
			htmlStr += QStringLiteral("<p>检测到第%1滴水珠的压力值为：%2</p>").arg(i + 1).arg(listAppearPressure[i]);
		}
	}
	else
	{
		htmlStr += QStringLiteral("<p>未检测到布面上出现水珠！</p>");
	}

	// 获取保存的实验结果图片
	QString imgPath;
	if (EndAuto == report.endMode)
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/report") + report.fileName);
	}
	else
	{
		imgPath = QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/last") + report.fileName);
	}

	htmlStr += QStringLiteral("<p> <br></br> </p> <p align = \"center\"><b>检测结果截图</b></p>");
	htmlStr += imgPath;
	htmlStr += QStringLiteral("<p> <br></br><br></br> </p> <p align = \"center\"><b>压力曲线</b></p>");
	htmlStr += QString("<p align = \"center\"><img src=\"%1\"/></p>").arg(QString("./report/curve") + report.fileName);

	ui.textEdit->insertHtml(htmlStr);
}

/*
* 参数：
* 返回：
* 功能：打印测试报告
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
* 参数：
* 返回：
* 功能：保存试验报告为pdf
*/
void ReportWindow::SaveReport()
{
	QPrinter printer;
	QString path1 = QFileDialog::getSaveFileName(this, QStringLiteral("导出报告"),
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
* 参数：
* 返回：
* 功能：
*/
void ReportWindow::OnBtnPrintClicked()
{
	PrintReport();
}

/*
* 参数：
* 返回：
* 功能：
*/
void ReportWindow::OnBtnSaveClicked()
{
	SaveReport();
}

/*
* 参数：
* 返回：
* 功能：
*/
