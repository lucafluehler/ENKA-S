#include "load_simulation_presenter.h"

#include <QObject>
#include <QThread>
#include <QTimer>

#include "../views/load_simulation_tab/i_load_simulation_view.h"
#include "../workers/file_check_worker.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent)
    : QObject(parent), view_(view), preview_timer_(new QTimer(this)) {
    // Set up timer for preview updates
    connect(preview_timer_, &QTimer::timeout, this, &LoadSimulationPresenter::onTimerTimeout);

    // Initialize file check worker
    file_check_worker_ = new FileCheckWorker();
    file_check_thread_ = new QThread(this);
    file_check_worker_->moveToThread(file_check_thread_);

    connect(this,
            &LoadSimulationPresenter::checkFiles,
            file_check_worker_,
            &FileCheckWorker::checkFiles);
    connect(file_check_worker_,
            &FileCheckWorker::fileChecked,
            this,
            &LoadSimulationPresenter::onFileChecked);

    file_check_thread_->start();
}

LoadSimulationPresenter::~LoadSimulationPresenter() {
    if (file_check_thread_) {
        file_check_thread_->quit();
        file_check_thread_->wait();
    }
}

void LoadSimulationPresenter::isSelected(bool selected) {
    if (selected) {
        const int fps = 30;  // Frames per second
        preview_timer_->start(1000 / fps);
    } else {
        preview_timer_->stop();
    }
}

void LoadSimulationPresenter::onTimerTimeout() {
    if (!view_) return;
    view_->updatePreview();
}

void LoadSimulationPresenter::onFileChecked(const FileType& file,
                                            const QString& path,
                                            bool result) {
    if (!view_) return;
    view_->onFileChecked(file, path, result);
}