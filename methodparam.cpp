/*
* 创建日期：2016-09-14
* 最后修改：2016-09-14
* 作      者：syf
* 描      述：
*/
#include "methodparam.h"



/*
* 参数：
* 返回：
* 功能：构造函数
*/
MethodParam::MethodParam(QObject *parent)
	: QObject(parent)
{

}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
MethodParam::~MethodParam()
{

}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
bool MethodParam::GetMethodInfo(int id, STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM account WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			while (query.next())
			{
				method.id					= query.value(0).toInt();
				method.name			= query.value(1).toString();
				method.plan				= query.value(2).toInt();
				method.standard		= query.value(3).toString();
				method.rate				= query.value(4).toDouble();
				method.timing			= query.value(5).toDouble();
				method.pressure		= query.value(6).toDouble();
				method.cycle				= query.value(7).toInt();
				method.holdingTime	= query.value(8).toDouble();
				method.uint				= query.value(9).toInt();
				method.discription		= query.value(10).toString();
				method.lastTime		= query.value(11).toString();
			}

			state = true;
			m_messageList.append(QStringLiteral("查询测试方法参数成功！"));
			
		}
		else
		{
			m_messageList.append(QStringLiteral("查询测试方法参数出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接测试方法数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：
* 返回：
* 功能：
*/
