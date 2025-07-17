#include "diagnostics_tab.h"

#include <QPalette>

#include "forms/diagnostics_tab/ui_diagnostics_tab.h"

DiagnosticsTab::DiagnosticsTab(QWidget *parent) : QWidget(parent), ui(new Ui::DiagnosticsTab) {
    ui->setupUi(this);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    ui->scrollAreaWidgetContents->setPalette(palette);

    addTitle("Energie");
    addLineChart("Energievergleich",
                 "Zeit",
                 "Energie",
                 {"Gesamtenergie", "Kinetische Energie", "Potentielle Energie"});
    // addLineChart("Relative Abweichung der Gesamtenergie", "Zeit", "Relative Abweichung");
    addLineChart("Gesamtenergie", "Zeit", "Energie");
    addLineChart("Kinetische Energie", "Zeit", "Energie");
    addLineChart("Potentielle Energie", "Zeit", "Energie");
    addHLine();
    addTitle("Drehimpuls");
}

DiagnosticsTab::~DiagnosticsTab() { delete ui; }

void DiagnosticsTab::addTitle(QString title) {
    QLabel *label = new QLabel(title);

    QFont font = label->font();
    font.setFamilies({QString::fromUtf8("Open Sans")});
    font.setBold(true);
    font.setPointSize(14);
    label->setFont(font);

    insertWidget(label);
}

void DiagnosticsTab::addHLine() {
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(2);

    insertWidget(line);
}

void DiagnosticsTab::addLineChart(QString title,
                                  QString x_axis_title,
                                  QString y_axis_title,
                                  QVector<QString> data_titles,
                                  QVector<LabelType> label_types,
                                  QWidget *parent) {
    LineChartWidget *widget =
        new LineChartWidget(title, x_axis_title, y_axis_title, data_titles, label_types, parent);
    line_charts.push_back(widget);

    insertWidget(widget);
}

void DiagnosticsTab::insertWidget(QWidget *widget) {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->scrollAreaWidgetContents->layout());
    size_t i = layout->count() - 1;
    layout->insertWidget(i, widget);
}

void DiagnosticsTab::update(const DiagnosticsData &data) {
    for (auto *line_chart : line_charts) {
        QString title = line_chart->title();

        if (title == "Energievergleich") {
            line_chart->append(data.time, data.e_kin + data.e_pot, "Gesamtenergie");
            line_chart->append(data.time, data.e_kin, "Kinetische Energie");
            line_chart->append(data.time, data.e_pot, "Potentielle Energie");
        } else if (title == "Gesamtenergie") {
            double e_tot = data.e_kin + data.e_pot;
            line_chart->append(data.time, e_tot);
        } else if (title == "Kinetische Energie") {
            line_chart->append(data.time, data.e_kin);
        } else if (title == "Potentielle Energie") {
            line_chart->append(data.time, data.e_pot);
        }
    }
}
