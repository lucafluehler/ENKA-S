#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/blocking_queue.h"
#include "core/snapshot.h"

class ISimulationWindowView;

class SimulationWindowPresenter : public QObject {
    Q_OBJECT
public:
    explicit SimulationWindowPresenter(ISimulationWindowView* view, QObject* parent = nullptr);

    enum class Mode { Uninitialized, Live, File };

    void initLiveMode(std::atomic<SystemSnapshotPtr>* render_queue_slot,
                      std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
                      double simulation_duration);

    void initFileMode(const QString& system_file_path = "",
                      const QString& diagnostics_file_path = "");

    Mode getMode() const { return mode_; };

public slots:
    void updateRendering();

private:
    ISimulationWindowView* view_;

    Mode mode_;
    double simulation_duration_;
    QTimer* render_timer_;
    std::chrono::steady_clock::time_point last_render_time_;

    std::atomic<SystemSnapshotPtr>* render_queue_slot_;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue_;
};
