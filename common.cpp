/*
* �������ڣ�2016-09-20
* ����޸ģ�2016-09-20
* ��      �ߣ�syf
* ��      ����
*/

#include "common.h"


/*
* ������ch���ַ�
* ���أ�
* ���ܣ����ַ�ת��Ϊ16������ʽ
*/
char ConvertCharHex(char ch)
{
	if ((ch >= 0) && (ch <= 9))
	{
		return ch + '0';//0x30;
	}
	else if ((ch >= 10) && (ch <= 15))
	{
		return ch - 10 + 'A';
	}
	else
	{
		return -1;
	}
}

/*
* ������hexData��16���Ƶ�����
			str�������ת������ַ���
* ���أ�
* ���ܣ���16��������ת���ɶ�Ӧ���ַ���
*/
void HexToString(const QByteArray &hexData, QString &str)
{
	int arrLen = hexData.length();
	char l, h;

	for (int i = 0; i < arrLen; i++)
	{
		l = ConvertCharHex(static_cast<quint8>(hexData[i]) % 16);
		h = ConvertCharHex(static_cast<quint8>(hexData[i]) / 16);
		str.append(h);
		str.append(l);
		str.append(' ');
	}
}