/*
* �������ڣ�2016-09-14
* ����޸ģ�2016-09-14
* ��      �ߣ�syf
* ��      ����
*/
#ifndef METHODPARAM_H
#define METHODPARAM_H

#include <QObject>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVariant>
#include <QVector>

#include "common.h"

class MethodParam : public QObject
{
	Q_OBJECT

public:
	MethodParam(QObject *parent = 0);
	~MethodParam();

	bool GetMethodList(QStringList &methodList);
	bool GetMethodInfo(int index, STRUCT_MethodParam &method);
	bool AddMethod(const STRUCT_MethodParam &method);
	bool DeleteMethod(int index);
	bool UpdateMethodParam(int index, const STRUCT_MethodParam &method);
private:
	QStringList		m_messageList;
	QVector<int>	m_idMap;
};

#endif // METHODPARAM_H
