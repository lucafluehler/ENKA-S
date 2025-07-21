#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/blocking_queue.h"
#include "core/snapshot.h"

class ISimulationWindowView;

/**
 * @brief Handles the logic for the simulation window, including rendering updates and managing
 * different modes (live or file).
 */
class SimulationWindowPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Initializes the presenter with a view.
     * @param view Pointer to the view interface for the simulation window.
     * @param parent Optional parent QObject.
     */
    explicit SimulationWindowPresenter(ISimulationWindowView* view, QObject* parent = nullptr);
    ~SimulationWindowPresenter() override = default;

    /**
     * @brief Enumeration for the different modes of the simulation window.
     * Uninitialized: The presenter has not been initialized.
     * Live: The presenter is in live mode, receiving real-time data.
     * File: The presenter is in file mode, reading from files.
     */
    enum class Mode { Uninitialized, Live, File };

    /**
     * @brief Initializes the presenter for live mode.
     * @param render_queue_slot Pointer to the atomic slot for the render queue.
     * @param chart_queue Shared pointer to the diagnostics chart queue.
     * @param simulation_duration Duration of the simulation in seconds.
     */
    void initLiveMode(std::atomic<SystemSnapshotPtr>* render_queue_slot,
                      std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
                      double simulation_duration);

    /**
     * @brief Initializes the presenter for file mode.
     * @param system_file_path Path to the system file.
     * @param diagnostics_file_path Path to the diagnostics file.
     */
    void initFileMode(const QString& system_file_path = "",
                      const QString& diagnostics_file_path = "");

    /**
     * @brief Returns the current mode of the presenter.
     * @return The current mode.
     */
    Mode getMode() const { return mode_; };

public slots:
    /**
     * @brief Updates the rendering of the simulation window.
     * This method is called periodically to update the view with the latest system snapshot.
     */
    void updateRendering();

    /**
     * @brief Updates the charts with the latest diagnostics data.
     */
    void updateCharts();

private:
    ISimulationWindowView* view_;

    Mode mode_;
    QTimer* render_timer_;

    const int target_fps_ = 120;
    std::chrono::steady_clock::time_point last_fps_update_time_;
    int frame_count_ = 0;

    std::atomic<SystemSnapshotPtr>* render_queue_slot_;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue_;
};
