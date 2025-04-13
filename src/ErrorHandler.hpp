#pragma once
#include <string>
#include <functional>
#include <map>
#include "EventManager.hpp"

enum class ErrorSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const std::string&, ErrorSeverity)>;

    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    void registerCallback(const std::string& id, ErrorCallback callback);
    void unregisterCallback(const std::string& id);
    void reportError(const std::string& message, ErrorSeverity severity);
    void clearErrors();

private:
    ErrorHandler();
    std::map<std::string, ErrorCallback> callbacks;
    EventManager& eventManager;
};