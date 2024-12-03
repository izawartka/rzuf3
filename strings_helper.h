#pragma once
#include "common.h"

#define RZUF3_WHITESPACE " \t\n\r\f\v"

class RZUF3_StringsHelper {
public:
	static void ltrim(std::string& s, const std::string& chars = RZUF3_WHITESPACE);
	static void rtrim(std::string& s, const std::string& chars = RZUF3_WHITESPACE);
	static void trim(std::string& s, const std::string& chars = RZUF3_WHITESPACE);
	static int getNextCharPos(const std::string& s, int pos, char c);
	static int getPrevCharPos(const std::string& s, int pos, char c);
};