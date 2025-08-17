#pragma once

#include <enkas/data/diagnostics.h>

#include <QScrollArea>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts>
#include <functional>
#include <vector>

#include "core/dataflow/snapshot.h"

/**
 * @brief A widget that displays diagnostic information as charts.
 */
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

    /**
     * @brief Sets up the charts based on the provided definitions.
     * @param chart_definitions The definitions for the charts to create.
     * @param time_unit The time unit to use for the x-axis.
     */
    void setupCharts(std::vector<ChartDefinition> chart_definitions, const QString& time_unit);

    /**
     * @brief Appends new diagnostic data to the charts.
     * @param diagnostics The new diagnostic data to display.
     */
    void updateData(const DiagnosticsSnapshot& diagnostics);

    /**
     * @brief Fills the charts with the provided diagnostic series.
     * @param series The diagnostic series to display.
     */
    void fillCharts(const DiagnosticsSeries& series);

private:
    void refreshCharts();
    void createBaseUi();
    void clearCharts();
    void resetDataCache();
    void resetChart(size_t index);
    std::vector<QVector<QPointF>> processSeries(const DiagnosticsSeries& series);
    void updateChart(size_t index, const QVector<QPointF>& points);
    static QPair<double, double> calculateYRange(double min_val, double max_val);

    QVBoxLayout* container_layout_;
    QScrollArea* scroll_area_;
    QWidget* scroll_area_widget_contents_;
    QVBoxLayout* charts_layout_;

    std::vector<ChartDefinition> definitions_;
    std::vector<QChart*> charts_;
    std::vector<QChartView*> chart_views_;
    std::vector<QLineSeries*> series_;

    std::vector<QValueAxis*> x_axes_;
    std::vector<QValueAxis*> y_axes_;

    double max_time_ = 0.0;
    std::vector<double> min_values_;
    std::vector<double> max_values_;

    std::vector<std::vector<QPointF>> full_data_;

    bool refresh_automatically_ = true;
};
