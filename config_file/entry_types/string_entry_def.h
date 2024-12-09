#pragma once
#include "entry_types.h"

class RZUF3_ConfigStringEntryDef : public RZUF3_ConfigEntryDef {
public:
    explicit RZUF3_ConfigStringEntryDef(const std::string& defaultValue)
        : RZUF3_ConfigEntryDef(defaultValue) {}

    bool parse(std::string& input, void* output) const override {
        *static_cast<std::string*>(output) = input;
        return true;
    }

    bool serialize(void* input, std::string& output) const override {
        output = *static_cast<std::string*>(input);
        return true;
    }

    bool validate(std::string& input) const override {
		return true;
	}

    std::unique_ptr<RZUF3_ConfigEntryDef> clone() const override {
		return std::make_unique<RZUF3_ConfigStringEntryDef>(m_defaultValue);
	}
};

inline auto RZUF3_ConfigStringEntry(const std::string& defaultValue) {
    return std::make_unique<RZUF3_ConfigStringEntryDef>(defaultValue);
}