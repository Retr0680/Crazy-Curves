#include "PresetManagerUI.h"
#include <algorithm>

PresetManagerUI::PresetManagerUI() {
    state.isPresetPanelOpen = false;
    state.selectedPresetIndex = -1;
    state.isRenamingPreset = false;
    state.showDeleteConfirmation = false;
}

PresetManagerUI::~PresetManagerUI() {}

PF_Err PresetManagerUI::draw(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[]) {
    PF_Err err = PF_Err_NONE;

    // Only draw if panel is open
    if (!state.isPresetPanelOpen) {
        return err;
    }

    // Calculate panel position (right side of effect window)
    PF_Point origin = {
        static_cast<A_short>(in_data->width - PANEL_WIDTH - 10),
        static_cast<A_short>(10)
    };

    // Draw main panel background
    PF_EffectWorld* world = out_data->effect_win;
    drawPresetPanel(world, &origin);

    // Draw preset list
    PF_Point listOrigin = {
        static_cast<A_short>(origin.h + 10),
        static_cast<A_short>(origin.v + 40)
    };
    drawPresetList(world, &listOrigin);

    // Draw preset controls
    PF_Point controlsOrigin = {
        static_cast<A_short>(origin.h + 10),
        static_cast<A_short>(origin.v + PANEL_HEIGHT - 60)
    };
    drawPresetControls(world, &controlsOrigin);

    // Draw delete confirmation if needed
    if (state.showDeleteConfirmation) {
        drawDeleteConfirmation(world, &origin);
    }

    return err;
}

void PresetManagerUI::drawPresetPanel(PF_EffectWorld* world, const PF_Point* origin) {
    // Draw panel background
    PF_Pixel bgColor = {32, 32, 32, 255};
    PF_Rect panelRect = {
        origin->v,
        origin->h,
        static_cast<A_short>(origin->v + PANEL_HEIGHT),
        static_cast<A_short>(origin->h + PANEL_WIDTH)
    };
    
    // Draw rectangle for panel background
    for (int y = panelRect.top; y < panelRect.bottom; y++) {
        PF_Pixel* row = (PF_Pixel*)((char*)world->data + y * world->rowbytes);
        for (int x = panelRect.left; x < panelRect.right; x++) {
            row[x] = bgColor;
        }
    }

    // Draw panel header
    PF_Pixel headerColor = {64, 64, 64, 255};
    PF_Rect headerRect = {
        origin->v,
        origin->h,
        static_cast<A_short>(origin->v + 30),
        static_cast<A_short>(origin->h + PANEL_WIDTH)
    };

    // Draw header background
    for (int y = headerRect.top; y < headerRect.bottom; y++) {
        PF_Pixel* row = (PF_Pixel*)((char*)world->data + y * world->rowbytes);
        for (int x = headerRect.left; x < headerRect.right; x++) {
            row[x] = headerColor;
        }
    }
}

void PresetManagerUI::drawPresetList(PF_EffectWorld* world, const PF_Point* origin) {
    // Get list of presets
    std::vector<std::string> presetNames = CurvePresets::getCustomPresetNames();
    
    // Add built-in presets
    std::vector<std::string> builtInPresets = {
        "Linear",
        "S-Curve",
        "Contrast",
        "Negative",
        "Solarize",
        "Vintage"
    };
    
    presetNames.insert(presetNames.begin(), builtInPresets.begin(), builtInPresets.end());

    // Draw each preset item
    PF_Point itemOrigin = *origin;
    for (size_t i = 0; i < presetNames.size(); i++) {
        bool isSelected = (static_cast<int>(i) == state.selectedPresetIndex);
        
        // Draw preset item background
        PF_Pixel bgColor = isSelected ? 
            PF_Pixel{64, 96, 128, 255} : 
            PF_Pixel{48, 48, 48, 255};

        PF_Rect itemRect = {
            itemOrigin.v,
            itemOrigin.h,
            static_cast<A_short>(itemOrigin.v + PRESET_ITEM_HEIGHT),
            static_cast<A_short>(itemOrigin.h + PANEL_WIDTH - 20)
        };

        // Draw item background
        for (int y = itemRect.top; y < itemRect.bottom; y++) {
            PF_Pixel* row = (PF_Pixel*)((char*)world->data + y * world->rowbytes);
            for (int x = itemRect.left; x < itemRect.right; x++) {
                row[x] = bgColor;
            }
        }

        itemOrigin.v += PRESET_ITEM_HEIGHT;
    }
}

