#include "diagnostics_widget.h"

#include <enkas/data/diagnostics.h>

#include <QPainter>
#include <QScrollArea>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>
#include <QtCharts>
#include <functional>
#include <vector>

#include "core/snapshot.h"

DiagnosticsWidget::DiagnosticsWidget(QWidget* parent)
    : QWidget(parent), refresh_timer_(new QTimer(this)) {
    createBaseUi();
    connect(refresh_timer_, &QTimer::timeout, this, &DiagnosticsWidget::refreshCharts);
    refresh_timer_->start(1000 / 5);  // Refresh rate: 5 Hz
}

void DiagnosticsWidget::createBaseUi() {
    this->setFixedWidth(600);

    container_layout_ = new QVBoxLayout(this);
    container_layout_->setContentsMargins(0, 0, 0, 0);

    scroll_area_ = new QScrollArea(this);
    scroll_area_->setWidgetResizable(true);
    scroll_area_->setFrameShape(QFrame::NoFrame);

    scroll_area_widget_contents_ = new QWidget();
    charts_layout_ = new QVBoxLayout(scroll_area_widget_contents_);

    scroll_area_->setWidget(scroll_area_widget_contents_);
    container_layout_->addWidget(scroll_area_);
}

void DiagnosticsWidget::clearCharts() {
    QLayoutItem* item;
    while ((item = charts_layout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    charts_.clear();
    chart_views_.clear();
    series_.clear();
    x_axes_.clear();
    y_axes_.clear();
    definitions_.clear();
    min_values_.clear();
    max_values_.clear();
    max_time_ = 0.0;
}

void DiagnosticsWidget::setupCharts(std::vector<ChartDefinition> chart_definitions,
                                    const QString& time_unit) {
    clearCharts();

    definitions_ = std::move(chart_definitions);

    min_values_.resize(definitions_.size(), std::numeric_limits<double>::max());
    max_values_.resize(definitions_.size(), std::numeric_limits<double>::lowest());

    for (const auto& def : definitions_) {
        auto series = new QLineSeries();

        // Thicker, dark red line
        QPen pen;
        pen.setColor(QColor(139, 0, 0));
        pen.setWidth(2);
        series->setPen(pen);

        auto chart = new QChart();

        // Add big, bold title
        chart->setTitle(def.title);
        QFont titleFont;
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        chart->setTitleFont(titleFont);

        chart->addSeries(series);
        chart->legend()->hide();

        // Configure X Axis
        auto axisX = new QValueAxis();
        axisX->setTitleText("Time / " + time_unit);
        axisX->setLabelFormat("%.2f");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        // Configure Y Axis
        auto axisY = new QValueAxis();
        axisY->setTitleText(def.title + " / " + def.unit);
        axisY->setLabelFormat("%.2e");
        axisY->setTickCount(10);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        // Configure chart appearance
        auto chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QChartView::HorizontalRubberBand);
        chartView->setMinimumHeight(400);

        charts_layout_->addWidget(chartView);

        charts_.push_back(chart);
        series_.push_back(series);
        chart_views_.push_back(chartView);
        x_axes_.push_back(axisX);
        y_axes_.push_back(axisY);
    }
}

void DiagnosticsWidget::updateData(DiagnosticsSnapshot& diag) {
    if (definitions_.empty()) {
        return;
    }

    const double timestamp = diag.time;
    max_time_ = std::max(max_time_, timestamp);

    for (size_t i = 0; i < definitions_.size(); ++i) {
        const double value = definitions_[i].value_extractor(diag);

        series_[i]->append(timestamp, value);
        min_values_[i] = std::min(min_values_[i], value);
        max_values_[i] = std::max(max_values_[i], value);
    }
}

void DiagnosticsWidget::fillCharts(const DiagnosticsSeries& series) {
    if (definitions_.empty()) {
        return;
    }

    max_time_ = 0.0;
    std::fill(min_values_.begin(), min_values_.end(), std::numeric_limits<double>::max());
    std::fill(max_values_.begin(), max_values_.end(), std::numeric_limits<double>::lowest());

    // Clear the series if the input series is empty
    if (series.empty()) {
        for (size_t i = 0; i < charts_.size(); ++i) {
            series_[i]->clear();
            auto* chart = charts_[i];
            auto* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
            auto* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
            if (axisX && axisY) {
                axisX->setRange(0, 1.0);
                axisY->setRange(0, 1.0);
            }
        }
        return;
    }

    // Prepare a list of points for each series so we can update them in one go
    std::vector<QList<QPointF>> all_points(definitions_.size());

    for (const auto& diag_point : series) {
        max_time_ = std::max(max_time_, diag_point.time);

        for (size_t i = 0; i < definitions_.size(); ++i) {
            const double value = definitions_[i].value_extractor(diag_point);

            min_values_[i] = std::min(min_values_[i], value);
            max_values_[i] = std::max(max_values_[i], value);

            all_points[i].append(QPointF(diag_point.time, value));
        }
    }

    for (size_t i = 0; i < definitions_.size(); ++i) {
        series_[i]->replace(all_points[i]);

        auto* chart = charts_[i];
        auto* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        auto* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

        if (axisX && axisY) {
            axisX->setRange(0, std::max(max_time_, 1e-9));

            const double y_margin = (max_values_[i] - min_values_[i]) * 0.05;
            axisY->setRange(min_values_[i] - y_margin, max_values_[i] + y_margin);

            if (qFuzzyCompare(axisY->min(), axisY->max())) {
                axisY->setRange(axisY->min() - 1.0, axisY->max() + 1.0);
            }
        }
    }
}

void DiagnosticsWidget::refreshCharts() {
    if (definitions_.empty()) {
        return;
    }

    const QRect viewport_rect = scroll_area_->viewport()->rect();

    for (size_t i = 0; i < definitions_.size(); ++i) {
        // Check if the chart is visible in the viewport
        auto* chart_view = chart_views_[i];
        const QRect chart_rect = chart_view->geometry();
        if (!viewport_rect.intersects(chart_rect)) {
            continue;  // Skip the update for this invisible chart
        }

        // The chart is visible, update its axes
        auto* axisX = x_axes_[i];
        auto* axisY = y_axes_[i];

        axisX->setRange(0, std::max(max_time_, 1e-9));

        const double y_margin = (max_values_[i] - min_values_[i]) * 0.05;
        axisY->setRange(min_values_[i] - y_margin, max_values_[i] + y_margin);

        if (qFuzzyCompare(axisY->min(), axisY->max())) {
            axisY->setRange(axisY->min() - 1.0, axisY->max() + 1.0);
        }
    }
}
