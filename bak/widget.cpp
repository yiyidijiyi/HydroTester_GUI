#include "widget.h"
#include "ui_widget.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	DesignUi();
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
Widget::~Widget()
{

}



/*
* ������
* ���أ�
* ���ܣ������ʾ����
*/
void Widget::DesignUi()
{
	ui->setupUi(this);

	// �̶���¼���ڴ�С
	setFixedSize(1366, 728);

	// �ޱ߿���ʾ
	setWindowFlags(Qt::FramelessWindowHint);

	// ������С����󡢹رհ�ť��ʽ
	ui->pushButton_min->setStyleSheet("QPushButton {border_image:url(:/main/resource/main/min.png);}");
	ui->pushButton_max->setStyleSheet("QPushButton {border_image:url(:/main/resource/main/max.png);}");
	ui->pushButton_close->setStyleSheet("QPushButton {border_image:url(:/main/resource/main/close.png);}");
}


/*
* ������
* ���أ�
* ���ܣ�
*/