void PresetManagerUI::drawPresetControls(PF_EffectWorld* world, const PF_Point* origin) {
    // Draw control buttons
    PF_Point buttonOrigin = *origin;
    
    // New Preset button
    drawButton(world, &buttonOrigin, "New Preset", false);
    buttonOrigin.v += BUTTON_HEIGHT + 5;
    
    // Delete Preset button (only enabled if a preset is selected)
    drawButton(world, &buttonOrigin, "Delete Preset", 
               state.selectedPresetIndex >= 0);
}

void PresetManagerUI::drawButton(PF_EffectWorld* world, const PF_Point* origin,
                               const std::string& label, bool isEnabled) {
    PF_Pixel buttonColor = isEnabled ? 
        PF_Pixel{64, 96, 128, 255} : 
        PF_Pixel{48, 48, 48, 255};

    PF_Rect buttonRect = {
        origin->v,
        origin->h,
        static_cast<A_short>(origin->v + BUTTON_HEIGHT),
        static_cast<A_short>(origin->h + PANEL_WIDTH - 20)
    };

    // Draw button background
    for (int y = buttonRect.top; y < buttonRect.bottom; y++) {
        PF_Pixel* row = (PF_Pixel*)((char*)world->data + y * world->rowbytes);
        for (int x = buttonRect.left; x < buttonRect.right; x++) {
            row[x] = buttonColor;
        }
    }
}

PF_Err PresetManagerUI::handleEvent(PF_InData* in_data, PF_OutData* out_data,
                                  PF_ParamDef* params[], PF_EventExtra* event) {
    PF_Err err = PF_Err_NONE;

    switch (event->e_type) {
        case PF_Event_MOUSE_DOWN:
            // Handle mouse clicks
            if (state.isPresetPanelOpen) {
                PF_Point mouse = {event->u.do_click.screen_point.v,
                                event->u.do_click.screen_point.h};
                
                // Check if click is within preset list
                if (mouse.h >= in_data->width - PANEL_WIDTH - 10 &&
                    mouse.h <= in_data->width - 10 &&
                    mouse.v >= 50 &&
                    mouse.v <= 50 + PANEL_HEIGHT - 100) {
                    
                    // Calculate selected preset index
                    int clickedIndex = (mouse.v - 50) / PRESET_ITEM_HEIGHT;
                    if (clickedIndex >= 0 && 
                        clickedIndex < static_cast<int>(CurvePresets::getCustomPresetNames().size())) {
                        state.selectedPresetIndex = clickedIndex;
                        out_data->refresh_flag = true;
                    }
                }
            }
            break;

        case PF_Event_MOUSE_UP:
            // Handle button clicks
            break;

        default:
            break;
    }

    return err;
}

void PresetManagerUI::loadPreset(const std::string& presetName) {
    // Convert preset name to enum type and load
    CurvePresets::PresetType type;
    if (presetName == "Linear") type = CurvePresets::PresetType::LINEAR;
    else if (presetName == "S-Curve") type = CurvePresets::PresetType::S_CURVE;
    else if (presetName == "Contrast") type = CurvePresets::PresetType::CONTRAST;
    else if (presetName == "Negative") type = CurvePresets::PresetType::NEGATIVE;
    else if (presetName == "Solarize") type = CurvePresets::PresetType::SOLARIZE;
    else if (presetName == "Vintage") type = CurvePresets::PresetType::VINTAGE;
    else type = CurvePresets::PresetType::CUSTOM;
}

void PresetManagerUI::savePreset(const std::string& presetName, const CurveData& curveData) {
    CurvePresets::saveCustomPreset(presetName, curveData);
}

void PresetManagerUI::deletePreset(const std::string& presetName) {
    // Only allow deletion of custom presets
    std::vector<std::string> customPresets = CurvePresets::getCustomPresetNames();
    if (std::find(customPresets.begin(), customPresets.end(), presetName) != customPresets.end()) {
        // Remove from custom presets
        // Note: Implementation of preset deletion would go here
    }
}

void PresetManagerUI::renamePreset(const std::string& oldName, const std::string& newName) {
    // Only allow renaming of custom presets
    std::vector<std::string> customPresets = CurvePresets::getCustomPresetNames();
    if (std::find(customPresets.begin(), customPresets.end(), oldName) != customPresets.end()) {
        // Rename preset
        // Note: Implementation of preset renaming would go here
    }
}