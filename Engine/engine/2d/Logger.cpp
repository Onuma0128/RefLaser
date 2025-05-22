#include "Logger.h"
#include <windows.h>

void Logger::Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}
