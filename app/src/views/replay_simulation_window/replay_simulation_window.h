#pragma once

#include "core/dataflow/snapshot.h"
#include "views/replay_simulation_window/i_replay_simulation_window_view.h"
#include "views/simulation_window/simulation_window.h"

class ReplaySimulationWindow : public SimulationWindow, public IReplaySimulationWindowView {
    Q_OBJECT

public:
    explicit ReplaySimulationWindow(std::shared_ptr<std::vector<double>> timestamps,
                                    std::shared_ptr<DiagnosticsSeries> diagnostics_series,
                                    QWidget* parent = nullptr);
    ~ReplaySimulationWindow() override = default;

    void fillCharts(const DiagnosticsSeries& series) override;

    /**
     * @brief Update the buffer value displayed in the progress bar.
     * @param buffer_value The new buffer value to display.
     */
    void updateBufferValue(int buffer_value);

signals:
    /** @signal
     * @brief Emitted when the playback is toggled.
     */
    void togglePlayback();

    /** @signal
     * @brief Emitted when the simulation is stepped forward.
     */
    void stepForward();

    /** @signal
     * @brief Emitted when the simulation is stepped backward.
     */
    void stepBackward();

    /** @signal
     * @brief Emitted when the number of steps per second is changed by the user.
     */
    void stepsPerSecondChanged(int sps);

    /** @signal
     * @brief Emitted when a jump is requested.
     */
    void requestJump(double timestamp);

protected:
    bool updateProgress() const override { return !halt_for_jump; }

private slots:
    void onTogglePlayback();
    void onStepsPerSecondChanged(int sps);

private:
    std::shared_ptr<std::vector<double>> timestamps_ = nullptr;
    bool playback_active_ = true;  // Indicates if the playback is active
    bool halt_for_jump = false;    // Indicates if the playback was active before a jump
};
