#pragma once

#include <QObject>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/dataflow/snapshot.h"
#include "core/dataflow/system_ring_buffer.h"
#include "presenters/simulation_window_presenter.h"
#include "views/simulation_window/simulation_window.h"
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

signals:
    /** @signal
     * @brief Emitted when the simulation window is closed.
     */
    void windowClosed();

public slots:
    /**
     * @brief Initializes the simulation player with the loaded data.
     * @param system_file_path Optional path to the system file to load.
     * @param timestamps Optional shared pointer to a vector of timestamps for the simulation.
     * @param diagnostics_series Optional shared pointer to a DiagnosticsSeries for additional data.
     * @note Atleast one of the parameters must be provided to initialize the player. For system
     * playback, both the system file path and timestamps must be provided.
     */
    void run(const std::filesystem::path& system_file_path = "",
             std::shared_ptr<std::vector<double>> timestamps = nullptr,
             std::shared_ptr<DiagnosticsSeries> diagnostics_series = nullptr);

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

private:
    void setupSystemBufferWorker();
    void setupDataUpdateTimer();

    SimulationWindow* simulation_window_;
    SimulationWindowPresenter* simulation_window_presenter_;

    SystemBufferWorker* system_buffer_worker_;
    QThread* system_buffer_thread_;

    bool is_playing_ = false;
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot_;

    std::shared_ptr<SystemRingBuffer> system_ring_buffer_ = nullptr;
    std::filesystem::path system_file_path_ = "";
};
