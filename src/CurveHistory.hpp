#pragma once
#include <vector>
#include "CurveData.hpp"

class CurveHistory {
public:
    static const int MAX_HISTORY = 20;
    
    void pushState(const CurveData& curveData);
    bool canUndo() const;
    bool canRedo() const;
    CurveData undo();
    CurveData redo();
    void clear();

private:
    std::vector<CurveData> undoStack;
    std::vector<CurveData> redoStack;
};