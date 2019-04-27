#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <string.h>

#define _DEBUG_LEVEL_NONE_	-1

#define _DEBUG_LEVEL_V_		0
#define _DEBUG_LEVEL_D_		1
#define _DEBUG_LEVEL_I_		2
#define _DEBUG_LEVEL_W_		3
#define _DEBUG_LEVEL_E_		4


#ifndef _DEBUG_LEVEL_
#   define _DEBUG_LEVEL_  _DEBUG_LEVEL_V_
#endif

//#define _SHORT_FILE_NAME_ ((strrchr(__FILE__, '/') == NULL)? __FILE__: (strrchr(__FILE__, '/') + 1))
#define _SHORT_FILE_NAME_ ((strrchr(__FILE__, '\\') == NULL)? __FILE__: (strrchr(__FILE__, '\\') + 1))

#define DBG_PosInfo() fprintf(stdout, "\n==>[%s %d %s()]", _SHORT_FILE_NAME_, __LINE__, __FUNCTION__)
#define DBG_Log_Print(fmt,args...) fprintf(stdout,fmt,##args)

#define LOG_A(fmt,args...) do{DBG_PosInfo();DBG_Log_Print(" ");DBG_Log_Print(fmt,##args);}while(0)

#if _DEBUG_LEVEL_ <= _DEBUG_LEVEL_V_ && !_RELEASE_VERSION_
#   define LOG_V(fmt,args...) do{DBG_PosInfo();DBG_Log_Print(" V:");DBG_Log_Print(fmt, ##args);}while(0)
#else
#   define LOG_V(fmt,args...)
#endif

#if _DEBUG_LEVEL_ <= _DEBUG_LEVEL_D_
#   define LOG_D(fmt,args...) do{DBG_PosInfo();DBG_Log_Print(" D:");DBG_Log_Print(fmt, ##args);}while(0)
#else
#   define LOG_D(fmt,args...)
#endif

#if _DEBUG_LEVEL_ <= _DEBUG_LEVEL_I_
#   define LOG_I(fmt,args...) do{DBG_PosInfo();DBG_Log_Print(" I:");DBG_Log_Print(fmt,##args);}while(0)
#else
#   define LOG_I(fmt,args...)
#endif

#if _DEBUG_LEVEL_ <= _DEBUG_LEVEL_W_
#   define LOG_W(fmt,args...) do{DBG_PosInfo();DBG_Log_Print(" W:");DBG_Log_Print(fmt,##args);}while(0)
#else
#   define LOG_W(fmt,args...)
#endif

#if _DEBUG_LEVEL_ <= _DEBUG_LEVEL_E_
#   define LOG_E(fmt,args...) do{DBG_PosInfo();DBG_Log_Print(" E:");DBG_Log_Print(fmt,##args);}while(0)
#else
#   define LOG_E(fmt,args...)
#endif


#if _DEBUG_LEVEL_ <=  _DEBUG_LEVEL_V_ && !_RELEASE_VERSION_
    #define FUNC_ENTRY() do{DBG_Log_Print("\n==>[%s %d] Enter %s().\n", _SHORT_FILE_NAME_, __LINE__, __FUNCTION__);}while(0)
#else
    #define FUNC_ENTRY()
#endif

#if _DEBUG_LEVEL_ <=  _DEBUG_LEVEL_V_ && !_RELEASE_VERSION_
    #define FUNC_EXIT() do{DBG_Log_Print("\n==>[%s %d] Exit %s().\n", _SHORT_FILE_NAME_, __LINE__, __FUNCTION__);}while(0)
#else
    #define FUNC_EXIT()
#endif

/*****************************************************************************
  *                          Debug Paramter Check
  *************************************************************************** */
#if _DEBUG_LEVEL_ <=  _DEBUG_LEVEL_V_ && !_RELEASE_VERSION_
    #define ASSERT(arg)  do { if (!(arg)){LOG_E("Assert failed:%s.\n",#arg); abort();}}while(0)

    #define CHECK_PARAM(ParamCorrectFormat, errorCode) ASSERT(ParamCorrectFormat)

    #define CHECK_RESULT(ParamCorrectFormat, errorCode, successCode) ASSERT(ParamCorrectFormat)
#else
    #define ASSERT(arg)

    #define CHECK_PARAM(ParamCorrectFormat, errorCode) \
            do{\
                if (!(ParamCorrectFormat))\
                {\
                    LOG_E("Param check: \"%s\" failed.\n",#ParamCorrectFormat);\
                    return errorCode;\
                }\
              }while(0)

    #define CHECK_RESULT(ParamCorrectFormat, errorCode, successCode)\
            do{\
                if (!(ParamCorrectFormat))\
                {\
                    LOG_E("Result check:\"%s\" failed.\n",#ParamCorrectFormat);\
                    return errorCode;\
                }\
                else\
                {\
                    return successCode;\
                }\
              }while(0)
#endif

/*****************************************************************************
 *                           Debug Print Buffer
 *************************************************************************** */

#if defined(_DEBUG_BUFFER_ON_) && defined(_DEBUG_ALL_PRINT_BUFFER_ON_)
#define DBG_PrintU8Buffer(bufferName, p_buffer, bufferLen) do {DBG_PosInfo();\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) begin===========================", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
		if( (p_buffer) != NULL )\
		{\
		    typedef unsigned char _U8_;\
		    _U8_ *p_u8Buffer = (_U8_ *)(p_buffer);\
		    int __i = 0;\
		    for( __i = 0; __i < (int)(bufferLen); __i++ )\
		    {\
		         if( (__i % 16) == 0 )  fprintf(stdout, "\n");\
		         DBG_Log_Print("0x%02X ", (p_u8Buffer)[__i]);\
		    }\
		}\
		else\
		{\
		    DBG_Log_Print("p_buffer = NULL\n" ); \
		}\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) end===========================\n\n", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
	}while(0);
