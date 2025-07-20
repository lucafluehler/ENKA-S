#pragma once

#include <enkas/logging/logger.h>

#include <QColor>
#include <QFont>
#include <QTableWidget>
#include <QWidget>

/**
 * @brief LogsTab displays log messages in a table format.
 *
 * It supports different log levels with color coding and has a maximum number of entries.
 */
class LogsTab : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Sets up the UI styling and initializes the log table.
     * @param parent The parent widget.
     */
    explicit LogsTab(QWidget* parent = nullptr);

public slots:
    /**
     * @brief Adds a log message to the table with the appropriate color based on its level.
     * @param level The log level of the message.
     * @param message The log message to display.
     */
    void addLogMessage(enkas::logging::LogLevel level, const QString& message);

private:
    void setupUi();
    QColor getColorForLevel(enkas::logging::LogLevel level) const;

    QTableWidget* log_table_;
    QFont log_font_;
    const int max_log_entries_ = 500;
};
