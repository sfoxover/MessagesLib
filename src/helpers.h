#pragma once
/*
	Static helper functions
*/
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

class Helpers
{
public:

	static bool FileExists(std::wstring path);

	static bool FileExists(std::string path);
	
	static std::string WideToUtf8(std::wstring value);

	static std::wstring Utf8ToWide(std::string value);

	static std::string AppendToRunPath(std::string path);

	static std::vector<std::string> SplitString(std::string value, char separator);
};