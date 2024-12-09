#include "enum_entry_def.h"

bool RZUF3_ConfigEnumEntryDef::parse(std::string& input, void* output) const
{
	if (!validate(input)) return false;

	*static_cast<std::string*>(output) = input;
	return true;
}

bool RZUF3_ConfigEnumEntryDef::serialize(void* input, std::string& output) const
{
	std::string outputStr = *static_cast<std::string*>(input);
	if (!validate(outputStr)) return false;

	output = outputStr;
	return true;
}

bool RZUF3_ConfigEnumEntryDef::validate(std::string& input) const
{
	return std::find(m_allowedValues.begin(), m_allowedValues.end(), input) != m_allowedValues.end();
}