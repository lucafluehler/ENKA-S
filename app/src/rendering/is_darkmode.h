#pragma once

#include <QGuiApplication>
#include <QStyleHints>

inline bool isDarkMode() {
    auto styleHints = QGuiApplication::styleHints();
    if (styleHints) {
        bool is_dark = (styleHints->colorScheme() == Qt::ColorScheme::Dark);
        return is_dark;
    }
    return false;  // default to light mode if unknown
}
