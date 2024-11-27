#pragma once
#include "common.h"

struct RZUF3_RectD {
	double x;
	double y;
	double w;
	double h;

	RZUF3_RectD(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {}
	RZUF3_RectD() : x(0), y(0), w(0), h(0) {}
};