#pragma once
#include "entry_types.h"

class RZUF3_ConfigBoolEntryDef : public RZUF3_ConfigEntryDef {
public:
    explicit RZUF3_ConfigBoolEntryDef(bool defaultValue)
        : RZUF3_ConfigEntryDef(defaultValue ? "true" : "false") {}

    bool parse(std::string& input, void*& output, size_t& size) const override;
    bool serialize(void* input, std::string& output) const override;
    bool validate(std::string& input) const override;

    std::unique_ptr<RZUF3_ConfigEntryDef> clone() const override {
        return std::make_unique<RZUF3_ConfigBoolEntryDef>(m_defaultValue == "true"); 
    }

    std::type_index getType() const override {
		return typeid(bool);
	}
};

inline auto RZUF3_ConfigBoolEntry(const bool defaultValue) {
    return std::make_unique<RZUF3_ConfigBoolEntryDef>(defaultValue);
}