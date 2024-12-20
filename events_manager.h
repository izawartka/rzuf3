#pragma once
#include "common.h"
#include "event.h"
#include "event_listener.h"

class RZUF3_Event;
struct RZUF3_EventListener;

class RZUF3_EventsManager {
public:
    RZUF3_EventsManager() {}
    ~RZUF3_EventsManager() {
#ifdef _DEBUG
        for (auto& eventTypes : m_eventTypes) {
            if (eventTypes.second.listeners.size() == 0) continue;

            spdlog::warn("Deleting EventsManager without removing all event listeners! ({} left) This may lead to unexpected behavior in the future", eventTypes.second.listeners.size());
        }
#endif // _DEBUG
    }
    void destroy() {
        m_destroyed = true;
        if(m_dispatchDepth <= 0) delete this;
    }

    struct EventListenerList {
        std::vector<RZUF3_EventListener> listeners;
        std::vector<int> dispatchIterators;
	};

    using EventListenersByType = std::unordered_map<std::type_index, EventListenerList>;

    template <class T>
    typename std::enable_if<std::is_base_of<RZUF3_Event, T>::value, int>::type
        addEventListener(RZUF3_EventCallback callback) {
        std::type_index eventTypeIndex(typeid(T));

        int id = m_nextEventListenerID++;

        RZUF3_EventListener eventListener(
            id,
            eventTypeIndex,
            callback
        );

        m_eventTypes[eventTypeIndex].listeners.push_back(eventListener);

        return id;
    }

    template <class T>
    typename std::enable_if<std::is_base_of<RZUF3_Event, T>::value, void>::type
        removeEventListener(int eventListenerID) {
        std::type_index eventTypeIndex(typeid(T));

        auto eventTypeIt = m_eventTypes.find(eventTypeIndex);
        if (eventTypeIt == m_eventTypes.end()) return;

        auto& eventListenerList = eventTypeIt->second;
        auto& eventListeners = eventListenerList.listeners;
        auto eventListenerIt = std::find_if(
            eventListeners.begin(),
            eventListeners.end(),
			[eventListenerID](RZUF3_EventListener& eventListener) { return eventListener.id == eventListenerID; }
		);

        if (eventListenerIt == eventListeners.end()) return;

        for (auto& dispatchIterator : eventListenerList.dispatchIterators) {
            if (dispatchIterator >= eventListenerIt - eventListeners.begin()) {
                dispatchIterator--;
            }
        }

        eventListeners.erase(eventListenerIt);
    }

    void dispatchEvent(RZUF3_Event* event) {
        std::type_index eventTypeIndex(typeid(*event));
        auto eventTypeIt = m_eventTypes.find(eventTypeIndex);

        if (eventTypeIt == m_eventTypes.end()) {
            return;
        }

        EventListenerList& eventListenerList = eventTypeIt->second;
        auto& eventListeners = eventListenerList.listeners;
        eventListenerList.dispatchIterators.push_back(0);
        m_dispatchDepth++;
        int& i = eventListenerList.dispatchIterators.back();

        for (i = 0; i < eventListeners.size(); i++) {
            eventListeners[i].callback(event);
            if (m_destroyed) break;
		}

        eventListenerList.dispatchIterators.pop_back();
        m_dispatchDepth--;

        if (m_dispatchDepth < 0) {
			spdlog::error("EventsManager dispatch depth is negative!");
		}
        if (m_destroyed && m_dispatchDepth <= 0) delete this;
    }

private:
    EventListenersByType m_eventTypes;
    int m_nextEventListenerID = 0;
    int m_dispatchDepth = 0;
    bool m_destroyed = false;
};
