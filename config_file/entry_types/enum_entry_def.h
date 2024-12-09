#pragma once
#include "entry_types.h"

class RZUF3_ConfigEnumEntryDef : public RZUF3_ConfigEntryDef {
public:
    RZUF3_ConfigEnumEntryDef(const std::string& defaultValue, const std::vector<std::string>& allowedValues)
        : RZUF3_ConfigEntryDef(defaultValue), m_allowedValues(allowedValues) {}

    bool parse(std::string& input, void* output) const override;
    bool serialize(void* input, std::string& output) const override;
    bool validate(std::string& input) const override;

    std::unique_ptr<RZUF3_ConfigEntryDef> clone() const override {
        return std::make_unique<RZUF3_ConfigEnumEntryDef>(m_defaultValue, m_allowedValues);
    }

private:
    std::vector<std::string> m_allowedValues;

};

inline auto RZUF3_ConfigEnumEntry(const std::string& defaultValue, const std::vector<std::string>& allowedValues) {
    return std::make_unique<RZUF3_ConfigEnumEntryDef>(defaultValue, allowedValues);
}