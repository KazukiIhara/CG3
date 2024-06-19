#pragma once
#include <string>
#include <format>
#include <Windows.h>

class cLog
{
public: /*静的メンバ関数*/
	static void Log(const std::string& message);
	static std::wstring ConvertString(const std::string& str);
	static std::string ConvertString(const std::wstring& str);
private:

};