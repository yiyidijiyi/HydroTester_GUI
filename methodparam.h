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
#include <QtCore/QVariant>

#include "common.h"

class MethodParam : public QObject
{
	Q_OBJECT

public:
	MethodParam(QObject *parent = 0);
	~MethodParam();

	bool GetMethodInfo(int id, STRUCT_MethodParam &method);
private:
	QStringList m_messageList;
};

#endif // METHODPARAM_H
