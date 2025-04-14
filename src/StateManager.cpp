#include "StateManager.hpp"

PF_Err StateManager::Initialize(PF_InData* in_data) {
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Initialize default state
    currentCurve = nullptr;
    selectedPointIndex = -1;
    isDirty = false;

    return err;
}

PF_Err StateManager::Shutdown() {
    PF_Err err = PF_Err_NONE;

    // Cleanup any resources
    currentCurve = nullptr;
    selectedPointIndex = -1;
    isDirty = false;

    return err;
}

PF_Err StateManager::SaveState(PF_InData* in_data) {
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Get sequence data
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Create state storage
    SavedState* state = new SavedState;
    if (!state) return PF_Err_OUT_OF_MEMORY;

    // Save current state
    state->curves[0] = seqData->rgb_curve;
    state->curves[1] = seqData->r_curve;
    state->curves[2] = seqData->g_curve;
    state->curves[3] = seqData->b_curve;
    state->selectedPoint = selectedPointIndex;
    state->needsUpdate = isDirty;

    // Store state in sequence data
    PF_Handle stateH = suites.HandleSuite1()->host_new_handle(sizeof(SavedState));
    if (!stateH) {
        delete state;
        return PF_Err_OUT_OF_MEMORY;
    }

    void* stateP = suites.HandleSuite1()->host_lock_handle(stateH);
    if (!stateP) {
        suites.HandleSuite1()->host_dispose_handle(stateH);
        delete state;
        return PF_Err_OUT_OF_MEMORY;
    }

    memcpy(stateP, state, sizeof(SavedState));
    suites.HandleSuite1()->host_unlock_handle(stateH);
    delete state;

    // Store handle in sequence data
    seqData->cache = stateH;

    return err;
}

PF_Err StateManager::RestoreState(PF_InData* in_data) {
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Get sequence data
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData || !seqData->cache) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Get state from handle
    PF_Handle stateH = static_cast<PF_Handle>(seqData->cache);
    void* stateP = suites.HandleSuite1()->host_lock_handle(stateH);
    if (!stateP) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    SavedState* state = static_cast<SavedState*>(stateP);

    // Restore state
    seqData->rgb_curve = state->curves[0];
    seqData->r_curve = state->curves[1];
    seqData->g_curve = state->curves[2];
    seqData->b_curve = state->curves[3];
    selectedPointIndex = state->selectedPoint;
    isDirty = state->needsUpdate;

    suites.HandleSuite1()->host_unlock_handle(stateH);

    return err;
}