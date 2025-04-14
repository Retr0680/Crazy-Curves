#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <memory>

class StateManager {
public:
    static StateManager& getInstance() {
        static StateManager instance;
        return instance;
    }

    PF_Err Initialize(PF_InData* in_data);
    PF_Err Shutdown();
    
    void SetCurrentCurve(CurveData* curve) { currentCurve = curve; }
    CurveData* GetCurrentCurve() const { return currentCurve; }
    
    void SetSelectedPoint(A_long index) { selectedPointIndex = index; }
    A_long GetSelectedPoint() const { return selectedPointIndex; }
    
    PF_Boolean IsDirty() const { return isDirty; }
    void SetDirty(PF_Boolean dirty) { isDirty = dirty; }

    PF_Err SaveState(PF_InData* in_data);
    PF_Err RestoreState(PF_InData* in_data);

private:
    StateManager() : 
        currentCurve(nullptr),
        selectedPointIndex(-1),
        isDirty(false) {}

    CurveData* currentCurve;
    A_long selectedPointIndex;
    PF_Boolean isDirty;
    
    struct SavedState {
        CurveData curves[4];
        A_long selectedPoint;
        PF_Boolean needsUpdate;
    };
};