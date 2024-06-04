#pragma once
#include "common.h"

struct RZUF3_Pos {
	int x;
	int y;

	RZUF3_Pos(int x, int y) : x(x), y(y) {}
	RZUF3_Pos() : x(0), y(0) {}
};