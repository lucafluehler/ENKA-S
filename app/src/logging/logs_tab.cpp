#include "logs_tab.h"

#include <enkas/logging/logger.h>
#include <qnamespace.h>

#include <QBrush>
#include <QFontMetrics>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

LogsTab::LogsTab(QWidget* parent) : QWidget(parent) {
    log_font_ = QFont("Consolas", 10);
    setupUi();
}

void LogsTab::setupUi() {
    log_table_ = new QTableWidget(this);
    log_table_->setColumnCount(1);
    log_table_->setHorizontalHeaderLabels({"Message"});
    log_table_->setWordWrap(false);
    log_table_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    log_table_->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    log_table_->setWordWrap(false);
    log_table_->horizontalHeader()->setStretchLastSection(false);
    log_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    log_table_->verticalHeader()->hide();
    log_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    log_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    log_table_->setShowGrid(false);
    log_table_->setFont(log_font_);

    QFontMetrics fm(log_font_);
    log_table_->verticalHeader()->setDefaultSectionSize(fm.height());

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(log_table_);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

QColor LogsTab::getColorForLevel(enkas::logging::LogLevel level) const {
    switch (level) {
        case enkas::logging::LogLevel::TRACE:
            return Qt::gray;
        case enkas::logging::LogLevel::DEBUG:
            return Qt::blue;
        case enkas::logging::LogLevel::INFO:
            return Qt::black;
        case enkas::logging::LogLevel::WARNING:
            return QColor(255, 165, 0);  // Orange
        case enkas::logging::LogLevel::ERROR:
            return Qt::red;
        case enkas::logging::LogLevel::CRITICAL:
            return QColor(139, 0, 0);  // Dark Red
        default:
            return Qt::black;
    }
}

void LogsTab::addLogMessage(enkas::logging::LogLevel level, const QString& message) {
    log_table_->insertRow(0);

    auto* item = new QTableWidgetItem(message);
    item->setForeground(QBrush(getColorForLevel(level)));

    log_table_->setItem(0, 0, item);

    // Prune old entries to prevent high memory usage
    if (log_table_->rowCount() > max_log_entries_) {
        log_table_->removeRow(log_table_->rowCount() - 1);
    }
}
