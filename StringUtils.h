#pragma once
#include <string>
#include <vector>

class StringUtils
{
public:
	StringUtils();
	~StringUtils();

	static bool startsWith(std::string mainStr, std::string toMatch);
	static const std::vector<std::string> explode(const std::string& s, const char& c);
};

