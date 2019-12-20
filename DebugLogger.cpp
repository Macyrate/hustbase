//使用不安全的文件操作函数
#define _CRT_SECURE_NO_WARNINGS

#include "StdAfx.h"
#include "DebugLogger.h"
#include <stdio.h>

void LogMessage(const char* Msg, MsgLevel Level)
{
#ifdef DEBUG

	//使用纯C

	FILE* fp = NULL;

	if (fp = fopen("Debug.log", "a"))
	{

		//日志文件打开成功
		//写入记录日志时的时间

		time_t timeNow = time(0);
		char strTimeNow[32];
		strftime(strTimeNow, sizeof(strTimeNow), "%Y/%m/%d %H:%M:%S ", localtime(&timeNow));
		fprintf(fp, strTimeNow);

		//写入日志级别

		switch (Level)
		{

		case LOG_INFO:
			fprintf(fp, "[INFO]  ");
			break;

		case LOG_ERROR:
			fprintf(fp, "[ERROR] ");
			break;

		}

		//写入消息

		fprintf(fp, Msg);
		fprintf(fp, "\n");

		//关闭文件

		fclose(fp);

	}

#endif
}
