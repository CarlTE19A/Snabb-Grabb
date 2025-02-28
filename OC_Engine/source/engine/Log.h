#pragma once
#include <iostream>
#include <wrl/client.h>
#ifndef _RETAIL

static bool showLog = true;
static bool showWarning = true;
static bool showError = true;
static bool showSuccess = true;
static bool showInfo = true;

static void Log(std::string string)
{
	if(!showLog)
		return;
	if(string == "")
		return;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	std::cout << string << std::endl;
}
static void LogWarning(std::string string)
{
	if (!showWarning)
		return;
	if (string == "")
		return;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
	std::cout << string << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
static void LogError(std::string string)
{
	if (!showError)
		return;
	if (string == "")
		return;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	std::cout << string << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
static void LogSuccess(std::string string)
{
	if (!showSuccess)
		return;
	if (string == "")
		return;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
	std::cout << string << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
static void LogInfo(std::string string)
{
	if (!showInfo)
		return;
	if (string == "")
		return;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
	std::cout << string << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
#else

static void Log(std::string){}
static void LogWarning(std::string){}
static void LogError(std::string){}
static void LogSuccess(std::string){}
static void LogInfo(std::string){}
#endif
