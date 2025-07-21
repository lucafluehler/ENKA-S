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

    void initLiveMode() override;

    void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                               double simulation_duration) override;
    void updateCharts(DiagnosticsSnapshotPtr diagnostics_snapshot) override;
    void updateFPS(int fps) override;

private slots:
    void saveSettings();
    void toggleSidebar();
    void toggleSettings();
    void toggleMovie(bool checked);

private:
    void setupCharts();

    RenderSettings settings_;

    QTimer *movie_timer;  // Used for recording the simulation

    Ui::SimulationWindow *ui_;
};
