#include "widget.h"
#include "ui_widget.h"


/*
* 参数：
* 返回：
* 功能：构造函数
*/
Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	DesignUi();
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
Widget::~Widget()
{

}



/*
* 参数：
* 返回：
* 功能：设计显示界面
*/
void Widget::DesignUi()
{
	ui->setupUi(this);

	// 固定登录窗口大小
	setFixedSize(1366, 728);

	// 无边框显示
	setWindowFlags(Qt::FramelessWindowHint);

	// 设置最小、最大、关闭按钮样式
	ui->pushButton_min->setStyleSheet("QPushButton {border_image:url(:/main/resource/main/min.png);}");
	ui->pushButton_max->setStyleSheet("QPushButton {border_image:url(:/main/resource/main/max.png);}");
	ui->pushButton_close->setStyleSheet("QPushButton {border_image:url(:/main/resource/main/close.png);}");
}


/*
* 参数：
* 返回：
* 功能：
*/