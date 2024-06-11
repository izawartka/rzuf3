#pragma once
#include "common.h"
#include "event.h"
#include "event_listener.h"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <algorithm>
#include <iostream>

class RZUF3_Event;
struct RZUF3_EventListener;

class RZUF3_EventsManager {
public:
    RZUF3_EventsManager() {}
    ~RZUF3_EventsManager() {}

    using EventListenerList = std::vector<RZUF3_EventListener>;
    using EventListenersByType = std::unordered_map<std::type_index, EventListenerList>;

    template <class T>
    typename std::enable_if<std::is_base_of<RZUF3_Event, T>::value, int>::type
        addEventListener(RZUF3_EventCallback callback) {
        std::type_index eventType(typeid(T));

        int id = m_nextEventListenerID++;

        RZUF3_EventListener eventListener(
            id,
            eventType,
            callback
        );

        m_eventListeners[eventType].push_back(eventListener);

        return id;
    }

    template <class T>
    typename std::enable_if<std::is_base_of<RZUF3_Event, T>::value, void>::type
        removeEventListener(int eventListenerID) {
        std::type_index eventType(typeid(T));

        auto it = m_eventListeners.find(eventType);
        if (it != m_eventListeners.end()) {
            it->second.erase(
                std::remove_if(
                    it->second.begin(),
                    it->second.end(),
                    [eventListenerID](RZUF3_EventListener& eventListener) { return eventListener.id == eventListenerID; }
                ),
                it->second.end()
            );

            if (it->second.empty()) {
                m_eventListeners.erase(it);
            }
        }
    }

    void dispatchEvent(RZUF3_Event* event) {
        std::type_index eventType(typeid(*event));
        auto it = m_eventListeners.find(eventType);

        if (it == m_eventListeners.end()) {
            return;
        }

        for (RZUF3_EventListener& eventListener : it->second) {
            eventListener.callback(event);
        }
    }

private:
    EventListenersByType m_eventListeners;
    int m_nextEventListenerID = 0;
};
