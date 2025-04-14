#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <string>

class VersionManager {
public:
    static VersionManager& getInstance() {
        static VersionManager instance;
        return instance;
    }

    PF_Err CheckVersion(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err UpdateSequenceData(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err ValidateDataStructures(
        const SequenceData* seqData);

private:
    VersionManager() = default;

    static const A_u_long CURRENT_VERSION = 1;
    static const A_u_long MIN_SUPPORTED_VERSION = 1;

    struct VersionInfo {
        A_u_long major;
        A_u_long minor;
        A_u_long bugfix;
        std::string buildDate;
    };

    PF_Err MigrateData(
        PF_InData* in_data,
        A_u_long fromVersion,
        A_u_long toVersion);
};