#pragma once
#include "../common.h"

class RZUF3_ConfigEntryDef {
protected:
    std::string m_defaultValue;

public:
    explicit RZUF3_ConfigEntryDef(const std::string& defaultValue)
        : m_defaultValue(defaultValue) {}

    virtual ~RZUF3_ConfigEntryDef() = default;

    const std::string& getDefaultValue() const {
        return m_defaultValue;
    }

    virtual bool parse(std::string&, void* output) const = 0;
    virtual bool serialize(void* input, std::string& output) const = 0;
    virtual bool validate(std::string& input) const = 0;

    virtual std::unique_ptr<RZUF3_ConfigEntryDef> clone() const = 0;
};