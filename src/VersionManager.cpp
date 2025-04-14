#include "VersionManager.hpp"

PF_Err VersionManager::CheckVersion(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Check version compatibility
    if (in_data->version.major < MIN_SUPPORTED_VERSION) {
        PF_STRCPY(out_data->return_msg,
            "This project was created with an older unsupported version.\n"
            "Please update the project data structures.");
        return PF_Err_INVALID_VERSION;
    }

    // Set version info
    out_data->my_version.major = MAJOR_VERSION;
    out_data->my_version.minor = MINOR_VERSION;
    out_data->my_version.bug_version = BUG_VERSION;
    out_data->my_version.stage_version = STAGE_VERSION;
    out_data->my_version.build = BUILD_VERSION;

    return err;
}

PF_Err VersionManager::UpdateSequenceData(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Check if data needs migration
    if (seqData->curve_data_id < CURRENT_VERSION) {
        ERR(MigrateData(in_data, seqData->curve_data_id, CURRENT_VERSION));
        
        if (!err) {
            seqData->curve_data_id = CURRENT_VERSION;
            out_data->out_flags |= PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING;
        }
    }

    return err;
}

PF_Err VersionManager::ValidateDataStructures(
    const SequenceData* seqData)
{
    PF_Err err = PF_Err_NONE;

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Validate curve data
    for (int i = 0; i < 4; i++) {
        const CurveData* curve = nullptr;
        switch (i) {
            case 0: curve = &seqData->rgb_curve; break;
            case 1: curve = &seqData->r_curve; break;
            case 2: curve = &seqData->g_curve; break;
            case 3: curve = &seqData->b_curve; break;
        }

        if (!curve) return PF_Err_INTERNAL_STRUCT_DAMAGED;

        // Check number of points
        if (curve->num_points < 2 || curve->num_points > 256) {
            return PF_Err_BAD_CALLBACK_PARAM;
        }

        // Validate point ordering
        for (A_long j = 1; j < curve->num_points; j++) {
            if (curve->points[j].x <= curve->points[j-1].x) {
                return PF_Err_BAD_CALLBACK_PARAM;
            }
        }
    }

    return err;
}

PF_Err VersionManager::MigrateData(
    PF_InData* in_data,
    A_u_long fromVersion,
    A_u_long toVersion)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Handle version-specific migrations
    switch (fromVersion) {
        case 0:
            // Migrate from version 0 to 1
            // Initialize default curves if needed
            if (seqData->rgb_curve.num_points < 2) {
                seqData->rgb_curve.num_points = 2;
                seqData->rgb_curve.points[0] = {0.0f, 0.0f};
                seqData->rgb_curve.points[1] = {1.0f, 1.0f};
            }
            // Fall through to next version...

        default:
            break;
    }

    return err;
}