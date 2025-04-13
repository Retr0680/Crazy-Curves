#pragma once
#include "ThemeManager.hpp"

ThemeManager::ThemeManager() {
    initializeDefaultThemes();
    loadTheme("Default");
}

void ThemeManager::initializeDefaultThemes() {
    // Default dark theme
    Theme darkTheme{
        Color(32, 32, 32),     // backgroundColor
        Color(64, 64, 64),     // gridColor
        Color(255, 255, 255),  // curveColor
        Color(128, 128, 255),  // pointColor
        Color(255, 128, 128),  // selectedPointColor
        Color(200, 200, 200),  // textColor
        Color(48, 48, 48),     // borderColor
        "Dark"
    };
    themes["Dark"] = darkTheme;

    // Light theme
    Theme lightTheme{
        Color(240, 240, 240),  // backgroundColor
        Color(200, 200, 200),  // gridColor
        Color(0, 0, 0),        // curveColor
        Color(0, 0, 255),      // pointColor
        Color(255, 0, 0),      // selectedPointColor
        Color(32, 32, 32),     // textColor
        Color(180, 180, 180),  // borderColor
        "Light"
    };
    themes["Light"] = lightTheme;
}

void ThemeManager::loadTheme(const std::string& themeName) {
    auto it = themes.find(themeName);
    if (it != themes.end()) {
        currentTheme = it->second;
    }
}

void ThemeManager::addTheme(const std::string& name, const Theme& theme) {
    themes[name] = theme;
}