/*
* �������ڣ�2016-09-02
* ����޸ģ�2016-09-07
* ��      �ߣ�syf
* ��      ����
*/
#include "widget.h"
#include "logindlg.h"
#include <QFile>
#include <QApplication>

static void LoadQssFile();

/*
* ������
* ���أ�
* ���ܣ�������
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//LoadQssFile();

    Widget w;
	w.setWindowTitle(QStringLiteral("������"));
    LoginDlg dlg;

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
* ������
* ���أ�
* ���ܣ�����Qss�ļ�
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
