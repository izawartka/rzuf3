#pragma once
#include "common.h"
#include "object_script.h"
#include "pos.h"

class RZUF3_ObjectScript;

struct RZUF3_ObjectDefinition {
public:
	std::string name;
	RZUF3_Pos pos = RZUF3_Pos(0, 0);
	std::vector<RZUF3_ObjectScript*> scripts = std::vector<RZUF3_ObjectScript*>();
};