#pragma once
#include "events.h"

class RZUF3_UIValueChangeEvent : public RZUF3_Event {
public:
	RZUF3_UIValueChangeEvent(std::type_index typeIndex, void* value, size_t size)
		: RZUF3_Event(), m_typeIndex(typeIndex), m_value(value), m_size(size) {}

	std::type_index getTypeIndex() const { return m_typeIndex; }
	void* getValue() const { return m_value; }
	size_t getSize() const { return m_size; }

private:
	std::type_index m_typeIndex;
	void* m_value;
	size_t m_size;
};