#pragma once

#include <QElapsedTimer>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QVector>

#include "core/blocking_queue.h"
#include "core/snapshot.h"
#include "i_simulation_window_view.h"
#include "rendering/render_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulationWindow;
}
QT_END_NAMESPACE

class SimulationWindow : public QMainWindow, ISimulationWindowView {
    Q_OBJECT

public:
    explicit SimulationWindow(QWidget *parent = nullptr);
    ~SimulationWindow() override;

    void initLiveMode(const std::shared_ptr<DataPtr> &data_ptr, double simulation_duration);

    void initFileMode(QString render_data_path = "",
                      QString diagnostics_data_path = "",
                      QString analytics_data_path = "");

    enum class Mode { Uninitialized, Live, File };
    Mode getMode() const;

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

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

    Mode mode;
    double simulation_duration;
    RenderSettings settings;

    QTimer *update_timer;
    bool update_in_progress;
    QElapsedTimer *renderer_timer;
    QTimer *movie_timer;

    Ui::SimulationWindow *ui_;
};
