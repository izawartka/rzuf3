#pragma once
#include "common.h"
#include "object_script.h"
#include "scene.h"
#include "object_definition.h"
#include "pos.h"
#include "events_manager.h"

class RZUF3_Scene;
class RZUF3_ObjectScript;
class RZUF3_ObjectDefinition;

class RZUF3_Object {
public:
	RZUF3_Object(RZUF3_ObjectDefinition objectDefinition, RZUF3_Scene* scene);
	~RZUF3_Object();

	void init();

	bool addScript(RZUF3_ObjectScript* script);
	void removeScript(RZUF3_ObjectScript* script);

	RZUF3_Scene* getScene() { return m_scene; }
	std::string getName() { return m_name; }
	RZUF3_Pos getPos() const { return m_pos; }
	void setPos(RZUF3_Pos pos) { m_pos = pos; }

	RZUF3_EventsManager* getEventsManager();

private:
	bool m_initialized;
	RZUF3_Scene* m_scene;
	RZUF3_EventsManager m_eventsManager;
	std::string m_name;
	std::vector<RZUF3_ObjectScript*> m_scripts;
	RZUF3_Pos m_pos;
};