#else
#define DBG_PrintU8Buffer(bufferName, p_buffer, bufferLen)
#endif

#if defined(_DEBUG_BUFFER_ON_) && defined(_DEBUG_ALL_PRINT_BUFFER_ON_)
#define DBG_PrintU16Buffer(bufferName, p_buffer, bufferLen) do {DBG_PosInfo();\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) begin===========================", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
		if( (p_buffer) != NULL )\
		{\
		    int __i = 0;\
		    for( __i = 0; __i < (int)(bufferLen); __i++ )\
		    {\
		         if( (__i % 16) == 0 )  fprintf(stdout, "\n");\
		         DBG_Log_Print("%04X ", (p_buffer)[__i]);\
		    }\
		}\
		else\
		{\
		    DBG_Log_Print("p_buffer = NULL\n" ); \
		}\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) end===========================\n\n", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
	}while(0);
#else
#define DBG_PrintU16Buffer(bufferName, p_buffer, bufferLen)
#endif


#if defined(_DEBUG_BUFFER_ON_) && defined(_DEBUG_ALL_PRINT_BUFFER_ON_)
#define DBG_PrintU32Buffer(bufferName, p_buffer, bufferLen) do {DBG_PosInfo();\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) begin===========================", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
		if( (p_buffer) != NULL )\
		{\
		    int __i = 0;\
		    for( __i = 0; __i < (int)(bufferLen); __i++ )\
		    {\
		         if( (__i % 16) == 0 )  DBG_Log_Print("\n");\
		         DBG_Log_Print("%08X ", (p_buffer)[__i]);\
		    }\
		}\
		else\
		{\
		    DBG_Log_Print("p_buffer = NULL\n" ); \
		}\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) end===========================\n\n", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
	}while(0);
#else
#define DBG_PrintU32Buffer(bufferName, p_buffer, bufferLen)
#endif


#if defined(_DEBUG_BUFFER_ON_) && defined(_DEBUG_ALL_PRINT_BUFFER_ON_)
#define DBG_PrintBuffer(bufferName, p_buffer, bufferLen) do {DBG_PosInfo();\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) begin===========================\n", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
		if( (p_buffer) != NULL )\
		{\
		    typedef unsigned char _U8_;\
			_U8_ *p_tmp;\
			p_tmp = (_U8_ *)(p_buffer);\
		    int i = 0;\
		    for( i = 0; i < (int)(bufferLen); i++ )\
		    {\
		         DBG_Log_Print("%c", p_tmp[i]);\
		    }\
		}\
		else\
		{\
		    DBG_Log_Print("p_buffer = NULL\n" ); \
		}\
		DBG_Log_Print("\n================Buffer:%s(size = %d(0x%X)) end===========================\n\n", bufferName, (int)(bufferLen), (unsigned int)(bufferLen));\
	}while(0);
#else
#define DBG_PrintBuffer(bufferName, p_buffer, bufferLen)
#endif


/*******************************************************************************
 *                       Debug Time
 *******************************************************************************/
#if defined(_DEBUG_TIME_CONSUME_) && defined(_DEBUG_ALL_TIME_CONSUME_ON_)
static struct timeval g_debugBeginTime;
static struct timeval g_debugEndTime;

#define DBG_BeginTime()    gettimeofday(&g_debugBeginTime, NULL)
#define DBG_EndTime(fmt, args...)      do {long timeDelta;\
                                     long secs;\
                                     long msecs;\
                                     long usecs;\
                                     gettimeofday(&g_debugEndTime, NULL);\
									 DBG_PosInfo();\
                                     DBG_Log_Print(fmt,##args);\
                                     timeDelta = (g_debugEndTime.tv_sec - g_debugBeginTime.tv_sec) * 1000000 + g_debugEndTime.tv_usec - g_debugBeginTime.tv_usec;\
                                     secs = timeDelta / 1000000;\
                                     msecs = (timeDelta % 1000000) /  1000;\
                                     usecs = (timeDelta % 1000000) %  1000;\
                                     DBG_Log_Print(" Using time:%lds, %ldms, %ldus.\n", secs, msecs, usecs);}while(0)
#else
#define DBG_BeginTime()
#define DBG_EndTime(fmt, args...)
#endif

#if defined(_DEBUG_TIME_STAMP_ON_) && defined(_DEBUG_ALL_STAMP_ON_)
#define DBG_TimeStamp(fmt, args...) do{struct timeval timeStamp; gettimeofday(&timeStamp, NULL);LOG_V(fmt"TimeStamp:(%lu sec, %lu usec).\n",##args, timeStamp.tv_sec, timeStamp.tv_usec);}while(0)
#else
#define DBG_TimeStamp(fmt, args...)
#endif

#endif  //_DEBUG_H_
