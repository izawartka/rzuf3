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
        for (auto& listeners : m_eventListeners) {
            if (listeners.second.size() == 0) continue;

            spdlog::warn("Deleting EventsManager without removing all event listeners! ({} left) This may lead to unexpected behavior in the future", listeners.second.size());
        }
#endif // _DEBUG
    }

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
        }
    }

    void dispatchEvent(RZUF3_Event* event) {
        std::type_index eventType(typeid(*event));
        auto it = m_eventListeners.find(eventType);

        if (it == m_eventListeners.end()) {
            return;
        }

        EventListenerList& eventListeners = it->second;
        int lastSize = eventListeners.size();

        for (int i = 0; i < lastSize; i++) {
            eventListeners[i].callback(event);

            int size = eventListeners.size();
            if (size < lastSize) {
                i -= lastSize - size;
			}

            lastSize = size;
		}
    }

private:
    EventListenersByType m_eventListeners;
    int m_nextEventListenerID = 0;
};
