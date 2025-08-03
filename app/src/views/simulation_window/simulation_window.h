#pragma once

#include <QElapsedTimer>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QVector>
#include <memory>

#include "core/dataflow/debug_info.h"
#include "core/dataflow/snapshot.h"
#include "i_simulation_window_view.h"
#include "rendering/render_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulationWindow;
}
QT_END_NAMESPACE

/**
 * @brief The SimulationWindow class provides the main window for the simulation,
 * allowing users to visualize and interact with particle systems.
 */
class SimulationWindow : public QMainWindow, public ISimulationWindowView {
    Q_OBJECT

public:
    /**
     * @brief Initializes the UI and sets up the simulation window.
     * @param parent The parent widget for this window.
     */
    explicit SimulationWindow(QWidget *parent = nullptr);
    ~SimulationWindow() override = default;

    void initLiveMode(std::shared_ptr<LiveDebugInfo> debug_info) override;
    void initReplayMode(std::shared_ptr<std::vector<double>> timestamps,
                        std::shared_ptr<DiagnosticsSeries> diagnostics_series) override;

    void updateSystemRendering(SystemSnapshotPtr system_snapshot) override;
    void updateDebugInfo(int fps, int sps) override;

    void updateDiagnostics(DiagnosticsSnapshotPtr diagnostics_snapshot) override;
    void fillCharts(const DiagnosticsSeries &series) override;

    int getTargetFPS() const override { return target_fps_; }

    void updateBufferValue(int buffer_value);

signals:
    /** @signal
     * @brief Emitted when the simulation window is closed.
     */
    void windowClosed();

    /** @signal
     * @brief Emitted when the play / pause button is prressed.
     */
    void togglePlayback();

    /** @signal
     * @brief Emitted when the step forward button is pressed.
     */
    void stepForward();

    /** @signal
     * @brief Emitted when the step backward button is pressed.
     */
    void stepBackward();

    /** @signal
     * @brief Emitted when the frames per second (FPS) changes.
     */
    void fpsChanged();

    /** @signal
     * @brief Emitted when the steps per second changes.
     */
    void stepsPerSecondChanged(int sps);

public slots:
    /**
     * @brief Updates the target frames per second for the simulation.
     * @param fps The new target FPS.
     */
    void onFpsChanged(int fps) {
        target_fps_ = fps;
        emit fpsChanged();
    }

private slots:
    void saveSettings();
    void toggleSidebar();
    void toggleSettings();
    void toggleDebugInfo();
    void toggleMovie(bool checked);
    void onTogglePlayback();
    void onStepsPerSecondChanged(int sps);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupCharts();

    RenderSettings settings_;

    QTimer *movie_timer;  // Used for recording the simulation

    Ui::SimulationWindow *ui_;

    double simulation_duration_ = 0.0;  // Total duration of the simulation
    int target_fps_ = 120;              // Target frames per second for the simulation

    // Stored data for live mode
    std::shared_ptr<LiveDebugInfo> live_debug_info_ = nullptr;

    // Stored data for replay mode
    std::shared_ptr<std::vector<double>> timestamps_ = nullptr;
    bool playback_active_ = true;  // Indicates if the playback is active
};
