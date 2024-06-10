#pragma once
#include "common.h"

struct RZUF3_Pos {
	int x;
	int y;
	double scaleX = 1.0;
	double scaleY = 1.0;

	RZUF3_Pos(int x, int y) : x(x), y(y) {}
	RZUF3_Pos(int x, int y, double scaleX, double scaleY) : x(x), y(y), scaleX(scaleX), scaleY(scaleY) {}
	RZUF3_Pos() : x(0), y(0) {}
};