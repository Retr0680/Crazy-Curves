#pragma once
#include <string>
#include "EventManager.hpp"

class StatusManager {
public:
    enum class Status {
        READY,
        PROCESSING,
        ERROR,
        SAVING,
        LOADING
    };

    static StatusManager& getInstance() {
        static StatusManager instance;
        return instance;
    }

    void setStatus(Status newStatus, const std::string& message = "");
    Status getCurrentStatus() const { return currentStatus; }
    const std::string& getCurrentMessage() const { return currentMessage; }

private:
    StatusManager();
    Status currentStatus;
    std::string currentMessage;
    EventManager& eventManager;
};