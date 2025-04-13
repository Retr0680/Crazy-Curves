#pragma once

#include "AEConfig.h"
#include "AE_Effect.h"
#include "AE_EffectUI.h"
#include "AE_EffectCB.h"
#include "CurvesData.hpp"
#include "CurvePresets.hpp"

class PresetManagerUI {
public:
    PresetManagerUI();
    ~PresetManagerUI();

    // UI State
    struct UIState {
        bool isPresetPanelOpen;
        int selectedPresetIndex;
        std::string newPresetName;
        bool isRenamingPreset;
        bool showDeleteConfirmation;
    };

    // Main UI methods
    PF_Err draw(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[]);
    PF_Err handleEvent(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_EventExtra* event);

    // Preset operations
    void loadPreset(const std::string& presetName);
    void savePreset(const std::string& presetName, const CurveData& curveData);
    void deletePreset(const std::string& presetName);
    void renamePreset(const std::string& oldName, const std::string& newName);

private:
    // UI Components
    void drawPresetPanel(PF_EffectWorld* world, const PF_Point* origin);
    void drawPresetList(PF_EffectWorld* world, const PF_Point* origin);
    void drawPresetControls(PF_EffectWorld* world, const PF_Point* origin);
    void drawDeleteConfirmation(PF_EffectWorld* world, const PF_Point* origin);

    // UI Helper methods
    void drawButton(PF_EffectWorld* world, const PF_Point* origin, 
                   const std::string& label, bool isSelected);
    void drawTextInput(PF_EffectWorld* world, const PF_Point* origin, 
                      std::string& text, bool isEditing);

    // UI Constants
    static const int PANEL_WIDTH = 200;
    static const int PANEL_HEIGHT = 300;
    static const int BUTTON_HEIGHT = 25;
    static const int PRESET_ITEM_HEIGHT = 30;

    UIState state;
};