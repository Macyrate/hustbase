#pragma once

typedef enum
{
	LOG_INFO,
	LOG_ERROR
} MsgLevel;

//��¼һ��Log
void LogMessage(const char* Msg, MsgLevel Level = LOG_INFO);