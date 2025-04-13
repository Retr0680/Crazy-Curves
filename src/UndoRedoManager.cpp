#include "UndoRedoManager.hpp"

void UndoRedoManager::pushState(const CurveData& state) {
    if (undoStack.size() >= MAX_HISTORY) {
        undoStack.erase(undoStack.begin());
    }
    
    undoStack.push_back(state);
    redoStack.clear();
}

bool UndoRedoManager::canUndo() const {
    return undoStack.size() > 1;  // Keep at least one state
}

bool UndoRedoManager::canRedo() const {
    return !redoStack.empty();
}

CurveData UndoRedoManager::undo() {
    if (!canUndo()) {
        return undoStack.front();
    }
    
    redoStack.push_back(undoStack.back());
    undoStack.pop_back();
    return undoStack.back();
}

CurveData UndoRedoManager::redo() {
    if (!canRedo()) {
        return undoStack.back();
    }
    
    undoStack.push_back(redoStack.back());
    redoStack.pop_back();
    return undoStack.back();
}

void UndoRedoManager::clear() {
    undoStack.clear();
    redoStack.clear();
}