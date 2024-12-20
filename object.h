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
	RZUF3_Pos getAbsolutePos() const { return m_absPos; }
	std::map<std::string, RZUF3_Object*> getChildren() { return m_children; }
	RZUF3_Object* getParent() { return m_parent; }

	void setPos(RZUF3_Pos pos);
	void updateAbsolutePos();
	void setParent(RZUF3_Object* parent);
	bool setParent(std::string parentName);

	RZUF3_EventsManager* getEventsManager();

private:
	bool m_initialized;
	RZUF3_Scene* m_scene;
	RZUF3_EventsManager* m_eventsManager = nullptr;
	std::string m_name;
	std::vector<RZUF3_ObjectScript*> m_scripts;
	std::map<std::string, RZUF3_Object*> m_children;
	RZUF3_Object* m_parent = nullptr;
	RZUF3_Pos m_pos;
	RZUF3_Pos m_absPos;
};