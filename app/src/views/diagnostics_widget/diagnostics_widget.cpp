#include "diagnostics_widget.h"

#include <enkas/data/diagnostics.h>

#include <QPainter>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>
#include <QtCharts>
#include <functional>
#include <vector>

#include "core/snapshot.h"

DiagnosticsWidget::DiagnosticsWidget(QWidget* parent) : QWidget(parent) { createBaseUi(); }

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
    series_.clear();
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

        auto chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QChartView::HorizontalRubberBand);
        chartView->setMinimumHeight(400);

        charts_layout_->addWidget(chartView);

        charts_.push_back(chart);
        series_.push_back(series);
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

        auto* chart = charts_[i];
        auto* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        auto* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

        if (axisX && axisY) {
            axisX->setRange(0, max_time_);

            min_values_[i] = std::min(min_values_[i], value);
            max_values_[i] = std::max(max_values_[i], value);

            const double y_margin = (max_values_[i] - min_values_[i]) * 0.05;
            axisY->setRange(min_values_[i] - y_margin, max_values_[i] + y_margin);

            if (qFuzzyCompare(axisY->min(), axisY->max())) {
                axisY->setRange(axisY->min() - 1.0, axisY->max() + 1.0);
            }
        }
    }
}
