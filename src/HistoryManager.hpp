#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>
#include <memory>

class HistoryManager {
public:
    static HistoryManager& getInstance() {
        static HistoryManager instance;
        return instance;
    }

    PF_Err SaveState(
        PF_InData* in_data,
        const CurveData* curves);

    PF_Err Undo(
        PF_InData* in_data,
        CurveData* curves);

    PF_Err Redo(
        PF_InData* in_data,
        CurveData* curves);

    bool CanUndo() const { return currentIndex > 0; }
    bool CanRedo() const { return currentIndex < history.size() - 1; }

private:
    struct HistoryState {
        CurveData curves[4];  // RGB, R, G, B curves
        A_long timestamp;
    };

    std::vector<HistoryState> history;
    size_t currentIndex{0};
    static const size_t MAX_HISTORY = 100;

    HistoryManager() = default;
    PF_Err CleanupOldStates();
};