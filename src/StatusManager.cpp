#include "StatusManager.hpp"

StatusManager::StatusManager() 
    : currentStatus(Status::READY)
    , eventManager(EventManager::getInstance())
{
    // Subscribe to relevant events
    eventManager.subscribe(EventType::CurveChanged, "StatusManager", 
        [this](void*) { setStatus(Status::PROCESSING, "Updating curve..."); });
        
    eventManager.subscribe(EventType::PresetLoaded, "StatusManager",
        [this](void*) { setStatus(Status::READY, "Preset loaded successfully"); });
}

void StatusManager::setStatus(Status newStatus, const std::string& message) {
    currentStatus = newStatus;
    currentMessage = message;
    
    // You could emit an event here if other components need to know about status changes
    struct StatusData {
        Status status;
        std::string message;
    };
    
    StatusData data{currentStatus, currentMessage};
    eventManager.emit(EventType::StatusChanged, &data);
}