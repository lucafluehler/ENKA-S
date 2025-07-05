#ifndef LINECHARTWIDGET_H_
#define LINECHARTWIDGET_H_

#include <QGroupBox>
#include <QGroupBox>
#include <QChartView>
#include <QToolButton>
#include <QLabel>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QString>
#include <QLineSeries>
#include <QValueAxis>

enum class LabelType
{
    Mean,
    AbsMax,
    Last
};

class LineChartWidget : public QGroupBox
{
    Q_OBJECT

public:
    LineChartWidget( QString title
                   , QString x_axis_title
                   , QString y_axis_title
                   , QVector<QString> data_titles = QVector<QString>()
                   , QVector<LabelType> label_types = QVector<LabelType>()
                   , QWidget *parent = nullptr );

    void append(qreal x, qreal y, QString data_title = "");

private:
    void resizeAxes(double x, double y);

    QChartView *chart_view;
    QValueAxis *x_axis;
    QValueAxis *y_axis;

    double x_min = std::numeric_limits<double>::max();
    double x_max = -std::numeric_limits<double>::max();
    double y_min = std::numeric_limits<double>::max();
    double y_max = -std::numeric_limits<double>::max();

    struct LabelData
    {
        QLabel* label;
        LabelType label_type;
        QString data_title;
    };

    QVector<LabelData> labels;
    QMap<QString, QLineSeries*> data;
};

#endif // LINECHARTWIDGET_H_
