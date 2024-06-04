#pragma once
#include "common.h"
#include "object.h"
#include "event.h"

class RZUF3_Object;
class RZUF3_Event;

class RZUF3_ObjectScript {
public:
	virtual void attach(RZUF3_Object* object) final;
	virtual void init() {};
	virtual void deinit() {};
	virtual void detach() final;

	virtual RZUF3_Object* getObject() final { return m_object; }

protected:
	RZUF3_Object* m_object = nullptr;
};