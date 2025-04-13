#pragma once
#include <functional>
#include <map>
#include <string>
#include <vector>

enum class EventType {
    CurveChanged,
    ChannelChanged,
    PointAdded,
    PointRemoved,
    PointMoved,
    PresetLoaded,
    UndoPerformed,
    RedoPerformed
};

class EventManager {
public:
    using EventCallback = std::function<void(void*)>;
    
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }

    void subscribe(EventType type, const std::string& subscriber, EventCallback callback);
    void unsubscribe(EventType type, const std::string& subscriber);
    void emit(EventType type, void* data = nullptr);

private:
    EventManager() = default;
    std::map<EventType, std::map<std::string, EventCallback>> subscribers;
};