#pragma once

#include <QObject>
#include <filesystem>
#include <memory>

#include "core/dataflow/snapshot.h"
#include "core/dataflow/system_ring_buffer.h"
#include "presenters/replay_simulation_window_presenter.h"
#include "views/replay_simulation_window/replay_simulation_window.h"
#include "workers/system_buffer_worker.h"

/**
 * @brief The SimulationPlayer class is responsible for managing the simulation playback,
 * including loading system files and handling playback controls.
 */
class SimulationPlayer : public QObject {
    Q_OBJECT
public:
    explicit SimulationPlayer(QObject* parent = nullptr);
    ~SimulationPlayer();

    /**
     * @brief Sets the playback speed in steps per second.
     * @param steps_per_second The desired steps per second.
     */
    void setStepsPerSecond(int steps_per_second) { step_delay_ms_ = 1000 / steps_per_second; }

    /**
     * @brief Struct representing required system data for the simulation replay.
     */
    struct SystemData {
        std::filesystem::path file_path = "";
        double simulation_duration = 0.0;
        std::size_t total_snapshots_count = 0;
    };

    /**
     * @brief Struct representing required diagnostics data for the simulation replay.
     */
    struct DiagnosticsData {
        std::shared_ptr<DiagnosticsSeries> diagnostics_series = nullptr;
    };

signals:
    /** @signal
     * @brief Emitted when the simulation window is closed.
     */
    void windowClosed();

public slots:
    /**
     * @brief Initializes the simulation player with the loaded data.
     * @param system_data Optional system data containing the file path, duration, and total
     * snapshot count.
     * @param diagnostics_data Optional diagnostics data containing the shared pointer to a
     * DiagnosticsSeries.
     */
    void run(std::optional<SystemData> system_data,
             std::optional<DiagnosticsData> diagnostics_data);

    /**
     * @brief Handles the pausing or resuming of the simulation playback.
     */
    void onTogglePlayback();

    /**
     * @brief Steps the simulation forward by one frame.
     */
    void onStepForward();

    /**
     * @brief Steps the simulation backward by one frame.
     */
    void onStepBackward();

    /**
     * @brief Jumps to a fraction of the playback bar.
     * @param fraction The fraction to jump to (0.0 to 1.0).
     */
    void onJump(float fraction);

private:
    void setupSystemBufferWorker();
    void setupDataUpdateTimer();
    void setupSimulationWindow(double simulation_duration,
                               const std::shared_ptr<DiagnosticsSeries>& diagnostics_series);

    ReplaySimulationWindow* simulation_window_ = nullptr;
    ReplaySimulationWindowPresenter* simulation_window_presenter_ = nullptr;

    SystemBufferWorker* system_buffer_worker_;
    QThread* system_buffer_thread_;

    bool is_playing_ = false;
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot_;
    int step_delay_ms_ = 1000 / 30;  // 30 steps per second

    std::shared_ptr<SystemRingBuffer> system_ring_buffer_ = nullptr;
    std::filesystem::path system_file_path_ = "";

    QTimer* buffer_value_update_timer_ = nullptr;
    int total_snapshots_count_ = 0;
};
