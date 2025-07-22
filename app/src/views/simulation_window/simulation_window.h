#pragma once

#include <QElapsedTimer>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QVector>

#include "core/snapshot.h"
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

    void initLiveMode(double simulation_duration) override;
    void initReplayMode(std::shared_ptr<std::vector<double>> timestamps,
                        std::shared_ptr<DiagnosticsSeries> diagnostics_series) override;

    void updateSystemRendering(SystemSnapshotPtr system_snapshot) override;
    void updateDebugInfo(int fps, int sps) override;

    void updateCharts(DiagnosticsSnapshotPtr diagnostics_snapshot) override;
    void fillCharts(const DiagnosticsSeries &series) override;

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

private slots:
    void saveSettings();
    void toggleSidebar();
    void toggleSettings();
    void toggleMovie(bool checked);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupCharts();

    RenderSettings settings_;

    QTimer *movie_timer;  // Used for recording the simulation

    Ui::SimulationWindow *ui_;

    // Stored data for live mode
    double simulation_duration_ = 0.0;

    // Stored data for replay mode
    std::shared_ptr<std::vector<double>> timestamps_ = nullptr;
};
