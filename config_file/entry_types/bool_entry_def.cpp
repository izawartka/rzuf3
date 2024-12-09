#include "bool_entry_def.h"

bool RZUF3_ConfigBoolEntryDef::parse(std::string& input, void* output) const
{
	if (!validate(input)) return false;
	*static_cast<bool*>(output) = input == "true";
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
