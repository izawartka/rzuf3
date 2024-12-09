#include "object_script.h"

void RZUF3_ObjectScript::attach(RZUF3_Object* object)
{
	m_object = object;
}

void RZUF3_ObjectScript::detach()
{
	m_object = nullptr;
}
