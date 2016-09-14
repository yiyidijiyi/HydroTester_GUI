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
* 参数：methodList--返回，测试方法列表
* 返回：
* 功能：查询测试方法参数数据库中所有的测试方法
*/
bool MethodParam::GetMethodList(QStringList &methodList)
{
	bool state = false;
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM parameters";
		QSqlQuery query(strQuery, db);
		m_idMap.clear();
		methodList.clear();

		if (query.exec())
		{
			while (query.next())
			{
				m_idMap.append(query.value(0).toInt());
				methodList.append(query.value(1).toString());
			}

			state = true;
			m_messageList.append(QStringLiteral("查询测试方法列表成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("查询测试方法列表出错！"));
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
* 参数：index--测试方法列表索引
			method--输出，测试方法参数
* 返回：
* 功能：根据数据库中parameters表的id，查询测试方法参数
*/
bool MethodParam::GetMethodInfo(int index, STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM parameters WHERE id = ?";
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
bool MethodParam::AddMethod(const STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "INSERT INTO   parameters  (name, plan, standard, rate, timing, pressure, cycle, holdingTime, uinit, description) \
											VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, method.name);
		query.bindValue(1, method.plan);
		query.bindValue(2, method.standard);
		query.bindValue(3, method.rate);
		query.bindValue(4, method.timing);
		query.bindValue(5, method.pressure);
		query.bindValue(6, method.cycle);
		query.bindValue(7, method.holdingTime);
		query.bindValue(8, method.uint);
		query.bindValue(9, method.discription);


		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("增加测试方法成功！"));
		}
		else
		{
			m_messageList.append(QStringLiteral("增加测试方法出错！"));
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
* 参数：index--测试方法列表索引
* 返回：
* 功能：根据测试列表索引删除一条测试方法
*/
bool MethodParam::DeleteMethod(int index)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "DELETE  FROM parameters WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("删除一条测试方法成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("删除测试方法时出错！"));
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
* 参数：index--测试方法列表索引
* 返回：
* 功能：根据测试列表索引修改一条测试方法的参数
*/
bool MethodParam::UpdateMethodParam(int index, const STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "UPDATE parameters SET name = ?, plan = ?, standard = ?, rate = ?, timing = ?, pressure= ?, \
						   cycle = ?, holdingTime = ?, uinit = ?, description = ?  WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, method.name);
		query.bindValue(1, method.plan);
		query.bindValue(2, method.standard);
		query.bindValue(3, method.rate);
		query.bindValue(4, method.timing);
		query.bindValue(5, method.pressure);
		query.bindValue(6, method.cycle);
		query.bindValue(7, method.holdingTime);
		query.bindValue(8, method.uint);
		query.bindValue(9, method.discription);
		query.bindValue(10, id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("更新一条测试方法成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("更新测试方法时出错！"));
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
