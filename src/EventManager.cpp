#include "EventManager.hpp"

void EventManager::subscribe(EventType type, const std::string& subscriber, EventCallback callback) {
    subscribers[type][subscriber] = callback;
}

void EventManager::unsubscribe(EventType type, const std::string& subscriber) {
    auto it = subscribers.find(type);
    if (it != subscribers.end()) {
        it->second.erase(subscriber);
    }
}

void EventManager::emit(EventType type, void* data) {
    auto it = subscribers.find(type);
    if (it != subscribers.end()) {
        for (const auto& [subscriber, callback] : it->second) {
            callback(data);
        }
    }
}