#pragma once
#include <vector>
#include <memory>
#include "CurveData.hpp"

class UndoRedoManager {
public:
    static const size_t MAX_HISTORY = 50;
    
    void pushState(const CurveData& state);
    bool canUndo() const;
    bool canRedo() const;
    CurveData undo();
    CurveData redo();
    void clear();

private:
    std::vector<CurveData> undoStack;
    std::vector<CurveData> redoStack;
};