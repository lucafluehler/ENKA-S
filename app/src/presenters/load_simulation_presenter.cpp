#include "load_simulation_presenter.h"

#include <QObject>
#include <QThread>
#include <QTimer>

#include "core/file_constants.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"
#include "workers/file_parse_worker.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent)
    : QObject(parent), view_(view), preview_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
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

void LoadSimulationPresenter::active() {
    const int fps = 30;
    preview_timer_->start(1000 / fps);
}

void LoadSimulationPresenter::inactive() { preview_timer_->stop(); }

void LoadSimulationPresenter::onTimerTimeout() { view_->updateInitialSystemPreview(); }

void LoadSimulationPresenter::checkFiles() {
    const auto& file_paths = view_->getFilesToCheck();
    if (file_paths.isEmpty()) {
        return;
    }

    for (const auto& file_path : file_paths) {
        if (file_path.endsWith(file_names::settings)) {
            file_parse_worker_->parseSettings(file_path);
        } else if (file_path.endsWith(file_names::system)) {
            file_parse_worker_->parseInitialSystem(file_path);
        } else if (file_path.endsWith(file_names::diagnostics)) {
            file_parse_worker_->parseDiagnosticsSeries(file_path);
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