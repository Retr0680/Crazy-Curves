#include "ActionHandler.hpp"

PF_Err ActionHandler::HandleNewSequence(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    
    SequenceData* seqData = new SequenceData();
    if (!seqData) return PF_Err_OUT_OF_MEMORY;
    
    ERR(InitializeCurves(seqData));
    
    if (!err) {
        out_data->sequence_data = seqData;
    } else {
        delete seqData;
    }
    
    return err;
}

PF_Err ActionHandler::HandleResetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    if (in_data->sequence_data) {
        SequenceData* seqData = reinterpret_cast<SequenceData*>(
            suites.HandleSuite1()->host_lock_handle(in_data->sequence_data));
            
        if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;
        
        ERR(InitializeCurves(seqData));
        
        suites.HandleSuite1()->host_unlock_handle(in_data->sequence_data);
    }
    
    return err;
}

PF_Err ActionHandler::HandleFlattenSequence(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    
    if (seqData) {
        // Update cache and flatten any temporary data
        if (seqData->cache) {
            seqData->cache->invalidate();
        }
        seqData->needs_update = true;
    }
    
    return err;
}

PF_Err ActionHandler::HandleSequenceSetdown(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    
    if (seqData) {
        ERR(CleanupSequenceData(seqData));
        delete seqData;
    }
    
    return err;
}

PF_Err ActionHandler::InitializeCurves(SequenceData* seqData)
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
    
    // Initialize individual channel curves
    seqData->r_curve = seqData->rgb_curve;
    seqData->g_curve = seqData->rgb_curve;
    seqData->b_curve = seqData->rgb_curve;
    
    return err;
}

PF_Err ActionHandler::CleanupSequenceData(SequenceData* seqData)
{
    PF_Err err = PF_Err_NONE;
    
    if (seqData->cache) {
        delete seqData->cache;
        seqData->cache = nullptr;
    }
    
    return err;
}