#include "EventCommunicator.hpp"

PF_Err EventCommunicator::RegisterListener(
    const std::string& listenerID,
    EventType eventType,
    EventCallback callback)
{
    PF_Err err = PF_Err_NONE;

    EventListener listener;
    listener.id = listenerID;
    listener.callback = callback;

    listeners[eventType].push_back(listener);

    return err;
}

PF_Err EventCommunicator::UnregisterListener(
    const std::string& listenerID,
    EventType eventType)
{
    PF_Err err = PF_Err_NONE;

    auto& eventListeners = listeners[eventType];
    eventListeners.erase(
        std::remove_if(
            eventListeners.begin(),
            eventListeners.end(),
            [&listenerID](const EventListener& listener) {
                return listener.id == listenerID;
            }
        ),
        eventListeners.end()
    );

    return err;
}

PF_Err EventCommunicator::BroadcastEvent(
    PF_InData* in_data,
    EventType eventType,
    void* eventData)
{
    PF_Err err = PF_Err_NONE;

    auto it = listeners.find(eventType);
    if (it != listeners.end()) {
        for (const auto& listener : it->second) {
            try {
                listener.callback(in_data, eventData);
            }
            catch (...) {
                err = PF_Err_INTERNAL_STRUCT_DAMAGED;
                break;
            }
        }
    }

    return err;
}