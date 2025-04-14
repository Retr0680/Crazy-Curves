#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

enum class EventType {
    CurveModified,
    PointAdded,
    PointRemoved,
    PointMoved,
    CurveInterpolated,
    PresetApplied,
    RenderStarted,
    RenderCompleted,
    CacheUpdated,
    UIRefreshNeeded
};

class EventCommunicator {
public:
    using EventCallback = std::function<void(PF_InData*, void*)>;

    static EventCommunicator& getInstance() {
        static EventCommunicator instance;
        return instance;
    }

    PF_Err RegisterListener(
        const std::string& listenerID,
        EventType eventType,
        EventCallback callback);

    PF_Err UnregisterListener(
        const std::string& listenerID,
        EventType eventType);

    PF_Err BroadcastEvent(
        PF_InData* in_data,
        EventType eventType,
        void* eventData);

private:
    EventCommunicator() = default;

    struct EventListener {
        std::string id;
        EventCallback callback;
    };

    std::map<EventType, std::vector<EventListener>> listeners;
};