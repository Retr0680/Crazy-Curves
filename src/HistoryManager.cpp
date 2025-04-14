#include "HistoryManager.hpp"

PF_Err HistoryManager::SaveState(
    PF_InData* in_data,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    // Remove any redo states
    if (currentIndex < history.size() - 1) {
        history.erase(history.begin() + currentIndex + 1, history.end());
    }

    // Create new history state
    HistoryState state;
    for (int i = 0; i < 4; i++) {
        state.curves[i] = curves[i];
    }
    state.timestamp = in_data->current_time;

    // Add to history
    history.push_back(state);
    currentIndex = history.size() - 1;

    // Cleanup if needed
    ERR(CleanupOldStates());

    return err;
}

PF_Err HistoryManager::Undo(
    PF_InData* in_data,
    CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    if (CanUndo()) {
        currentIndex--;
        const HistoryState& state = history[currentIndex];
        
        for (int i = 0; i < 4; i++) {
            curves[i] = state.curves[i];
            curves[i].dirty = true;
        }
    }

    return err;
}

PF_Err HistoryManager::Redo(
    PF_InData* in_data,
    CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    if (CanRedo()) {
        currentIndex++;
        const HistoryState& state = history[currentIndex];
        
        for (int i = 0; i < 4; i++) {
            curves[i] = state.curves[i];
            curves[i].dirty = true;
        }
    }

    return err;
}

PF_Err HistoryManager::CleanupOldStates()
{
    PF_Err err = PF_Err_NONE;

    // Keep history size under limit
    if (history.size() > MAX_HISTORY) {
        size_t numToRemove = history.size() - MAX_HISTORY;
        history.erase(history.begin(), history.begin() + numToRemove);
        currentIndex = std::max(0ul, currentIndex - numToRemove);
    }

    return err;
}