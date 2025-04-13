#include "ErrorHandler.hpp"
#include <sstream>
#include <chrono>

ErrorHandler::ErrorHandler() : eventManager(EventManager::getInstance()) {
    // Register with event manager for system-wide errors
    eventManager.subscribe(EventType::CurveChanged, "ErrorHandler",
        [this](void* data) {
            if (!data) {
                reportError("Invalid curve data received", ErrorSeverity::WARNING);
            }
        });
}

void ErrorHandler::registerCallback(const std::string& id, ErrorCallback callback) {
    callbacks[id] = callback;
}

void ErrorHandler::unregisterCallback(const std::string& id) {
    callbacks.erase(id);
}

void ErrorHandler::reportError(const std::string& message, ErrorSeverity severity) {
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::ctime(&time) << " - " << message;

    // Notify all registered callbacks
    for (const auto& [id, callback] : callbacks) {
        callback(ss.str(), severity);
    }

    // Update status manager if error is severe enough
    if (severity >= ErrorSeverity::ERROR) {
        StatusManager::getInstance().setStatus(StatusManager::Status::ERROR, message);
    }
}

void ErrorHandler::clearErrors() {
    // Notify all callbacks that errors are cleared
    for (const auto& [id, callback] : callbacks) {
        callback("Errors cleared", ErrorSeverity::INFO);
    }
}