#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <string>

class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    PF_Err HandleError(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_Err err,
        const std::string& context);

    PF_Err LogError(
        PF_InData* in_data,
        PF_Err err,
        const std::string& message);

    bool HasError() const { return lastError != PF_Err_NONE; }
    PF_Err GetLastError() const { return lastError; }
    const std::string& GetLastErrorContext() const { return lastErrorContext; }

private:
    ErrorHandler() : lastError(PF_Err_NONE) {}
    
    PF_Err lastError;
    std::string lastErrorContext;
    
    void DisplayError(
        PF_InData* in_data,
        PF_OutData* out_data,
        const std::string& message);
};