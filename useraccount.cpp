/*
* �������ڣ�2016-09-12
* ����޸ģ�2016-09-12
* ��      �ߣ�syf
* ��      ����
*/
#include "useraccount.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
UserAccount::UserAccount(QObject *parent)
	: QObject(parent)
{
	m_accountList.clear();
	m_messageList.clear();
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
UserAccount::~UserAccount()
{

}


/*
* ������userType--�û�����
* ���أ�
* ���ܣ������û����ͻ�ȡ���ݿ��е��˻��б�
*/
QStringList& UserAccount::GetAccontList(ENUM_AccountType userType)
{
	m_messageList.clear();
	m_accountList.clear();


	return m_accountList;
}


/*
* ������
* ���أ�
* ���ܣ�
*/
