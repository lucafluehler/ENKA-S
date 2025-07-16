#include "load_simulation_presenter.h"

#include <QObject>
#include <QThread>
#include <QTimer>

#include "../views/load_simulation_tab/i_load_simulation_view.h"
#include "../workers/file_parse_worker.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent)
    : QObject(parent), view_(view), preview_timer_(new QTimer(this)) {
    // Set up timer for preview updates
    connect(preview_timer_, &QTimer::timeout, this, &LoadSimulationPresenter::onTimerTimeout);

    // Initialize file parse worker
    file_parse_worker_ = new FileParseWorker();
    file_parse_thread_ = new QThread(this);
    file_parse_worker_->moveToThread(file_parse_thread_);

    connect(file_parse_worker_,
            &FileParseWorker::settingsParsed,
            this,
            &LoadSimulationPresenter::onSettingsParsed);
    connect(file_parse_worker_,
            &FileParseWorker::initialSystemParsed,
            this,
            &LoadSimulationPresenter::onInitialSystemParsed);
    connect(file_parse_worker_,
            &FileParseWorker::diagnosticsSeriesParsed,
            this,
            &LoadSimulationPresenter::onDiagnosticsSeriesParsed);

    file_parse_thread_->start();
}

LoadSimulationPresenter::~LoadSimulationPresenter() {
    if (file_parse_thread_) {
        file_parse_thread_->quit();
        file_parse_thread_->wait();
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

void LoadSimulationPresenter::checkFiles(const QVector<FileType>& files, const QString& file_path) {
    for (const auto& file : files) {
        switch (file) {
            case FileType::Settings:
                file_parse_worker_->parseSettings(file_path);
                break;
            case FileType::System:
                file_parse_worker_->parseNextSystemFrame(file_path);
                break;
            case FileType::DiagnosticsData:
                file_parse_worker_->parseDiagnosticsSeries(file_path);
                break;
        }
    }
}

void LoadSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    view_->onSettingsParsed(settings.has_value());
}

void LoadSimulationPresenter::onInitialSystemParsed(
    const std::optional<enkas::data::System>& system) {
    view_->onInitialSystemParsed(system);
}

void LoadSimulationPresenter::onDiagnosticsSeriesParsed(
    const std::optional<DiagnosticsSeries>& series) {
    view_->onDiagnosticsSeriesParsed(series.has_value());
}