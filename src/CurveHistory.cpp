#include "CurveHistory.hpp"

void CurveHistory::pushState(const CurveData& curveData) {
    if (undoStack.size() >= MAX_HISTORY) {
        undoStack.erase(undoStack.begin());
    }
    
    undoStack.push_back(curveData);
    redoStack.clear();
}

bool CurveHistory::canUndo() const {
    return !undoStack.empty();
}

bool CurveHistory::canRedo() const {
    return !redoStack.empty();
}

CurveData CurveHistory::undo() {
    if (!canUndo()) {
        return CurveData();
    }
    
    CurveData current = undoStack.back();
    undoStack.pop_back();
    redoStack.push_back(current);
    return undoStack.empty() ? CurveData() : undoStack.back();
}

CurveData CurveHistory::redo() {
    if (!canRedo()) {
        return CurveData();
    }
    
    CurveData current = redoStack.back();
    redoStack.pop_back();
    undoStack.push_back(current);
    return current;
}

void CurveHistory::clear() {
    undoStack.clear();
    redoStack.clear();
}