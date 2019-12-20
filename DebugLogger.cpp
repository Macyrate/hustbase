//ʹ�ò���ȫ���ļ���������
#define _CRT_SECURE_NO_WARNINGS

#include "StdAfx.h"
#include "DebugLogger.h"
#include <stdio.h>

void LogMessage(const char* Msg, MsgLevel Level)
{
#ifdef DEBUG

	//ʹ�ô�C

	FILE* fp = NULL;

	if (fp = fopen("Debug.log", "a"))
	{

		//��־�ļ��򿪳ɹ�
		//д���¼��־ʱ��ʱ��

		time_t timeNow = time(0);
		char strTimeNow[32];
		strftime(strTimeNow, sizeof(strTimeNow), "%Y/%m/%d %H:%M:%S ", localtime(&timeNow));
		fprintf(fp, strTimeNow);

		//д����־����

		switch (Level)
		{

		case LOG_INFO:
			fprintf(fp, "[INFO]  ");
			break;

		case LOG_ERROR:
			fprintf(fp, "[ERROR] ");
			break;

		}

		//д����Ϣ

		fprintf(fp, Msg);
		fprintf(fp, "\n");

		//�ر��ļ�

		fclose(fp);

	}

#endif
}
