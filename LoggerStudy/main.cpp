#include "Logger.h"

//LOGGER_USING(logger);

int main(int argc, char *argv[])
{
	// ������־
	LOGGER_DECLARE(theLogger);

	// ����־ϵͳ
	theLogger.open();

	// ��ʼ�������
	int consoleApender = theLogger.addConsoleAppender();

	LOGGER_INFO(theLogger, _T("Hello world\n"));

	//theLogger.removeAppender(consoleApender);
}