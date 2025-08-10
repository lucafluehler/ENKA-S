#pragma once

#include <QGuiApplication>
#include <QStyleHints>

/**
 * @brief Check if the application is in dark mode.
 * @return true if dark mode is enabled, false otherwise.
 */
inline bool isDarkMode() {
    auto styleHints = QGuiApplication::styleHints();
    if (styleHints) {
        bool is_dark = (styleHints->colorScheme() == Qt::ColorScheme::Dark);
        return is_dark;
    }
    return false;  // default to light mode if unknown
}
