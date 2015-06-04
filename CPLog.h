#ifndef CROSS_PLATFORM_LOG_H_H_H
#define CROSS_PLATFORM_LOG_H_H_H

#define CPLOG_TXT_FILE_NAME "log.log"

#ifdef _DEBUG
#define USE_CPLOG
#else

#endif

#include <stdio.h>
#include "CPLog_Details.h"

#define _CONNECT_STRING(str1,str2) str1##str2
#define CONNECT_STRING(str1,str2) _CONNECT_STRING(str1,str2)

#define LINE_NUM_STRING __LINE__

#define LINE_NUM_VARIABLE_NAME(name) CONNECT_STRING(name,CONNECT_STRING(_Line_,LINE_NUM_STRING))
#define LINE_NUM_VARIABLE_NAME_DEFAULT LINE_NUM_VARIABLE_NAME(default_name_)



#ifdef USE_CPLOG

#define CPLOG_FUNCTION_ENTER(description) \
	CPLog::CPLog_Block_Enter LINE_NUM_VARIABLE_NAME(__functionEnter_)(CPLOG_FUNCTION_NAME,description)

#define CPLOG_BLOCK_ENTER(block_name,description) \
	CPLog::CPLog_Block_Enter LINE_NUM_VARIABLE_NAME(__blockEnter_)(block_name,description)



#define CPLOG_MESSAGE(messageInfo) \
	CPLog::CPLog_Message LINE_NUM_VARIABLE_NAME(_message_)(messageInfo)



#define CPFE(arg) CPLOG_FUNCTION_ENTER(arg)
#define CPBE(arg1,arg2) CPLOG_BLOCK_ENTER(arg1,arg2)
#define CPME(arg) CPLOG_MESSAGE(arg)



#define CPLOG_CODE(code) code



#else
#define CPLOG_FUNCTION_ENTER(description)
#define CPLOG_BLOCK_ENTER(block_name,description)
#define CPLOG_MESSAGE(messageInfo)

#define CPFE(arg)
#define CPBE(arg1,arg2)
#define CPME(arg)

#define CPLOG_CODE(code)

#endif

#define CPLOG_FORMAT_STRING(...) \
	CPLog::printToBuff(__VA_ARGS__)

#define cpfs CPLOG_FORMAT_STRING

#endif
