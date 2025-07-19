#pragma once

#include <enkas/logging/logger.h>

#include <QColor>
#include <QFont>
#include <QTableWidget>
#include <QWidget>

class LogsTab : public QWidget {
    Q_OBJECT

public:
    explicit LogsTab(QWidget* parent = nullptr);

public slots:
    void addLogMessage(enkas::logging::LogLevel level, const QString& message);

private:
    void setupUi();
    QColor getColorForLevel(enkas::logging::LogLevel level) const;

    QTableWidget* log_table_;
    QFont log_font_;
    const int max_log_entries_ = 500;
};
