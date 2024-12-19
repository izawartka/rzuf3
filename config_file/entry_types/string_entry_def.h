#pragma once
#include "entry_types.h"

class RZUF3_ConfigStringEntryDef : public RZUF3_ConfigEntryDef {
public:
    explicit RZUF3_ConfigStringEntryDef(const std::string& defaultValue)
        : RZUF3_ConfigEntryDef(defaultValue) {}

    bool parse(std::string& input, void*& output, size_t& size) const override {
        output = new std::string(input);
        size = sizeof(input);
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

    std::type_index getType() const override {
		return typeid(std::string);
	}
};

inline auto RZUF3_ConfigStringEntry(const std::string& defaultValue) {
    return std::make_unique<RZUF3_ConfigStringEntryDef>(defaultValue);
}