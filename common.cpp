/*
* 创建日期：2016-09-20
* 最后修改：2016-09-20
* 作      者：syf
* 描      述：
*/

#include "common.h"


/*
* 参数：ch：字符
* 返回：
* 功能：将字符转换为16进制形式
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
* 参数：hexData：16进制的组数
			str：输出，转换后的字符串
* 返回：
* 功能：将16进制数组转换成对应的字符串
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