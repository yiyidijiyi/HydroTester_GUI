/*
* 创建日期：2016-09-12
* 最后修改：2016-09-12
* 作      者：syf
* 描      述：
*/
#include "useraccount.h"


/*
* 参数：
* 返回：
* 功能：构造函数
*/
UserAccount::UserAccount(QObject *parent)
	: QObject(parent)
{
	m_accountList.clear();
	m_messageList.clear();
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
UserAccount::~UserAccount()
{

}


/*
* 参数：userType--用户类型
* 返回：
* 功能：根据用户类型获取数据库中的账户列表
*/
QStringList& UserAccount::GetAccontList(ENUM_AccountType userType)
{
	m_messageList.clear();
	m_accountList.clear();


	return m_accountList;
}


/*
* 参数：
* 返回：
* 功能：
*/
