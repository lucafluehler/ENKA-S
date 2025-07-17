#pragma once

#include <enkas/data/diagnostics.h>

#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QString>
#include <QVector>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "enkas/data/diagnostics.h"
#include "widgets/line_chart_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DiagnosticsTab;
}
QT_END_NAMESPACE

class DiagnosticsTab : public QWidget {
    Q_OBJECT

public:
    DiagnosticsTab(QWidget *parent = nullptr);
    ~DiagnosticsTab();

    void update(const enkas::data::Diagnostics &data);

private:
    void addTitle(QString title);
    void addHLine();
    void addLineChart(QString title,
                      QString x_axis_title,
                      QString y_axis_title,
                      QVector<QString> data_titles = QVector<QString>(),
                      QVector<LabelType> label_types = QVector<LabelType>(),
                      QWidget *parent = nullptr);
    void insertWidget(QWidget *widget);

    QVector<LineChartWidget *> line_charts;
    Ui::DiagnosticsTab *ui;
};
