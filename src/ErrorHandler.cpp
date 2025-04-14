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

PF_Err ErrorHandler::HandleError(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_Err err,
    const std::string& context)
{
    if (err != PF_Err_NONE) {
        lastError = err;
        lastErrorContext = context;

        // Log error
        ERR(LogError(in_data, err, context));

        // Display error to user if appropriate
        std::string errorMessage;
        switch (err) {
            case PF_Err_OUT_OF_MEMORY:
                errorMessage = "Out of memory in: " + context;
                break;
            case PF_Err_INTERNAL_STRUCT_DAMAGED:
                errorMessage = "Internal structure damaged in: " + context;
                break;
            case PF_Err_INVALID_INDEX:
                errorMessage = "Invalid index in: " + context;
                break;
            case PF_Err_UNRECOGNIZED_PARAM_TYPE:
                errorMessage = "Unrecognized parameter type in: " + context;
                break;
            case PF_Err_BAD_CALLBACK_PARAM:
                errorMessage = "Invalid parameter in: " + context;
                break;
            default:
                errorMessage = "Error " + std::to_string(err) + " in: " + context;
                break;
        }

        DisplayError(in_data, out_data, errorMessage);

        // Set error flags
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
    }

    return err;
}

PF_Err ErrorHandler::LogError(
    PF_InData* in_data,
    PF_Err err,
    const std::string& message)
{
    PF_Err logging_err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    try {
        // Format error message
        std::string logMessage = 
            "Error " + std::to_string(err) + 
            " at time " + std::to_string(in_data->current_time) +
            ": " + message;

        // Log to AE's error log
        suites.ANSICallbacksSuite1()->sprintf(
            out_data->return_msg,
            logMessage.c_str());
    }
    catch (...) {
        logging_err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    return logging_err;
}

void ErrorHandler::DisplayError(
    PF_InData* in_data,
    PF_OutData* out_data,
    const std::string& message)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Display in AE's error dialog
    suites.ANSICallbacksSuite1()->sprintf(
        out_data->return_msg,
        message.c_str());
}