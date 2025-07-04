#ifndef SIMULATION_WINDOW_H_
#define SIMULATION_WINDOW_H_

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QTimer>
#include <QElapsedTimer>

#include "render_settings.h"
#include "data_ptr.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SimulationWindow; }
QT_END_NAMESPACE

class SimulationWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimulationWindow(QWidget *parent = nullptr);

    void initLiveMode( const std::shared_ptr<DataPtr>& data_ptr
                     , double simulation_duration);

    void initFileMode( QString render_data_path = ""
                     , QString diagnostics_data_path = ""
                     , QString analytics_data_path = "" );

    enum class Mode {Uninitialized, Live, File};
    Mode getMode() const;

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

public slots:
    void renderDataUpdate();
    void diagnosticsDataUpdate();
    void analyticsDataUpdate();

private slots:
    void saveSettings();
    void toggleSidebar();
    void toggleSettings();
    void toggleMovie(bool checked);
    void update();

private:
    double getSimulationDurationFromFile() const;
    RenderData getRenderDataFromFile() const;
    DiagnosticsData getDiagnosticsDataFromFile() const;
    AnalyticsData getAnalyticsDataFromFile() const;

    Mode mode;
    double simulation_duration;
    RenderSettings settings;

    std::shared_ptr<DataPtr> data_ptr;

    QString render_data_path;
    QString diagnostics_data_path;
    QString analytics_data_path;
    size_t render_time_idx;
    QVector<double> render_times;

    QTimer *update_timer;
    bool update_in_progress;
    QElapsedTimer *renderer_timer;
    QTimer *movie_timer;

    Ui::SimulationWindow *ui;
};

#endif // SIMULATION_WINDOW_H_
