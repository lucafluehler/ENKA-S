#include <QVBoxLayout>
#include "line_chart_widget.h"

LineChartWidget::LineChartWidget( QString title
                                , QString x_axis_title
                                , QString y_axis_title
                                , QVector<QString> data_titles
                                , QVector<LabelType> label_types
                                , QWidget *parent)
    : QGroupBox(title, parent)
{
    QFont font;
    font.setBold(true);
    setFont(font);

    QVBoxLayout *group_layout = new QVBoxLayout;
    group_layout->setSpacing(14);

    // Add chart to group layout
    QChart *chart = new QChart;

    x_axis = new QValueAxis;
    y_axis = new QValueAxis;
    x_axis->setTitleText(x_axis_title);
    y_axis->setTitleText(y_axis_title);
    chart->addAxis(x_axis, Qt::AlignBottom);
    chart->addAxis(y_axis, Qt::AlignLeft);

    if (data_titles.isEmpty()) {
        data_titles.push_back("smellySecretDataName");
        chart->legend()->hide();
    }

    for (const auto& data_title : data_titles) {
        QLineSeries *series = new QLineSeries;
        chart->addSeries(series);
        series->setName(data_title);
        series->attachAxis(x_axis);
        series->attachAxis(y_axis);
        data[data_title] = series;
    }

    chart_view = new QChartView(chart);
    chart_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    chart_view->setFixedHeight(400);
    chart_view->setRenderHint(QPainter::Antialiasing);
    group_layout->addWidget(chart_view);

    // OPTIONAL: add labels to group layout
    if (!label_types.empty()) {
        // TODO
    }

    setLayout(group_layout);
}

void LineChartWidget::append(qreal x, qreal y, QString data_title)
{
    if (data_title.isEmpty()) data_title = "smellySecretDataName";

    if (!data.contains(data_title)) return;

    data.value(data_title)->append(x, y);
    resizeAxes(x, y);
    chart_view->update();
}

void LineChartWidget::resizeAxes(double x, double y)
{
    x_min = std::min(x_min, x);
    x_max = std::max(x_max, x);
    y_min = std::min(y_min, y);
    y_max = std::max(y_max, y);

    x_axis->setRange(x_min, x_max);
    y_axis->setRange(y_min, y_max);
}
