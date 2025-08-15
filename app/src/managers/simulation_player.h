#pragma once

#include <QObject>
#include <filesystem>
#include <memory>

#include "core/dataflow/snapshot.h"
#include "core/dataflow/system_ring_buffer.h"
#include "i_simulation_player.h"
#include "managers/i_simulation_player.h"
#include "presenters/simulation_window/replay/replay_simulation_window_presenter.h"
#include "views/simulation_window/replay/replay_simulation_window.h"
#include "workers/system_buffer_worker.h"

/**
 * @brief The SimulationPlayer class is responsible for managing the simulation playback,
 * including loading system files and handling playback controls.
 */
class SimulationPlayer : public ISimulationPlayer {
    Q_OBJECT
public:
    explicit SimulationPlayer(QObject* parent = nullptr);
    ~SimulationPlayer();

    /**
     * @brief Sets the playback speed in steps per second.
     * @param steps_per_second The desired steps per second.
     */
    void setStepsPerSecond(int steps_per_second) { step_delay_ms_ = 1000 / steps_per_second; }

public slots:
    void run(std::optional<ISimulationPlayer::SystemData> system_data,
             std::optional<ISimulationPlayer::DiagnosticsData> diagnostics_data) override;

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
    int step_delay_ms_;

    std::shared_ptr<SystemRingBuffer> system_ring_buffer_ = nullptr;
    std::filesystem::path system_file_path_ = "";

    QTimer* buffer_value_update_timer_ = nullptr;
    int total_snapshots_count_ = 0;
};
