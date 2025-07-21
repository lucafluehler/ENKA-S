#pragma once

#include <enkas/data/diagnostics.h>

#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts>
#include <functional>
#include <vector>

#include "core/snapshot.h"

class DiagnosticsWidget : public QWidget {
    Q_OBJECT

public:
    struct ChartDefinition {
        QString title;
        QString unit;
        std::function<double(DiagnosticsSnapshot&)> value_extractor;
    };

    explicit DiagnosticsWidget(QWidget* parent = nullptr);

    ~DiagnosticsWidget() override = default;

    void setupCharts(std::vector<ChartDefinition> chart_definitions, const QString& time_unit);

public slots:
    void updateData(DiagnosticsSnapshot& diagnostics);
    void fillCharts(const DiagnosticsSeries& series);

private:
    void createBaseUi();
    void clearCharts();

    QVBoxLayout* container_layout_;
    QScrollArea* scroll_area_;
    QWidget* scroll_area_widget_contents_;
    QVBoxLayout* charts_layout_;

    std::vector<ChartDefinition> definitions_;
    std::vector<QChart*> charts_;
    std::vector<QLineSeries*> series_;

    double max_time_ = 0.0;
    std::vector<double> min_values_;
    std::vector<double> max_values_;
};