#include "strings_helper.h"

void RZUF3_StringsHelper::ltrim(std::string& s, const std::string& chars)
{
	s.erase(0, s.find_first_not_of(chars));
}

void RZUF3_StringsHelper::rtrim(std::string& s, const std::string& chars)
{
	s.erase(s.find_last_not_of(chars) + 1);
}

void RZUF3_StringsHelper::trim(std::string& s, const std::string& chars)
{
	ltrim(s, chars);
	rtrim(s, chars);
}

int RZUF3_StringsHelper::getNextCharPos(const std::string& s, int pos, char c)
{
	for (int i = pos; i < s.size(); i++)
	{
		if (s[i] == c) return i;
	}
	return -1;
}

int RZUF3_StringsHelper::getPrevCharPos(const std::string& s, int pos, char c)
{
	for (int i = pos; i >= 0; i--)
	{
		if (s[i] == c) return i;
	}
	return -1;
}
