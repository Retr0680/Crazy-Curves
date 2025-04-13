#pragma once
#include <string>
#include <map>
#include "DrawingUtils.hpp"

struct Theme {
    Color backgroundColor;
    Color gridColor;
    Color curveColor;
    Color pointColor;
    Color selectedPointColor;
    Color textColor;
    Color borderColor;
    std::string name;
};

class ThemeManager {
public:
    static ThemeManager& getInstance() {
        static ThemeManager instance;
        return instance;
    }

    void loadTheme(const std::string& themeName);
    const Theme& getCurrentTheme() const { return currentTheme; }
    void addTheme(const std::string& name, const Theme& theme);

private:
    ThemeManager();
    std::map<std::string, Theme> themes;
    Theme currentTheme;
    void initializeDefaultThemes();
};