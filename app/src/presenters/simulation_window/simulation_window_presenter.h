#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/dataflow/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"

class SimulationWindowPresenter : public QObject {
    Q_OBJECT
public:
    explicit SimulationWindowPresenter(
        ISimulationWindowView* view,
        std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
        double simulation_duration,
        QObject* parent = nullptr);
    ~SimulationWindowPresenter() override = default;

public slots:
    /**
     * @brief Updates the rendering of the simulation window.
     * This method is called periodically to update the view with the latest system snapshot.
     */
    void updateRendering();

    /**
     * @brief Handles changes in the target frames per second (FPS).
     */
    void onFpsChanged() { render_timer_->setInterval(1000 / view_->getTargetFPS()); }

private:
    ISimulationWindowView* view_;

    QTimer* render_timer_;
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot_;
    std::chrono::steady_clock::time_point last_fps_update_time_;
    int frame_count_ = 0;
    double simulation_duration_ = 0.0;
};
