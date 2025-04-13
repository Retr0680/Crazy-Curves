#pragma once
#include <vector>
#include "CurveData.hpp"

class CurveHistory {
public:
    static const size_t MAX_HISTORY = 50;

    CurveHistory();
    void pushState(const CurveData& state);
    bool canUndo() const;
    bool canRedo() const;
    CurveData undo();
    CurveData redo();
    void clear();

private:
    std::vector<CurveData> undoStack;
    std::vector<CurveData> redoStack;
    size_t currentIndex;
};