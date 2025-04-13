#include "SequenceData.hpp"
#include "AEGP_SuiteHandler.h"

PF_Err SequenceDataManager::Allocate(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    
    SequenceData* seqData = new SequenceData();
    if (!seqData) return PF_Err_OUT_OF_MEMORY;
    
    ERR(InitializeCurves(seqData));
    
    if (!err) {
        seqData->selected_point = -1;
        seqData->needs_update = true;
        seqData->cache = nullptr;
        out_data->sequence_data = seqData;
    } else {
        delete seqData;
    }
    
    return err;
}

PF_Err SequenceDataManager::Dispose(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    
    if (in_data->sequence_data) {
        SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
        CleanupCurves(seqData);
        delete seqData;
    }
    
    return err;
}

PF_Err SequenceDataManager::Flatten(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    if (seqData) {
        seqData->needs_update = true;
    }
    
    return err;
}

PF_Err SequenceDataManager::Setup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    SequenceData* seqData = static_cast<SequenceData*>(suites.HandleSuite1()->host_lock_handle(in_data->sequence_data));
    
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;
    
    ERR(InitializeCurves(seqData));
    
    if (!err) {
        seqData->selected_point = -1;
        seqData->needs_update = true;
        seqData->cache = nullptr;
    }
    
    suites.HandleSuite1()->host_unlock_handle(in_data->sequence_data);
    
    return err;
}

PF_Err SequenceDataManager::InitializeCurves(SequenceData* seqData)
{
    PF_Err err = PF_Err_NONE;
    
    // Initialize RGB curve
    seqData->rgb_curve.curve_id = CURVEDATA_DISK_ID;
    seqData->rgb_curve.num_points = 2;
    seqData->rgb_curve.points[0].x = 0.0f;
    seqData->rgb_curve.points[0].y = 0.0f;
    seqData->rgb_curve.points[1].x = 1.0f;
    seqData->rgb_curve.points[1].y = 1.0f;
    seqData->rgb_curve.dirty = true;
    
    // Copy RGB curve to individual channels
    seqData->r_curve = seqData->rgb_curve;
    seqData->g_curve = seqData->rgb_curve;
    seqData->b_curve = seqData->rgb_curve;
    
    return err;
}

void SequenceDataManager::CleanupCurves(SequenceData* seqData)
{
    if (seqData->cache) {
        delete seqData->cache;
        seqData->cache = nullptr;
    }
}