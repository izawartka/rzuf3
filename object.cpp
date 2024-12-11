#include "object.h"
#include "object_definition.h"
#include "object_script.h"
#include "scene.h"

RZUF3_Object::RZUF3_Object(RZUF3_ObjectDefinition objectDefinition, RZUF3_Scene* scene)
{
	m_initialized = false;
	m_name = objectDefinition.name;
	m_scene = scene;
	m_scripts = std::vector<RZUF3_ObjectScript*>();

	setParent(objectDefinition.parentName);
	setPos(objectDefinition.pos);

	for(auto script : objectDefinition.scripts)
	{
		addScript(script);
	}
}

RZUF3_Object::~RZUF3_Object()
{
	while(!m_scripts.empty())
	{
		removeScript(m_scripts.back());
	}

	while (!m_children.empty())
	{
		m_children.begin()->second->setParent(nullptr);
	}

	setParent(nullptr);
}

void RZUF3_Object::init()
{
	if(m_initialized) return;
	m_initialized = true;

	std::vector<RZUF3_ObjectScript*> scriptsOnInit = m_scripts;
	for (RZUF3_ObjectScript*& script : scriptsOnInit)
	{
		script->init();
	}
}

bool RZUF3_Object::addScript(RZUF3_ObjectScript* script)
{
	if(script->getObject() != nullptr)
	{
		spdlog::error("Script {} is already attached to {}, cannot attach it to {}",
			fmt::ptr(script),
			script->getObject()->getName(),
			getName()
		);
		return false;
	}

	m_scripts.push_back(script);
	script->attach(this);
	if(m_initialized) script->init();
	return true;
}

void RZUF3_Object::removeScript(RZUF3_ObjectScript* script)
{
	auto it = std::find(m_scripts.begin(), m_scripts.end(), script);
	if(it == m_scripts.end())
	{
		// maybe already removed
		// do not throw warning here, it may cause accessing deleted object
		return;
	}

	(*it)->deinit();
	(*it)->detach();
	m_scripts.erase(it);
}

void RZUF3_Object::setPos(RZUF3_Pos pos)
{
	m_pos = pos;

	updateAbsolutePos();
}

void RZUF3_Object::updateAbsolutePos()
{
	m_absPos = m_pos;

	if (m_parent != nullptr) {
		RZUF3_Pos parentPos = m_parent->getAbsolutePos();

		m_absPos.x = parentPos.x + m_pos.x * parentPos.scaleX;
		m_absPos.y = parentPos.y + m_pos.y * parentPos.scaleY;
		m_absPos.scaleX *= parentPos.scaleX;
		m_absPos.scaleY *= parentPos.scaleY;
	}

	for (auto& child : m_children)
	{
		child.second->updateAbsolutePos();
	}
}

void RZUF3_Object::setParent(RZUF3_Object* parent)
{
	if (m_parent != nullptr) {
		m_parent->m_children.erase(m_name);
	}

	m_parent = parent;
	if (m_parent) m_parent->m_children.insert(std::pair(m_name, this));

	updateAbsolutePos();
}

bool RZUF3_Object::setParent(std::string parentName)
{
	if (parentName.empty()) return false;

	RZUF3_Object* parent = m_scene->getObject(parentName);

	if (!parent) {
		spdlog::error("Could not find the parent object");
		return false;
	}

	setParent(parent);
	return true;
}

RZUF3_EventsManager* RZUF3_Object::getEventsManager()
{
	return &m_eventsManager;
}
