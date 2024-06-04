#include "object_script.h"

void RZUF3_ObjectScript::attach(RZUF3_Object* object)
{
	this->m_object = object;
}

void RZUF3_ObjectScript::detach()
{
	this->m_object = nullptr;
}
