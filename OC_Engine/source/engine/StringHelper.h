#pragma once

//#include <string>

class StringHelper
{
public:
	StringHelper() = delete;
	StringHelper(const StringHelper&) = delete;
	StringHelper& operator=(const StringHelper&) = delete;

	static std::wstring s2ws(const std::string& aString);
	static std::string ws2s(const std::wstring& aWString);
	static std::string GetFileExtension(const std::string& aString);
};
