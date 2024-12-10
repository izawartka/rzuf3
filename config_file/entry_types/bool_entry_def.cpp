#include "bool_entry_def.h"

bool RZUF3_ConfigBoolEntryDef::parse(std::string& input, void*& output, size_t& size) const
{
	if (!validate(input)) return false;
	output = new bool(input == "true");
	size = sizeof(bool);
	return true;
}

bool RZUF3_ConfigBoolEntryDef::serialize(void* input, std::string& output) const
{
	bool* inputBool = static_cast<bool*>(input);
	output = *inputBool ? "true" : "false";
	return true;
}

bool RZUF3_ConfigBoolEntryDef::validate(std::string& input) const
{
	return input == "true" || input == "false";
}
