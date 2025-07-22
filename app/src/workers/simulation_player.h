#pragma once

#include <QObject>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/snapshot.h"
#include "core/system_ring_buffer.h"
#include "presenters/simulation_window_presenter.h"
#include "views/simulation_window/simulation_window.h"
#include "workers/system_buffer_worker.h"

class SimulationPlayer : public QObject {
    Q_OBJECT
public:
    explicit SimulationPlayer(QObject* parent = nullptr);
    ~SimulationPlayer();

signals:
    void windowClosed();

public slots:
    void run(const std::filesystem::path& system_file_path = "",
             std::shared_ptr<std::vector<double>> timestamps = nullptr,
             std::shared_ptr<DiagnosticsSeries> diagnostics_series = nullptr);

    void onTogglePlayback();
    void onStepForward();
    void onStepBackward();

private:
    void setupSystemBufferWorker();
    void setupDataUpdateTimer();

    SimulationWindow* simulation_window_;
    SimulationWindowPresenter* simulation_window_presenter_;

    SystemBufferWorker* system_buffer_worker_;
    QThread* system_buffer_thread_;

    QTimer* data_update_timer_ = nullptr;
    std::atomic<SystemSnapshotPtr> render_queue_slot_;

    std::shared_ptr<SystemRingBuffer> system_ring_buffer_ = nullptr;
    std::filesystem::path system_file_path_ = "";
};
