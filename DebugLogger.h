#pragma once

typedef enum
{
	LOG_INFO,
	LOG_ERROR
} MsgLevel;

//¼ÇÂ¼Ò»ÌõLog
void LogMessage(const char* Msg, MsgLevel Level = LOG_INFO);