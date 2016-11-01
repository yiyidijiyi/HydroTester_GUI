/*
* 创建日期：2016-09-02
* 最后修改：2016-09-07
* 作      者：syf
* 描      述：
*/
#include "widget.h"
#include "logindlg.h"
#include <QFile>
#include <QApplication>

static void LoadQssFile();

/*
* 参数：
* 返回：
* 功能：主函数
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//LoadQssFile();

    Widget w;
	w.setWindowTitle(QStringLiteral("主窗口"));
    LoginDlg dlg;

	qDebug() << "main:" << QThread::currentThreadId() << endl;

	QObject::connect(&dlg, &LoginDlg::AccountID, &w, &Widget::OnLoginAccepted);

    if(dlg.exec() == QDialog::Accepted)
    {
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}


/*
* 参数：
* 返回：
* 功能：加载Qss文件
*/
static void LoadQssFile()
{
	QFile qssFile("./qss/style.qss");
	if (qssFile.open(QFile::ReadOnly))
	{
		qApp->setStyleSheet(qssFile.readAll());
		qssFile.close();
	}
}
