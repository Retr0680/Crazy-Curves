#include "CurveHistory.hpp"

CurveHistory::CurveHistory() : currentIndex(0) {}

void CurveHistory::pushState(const CurveData& state) {
    // Clear redo stack when new state is pushed
    redoStack.clear();

    // Remove oldest states if history limit reached
    if (undoStack.size() >= MAX_HISTORY) {
        undoStack.erase(undoStack.begin());
    }

    undoStack.push_back(state);
    currentIndex = undoStack.size() - 1;
}

bool CurveHistory::canUndo() const {
    return currentIndex > 0;
}

bool CurveHistory::canRedo() const {
    return !redoStack.empty();
}

CurveData CurveHistory::undo() {
    if (!canUndo()) {
        return undoStack.front();
    }

    redoStack.push_back(undoStack[currentIndex]);
    currentIndex--;
    return undoStack[currentIndex];
}

CurveData CurveHistory::redo() {
    if (!canRedo()) {
        return undoStack[currentIndex];
    }

    currentIndex++;
    CurveData state = redoStack.back();
    redoStack.pop_back();
    undoStack.push_back(state);
    return state;
}

void CurveHistory::clear() {
    undoStack.clear();
    redoStack.clear();
    currentIndex = 0;
}