#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <locale>
#include <tchar.h>

#include "INoCopy.h"
#pragma warning(disable:4996)

// 添加一些宏定义，方便操作日志

// log level define
#define LOGGER_LEVEL	slogger::LogLevel
#define LL_NONE			slogger::ll_none
#define LL_FATAL		slogger::ll_fatal
#define LL_ERROR		slogger::ll_error
#define LL_WARNING		slogger::ll_warning
#define LL_INFO			slogger::ll_info
#define LL_DEBUG		slogger::ll_debug
#define LL_TRACE		slogger::ll_trace
#define LL_ALL			slogger::ll_all


// logger disable all
#ifndef LOGGER_DISABLE
#define LOGGER_DECLARE(logger)	slogger::Logger logger
#define LOGGER_USING(logger)	extern slogger::Logger logger
#define LOGGER_CLOG(logger, ll, fmt, ...)\
	do \
	{\
		if((ll) <= (logger).getLogLevel())\
		{\
			(logger).log((ll), (fmt), __VA_ARGS__);\
		}\
	}while(0)

#else
#define LOGGER_DEFINE(logger)
#define LOGGER_USING(logger)
#define LOGGER_CLOG(logger, ll, fmt, args)
#endif


// logger disable ll_fatal
#if !defined(LOGGER_DISABLE_FATAL) && !defined(LOGGER_DISABLE)
#define LOGGER_FATAL(logger, msg) (logger) << slogger::ll_fatal << ##msg## << (logger)
#define LOGGER_CFATAL(logger, fmt, ...) LOGGER_CLOG((logger), (LL_FATAL), (fmt), __VA_ARGS__)
#else
#define LOGGER_FATAL(logger, msg)
#define LOGGER_CFATAL(logger, fmt, ...)
#endif

// logger disable ll_error
#if !defined(LOGGER_DISABLE_ERROR) && !defined(LOGGER_DISABLE)
#define LOGGER_ERROR(logger, msg) (logger) << slogger::ll_error << ##msg## << (logger)
#define LOGGER_CERROR(logger, fmt, ...) LOGGER_CLOG((logger), (LL_ERROR), (fmt), __VA_ARGS__)
#else
#define LOGGER_ERROR(logger, msg)
#define LOGGER_CERROR(logger, fmt, ...)
#endif

// logger disable ll_info
#if !defined(LOGGER_DISABLE_INFO) && !defined(LOGGER_DISABLE)
#define LOGGER_INFO(logger, msg) (logger) << slogger::ll_info << ##msg## << (logger)
#define LOGGER_CINFO(logger, fmt, ...) LOGGER_ClOG((logger), (LL_INFO), (fmt), __VA_ARGS__)
#else
#define LOGGER_INFO(logger, msg)
#define LOGGER_CINFO(logger, fmt, ...)
#endif

// logger disable ll_warning
#if !defined(LOGGER_DISABLE_WARNING) && !defined(LOGGER_DISABLE)
#define LOGGER_WARNING(logger, msg) (logger) << slogger::ll_warning << ##msg## << (logger)
#define LOGGER_CWARNING(logger, fmt, ...) LOGGER_CLOG(logger, (LL_WARNING), (fmt), __VA_ARGS__)
#else
#define LOGGER_WARNING(logger, msg)
#define LOGGER_CWARNING(logger, fmt, ...)
#endif

// logger disable ll_debug
#if !defined(LOGGER_DIABLE) && !defined(LOGGER_DISABLE)
#define LOGGER_DEBUG(logger, msg) (logger) << slogger::ll_debug << ##msg## << (logger)
#define LOGGER_CDEBUG(log, fmt, ...) LOGGER_CLOG((logger), (LL_DEBUG), (fmt), __VA_ARGS__)
#else
#define LOGGER_DEBUG(logger, msg)
#define LOGGER_CDEBUG(loger, fmt, ...)
#endif

// logger disable ll_trace
#if !defined(LOGGER_DISABLE_TRACE) && !defined(LOGGER_DISABLE)
#define LOGGER_TRACE(logger, msg) (logger) << slogger::ll_trace << ##msg## (logger)
#define LOGGER_CTRACE(logger, fmt, ...) << LOGGER_CLOG((logger), (LL_TRACE), fmt, __VA_ARGS__)
#else
#define LOGGER_TRACE(logger, msg)
#define LOGGER_CTRACE(logger, fmt, ...)
#endif

namespace slogger
{
#if defined(_UNICODE) || defined (UNICODE)
	typedef std::wstring		tstring;
	typedef std::wostringstream tstringstream;
	typedef std::wostream		tostream;
	typedef wchar_t				tchar;
	typedef std::wofstream		tofstream;
	#define tcout				std::wcout
#else
	typedef std::string			tstring;
	typedef std::ostringstream	tstringstream;
	typedef std::ostream		tostream;
	typedef char				tchar;
	typedef std::ofstream		tofstream;
	#define tcout				std::cout
#endif

	// log level
	enum LogLevel
	{
		ll_none = 0,
		ll_fatal,
		ll_error,
		ll_warning,
		ll_info,
		ll_debug,
		ll_trace,
		ll_all
	};

	const tchar *LOGLEVEL_NAME[];

	// 输出的基类
	class Appender
	{
	public:
		Appender(){}
		virtual ~Appender(){}
		virtual  int append(const tstring &logMsg) = 0;
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual bool autoDelete() = 0;
	};

	typedef std::vector<Appender *> appender_list_t;

	class Logger : public INoCopy
	{
	private:
		tstring			_timeFmt;	// time format
		LogLevel		_ll;		// log level
		LogLevel		_llTmp;		// for cpp style output
		tstringstream	_buf;		// for cpp style output
		appender_list_t _appenders;	// appender list
		bool			_mt;		// multiple thread mode
		bool			_isOpen;	// is open

		// 对于同步问题，windows使用临界区，linux使用互斥量
#ifdef _WIN32
		CRITICAL_SECTION _locker;	// multi-thread lock
#else
		pthread_mutex_t	 _locker;	// for linux
#endif

	private:
		int _doLog(LogLevel ll, const tchar *msg);	// format time string and call appenders to write log.
		int _flush();								// flush() internal buffer to appenders, for cpp style output

		void _lock();								// multi thread lock
		void _unlock();

	public:
		Logger();
		virtual ~Logger();

		// 初始化和设置
		bool open(bool mt = true, LogLevel ll = ll_all, const tstring &timeFmt = _T("%m/%d/%Y-%H:%M:%S"));
		bool close();
		bool isOpen(){ return _isOpen; }


		// 日志级别
		LogLevel getLogLevel(){ return _ll; }

		LogLevel setLogLevel(LogLevel ll);

		// 添加输出器
		int addAppender(Appender *app);

		// 3种预定义输出器
		int addFileAppender(const tstring &fileName, unsigned long maxBytes = 5 * 1024 * 1024, bool multiFile = true);
		int addConsoleAppender(tostream &os = tcout);	// 预定义的标准输出器，可以是cout / wcout， cerror / werror, clog / wlog
		int addDebugAppender();							// 预定义的调试器输出器
		bool removeAppender(int appenderHandle);

		// 输出日志
		// cpp style 调用格式: theLogger << ll_info << 1234 << _T("Hello World!") << 0.12 << theLogger;
		// 如果忘记最后的<< theLogger,可能会导致多线程死锁
		// 运用宏，避免忘记最后的 << theLogger
		tstringstream &operator << (const LogLevel ll);
		friend Logger &operator << (tostream &ss, Logger &logger);

		// c style 调用格式
		int log(LogLevel ll, const tchar *fmt, ...);

	};

};


