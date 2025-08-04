#pragma once

#include <QElapsedTimer>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QVector>

#include "core/dataflow/snapshot.h"
#include "i_simulation_window_view.h"
#include "rendering/render_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulationWindow;
}
QT_END_NAMESPACE

class SimulationWindow : public QMainWindow, virtual public ISimulationWindowView {
    Q_OBJECT

public:
    explicit SimulationWindow(QWidget *parent = nullptr);
    ~SimulationWindow() override = default;

    void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                               double simulation_duration) override;
    void updateFPS(int fps) override;
    int getTargetFPS() const override { return target_fps_; }

signals:
    void windowClosed();
    void fpsChanged();

public slots:
    void onFpsChanged(int fps) {
        target_fps_ = fps;
        emit fpsChanged();
    }

protected:
    void closeEvent(QCloseEvent *event) override;

    virtual bool updateProgress() const { return true; }

    Ui::SimulationWindow *ui_;

private slots:
    void saveSettings();
    void toggleSidebar();
    void toggleSettings();
    void toggleMovie(bool checked);

private:
    void setupCharts();

    RenderSettings settings_;
    int target_fps_ = 120;  // Target frames per second for the simulation

    QTimer *movie_timer;  // Used for recording the simulation
};
