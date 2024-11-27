#include "draggable_root.h"
#include "../game.h"

RZUF3_DraggableRoot::RZUF3_DraggableRoot(RZUF3_DraggableRootOptions options)
{
	mp_options = options;
}

RZUF3_DraggableRoot::~RZUF3_DraggableRoot()
{

}

void RZUF3_DraggableRoot::init()
{
	m_x = 0;
	m_y = 0;
	m_scale = 1.0;

	linkContent();

	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, DraggableUpdate);
}

void RZUF3_DraggableRoot::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, DraggableUpdate);

	m_contentObj = nullptr;
}

void RZUF3_DraggableRoot::onDraggableUpdate(RZUF3_DraggableUpdateEvent* event)
{
	m_x = event->getX();
	m_y = event->getY();
	m_scale = event->getScale();

	updateContentPosition();
}

void RZUF3_DraggableRoot::linkContent()
{
	m_contentObj = g_scene->getObject(mp_options.contentObjName);
	if (m_contentObj == nullptr) {
		spdlog::error("Draggable content object not found");
		return;
	}
}

void RZUF3_DraggableRoot::updateContentPosition()
{
	if(!m_contentObj) return;

	m_contentObj->setPos({
		m_x,
		m_y,
		m_scale,
		m_scale
	});
}
