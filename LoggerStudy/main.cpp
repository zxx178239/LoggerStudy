#include "Logger.h"

//LOGGER_USING(logger);

int main(int argc, char *argv[])
{
	// 声明日志
	LOGGER_DECLARE(theLogger);

	// 打开日志系统
	theLogger.open();

	// 初始化输出器
	int consoleApender = theLogger.addConsoleAppender();

	LOGGER_INFO(theLogger, _T("Hello world\n"));

	//theLogger.removeAppender(consoleApender);
}