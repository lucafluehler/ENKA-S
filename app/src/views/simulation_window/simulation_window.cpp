#include "simulation_window.h"

#include <QDebug>
#include <QFileDialog>
#include <QToolButton>
#include <QVBoxLayout>

#include "core/charts/chart_extractors.h"
#include "core/dataflow/snapshot.h"
#include "forms/simulation_window/ui_simulation_window.h"
#include "rendering/particle_renderer.h"
#include "rendering/render_settings.h"
#include "widgets/debug_info_widget.h"
#include "widgets/render_settings_widget.h"

SimulationWindow::SimulationWindow(QWidget* parent)
    : QMainWindow(parent), movie_timer(new QTimer), ui_(new Ui::SimulationWindow) {
    // Ui Setup
    ui_->setupUi(this);
    ui_->wgtSidebar->hide();
    ui_->wgtSettings->hide();
    ui_->wgtDebugInfo->hide();

    // Setup the steps per second slider
    const int min_sps = 1;
    const int max_sps = 1000;
    const int default_sps = 60;
    ui_->hslStepsPerSecond->setRange(min_sps, max_sps);
    ui_->hslStepsPerSecond->setValue(default_sps);
    onStepsPerSecondChanged(default_sps);

    // Maximize window on startup. MUST be after ui setup
    setWindowState(windowState() | Qt::WindowMaximized);

    // Movie timer
    movie_timer->setInterval(1000 / 2);

    // Signal Management
    connect(ui_->wgtSettings,
            &RenderSettingsWidget::settingsChanged,
            this,
            &SimulationWindow::saveSettings);
    connect(
        ui_->wgtSettings, &RenderSettingsWidget::fpsChanged, this, &SimulationWindow::onFpsChanged);
    connect(ui_->btnToggleSettings, &QToolButton::clicked, this, &SimulationWindow::toggleSettings);
    connect(
        ui_->btnToggleDebugInfo, &QToolButton::clicked, this, &SimulationWindow::toggleDebugInfo);
    connect(ui_->btnToggleSidebar, &QToolButton::clicked, this, &SimulationWindow::toggleSidebar);
    connect(ui_->btnScreenshot,
            &QToolButton::clicked,
            ui_->oglParticleRenderer,
            &ParticleRenderer::saveScreenshot);
    connect(ui_->btnMovie, &QToolButton::toggled, this, &SimulationWindow::toggleMovie);
    connect(ui_->hslStepsPerSecond,
            &QSlider::valueChanged,
            this,
            &SimulationWindow::onStepsPerSecondChanged);

    // Replay controls
    connect(
        ui_->btnTogglePlayback, &QPushButton::clicked, this, &SimulationWindow::onTogglePlayback);
    connect(ui_->btnStepForward, &QPushButton::clicked, this, [this]() { emit stepForward(); });
    connect(ui_->btnStepBackward, &QPushButton::clicked, this, [this]() { emit stepBackward(); });

    connect(
        movie_timer, &QTimer::timeout, ui_->oglParticleRenderer, &ParticleRenderer::saveScreenshot);

    // Add charts for displaying the diagnostics data
    setupCharts();
}

void SimulationWindow::initLiveMode(std::shared_ptr<LiveDebugInfo> debug_info) {
    ui_->btnJumpToStart->setVisible(false);
    ui_->btnStepBackward->setVisible(false);
    ui_->btnTogglePlayback->setVisible(false);
    ui_->btnStepForward->setVisible(false);
    ui_->btnJumpToEnd->setVisible(false);
    ui_->hslStepsPerSecond->setVisible(false);
    ui_->hslPlaybackBar->setEnabled(false);

    live_debug_info_ = debug_info;
    simulation_duration_ = debug_info->duration;

    static const std::vector<DebugInfoRow<LiveDebugInfo>> live_debug_mapping = {
        {.name = "System Data Pool",
         .size_member = &LiveDebugInfo::system_data_pool_size,
         .capacity_member = &LiveDebugInfo::system_data_pool_capacity,
         .more_is_better = true},
        {.name = "Diagnostics Data Pool",
         .size_member = &LiveDebugInfo::diagnostics_data_pool_size,
         .capacity_member = &LiveDebugInfo::diagnostics_data_pool_capacity,
         .more_is_better = true},
        {.name = "System Snapshot Pool",
         .size_member = &LiveDebugInfo::system_snapshot_pool_size,
         .capacity_member = &LiveDebugInfo::system_snapshot_pool_capacity,
         .more_is_better = true},
        {.name = "Diagnostics Snapshot Pool",
         .size_member = &LiveDebugInfo::diagnostics_snapshot_pool_size,
         .capacity_member = &LiveDebugInfo::diagnostics_snapshot_pool_capacity,
         .more_is_better = true},
        {.name = "Chart Queue",
         .size_member = &LiveDebugInfo::chart_queue_size,
         .capacity_member = &LiveDebugInfo::chart_queue_capacity,
         .more_is_better = false},
        {.name = "System Storage Queue",
         .size_member = &LiveDebugInfo::system_storage_queue_size,
         .capacity_member = &LiveDebugInfo::system_storage_queue_capacity,
         .more_is_better = false},
        {.name = "Diagnostics Storage Queue",
         .size_member = &LiveDebugInfo::diagnostics_storage_queue_size,
         .capacity_member = &LiveDebugInfo::diagnostics_storage_queue_capacity,
         .more_is_better = false},
    };

    ui_->wgtDebugInfo->setupInfo<LiveDebugInfo>(live_debug_mapping);
}

void SimulationWindow::initReplayMode(std::shared_ptr<std::vector<double>> timestamps,
                                      std::shared_ptr<DiagnosticsSeries> diagnostics_series) {
    // Setup the UI for replay mode
    ui_->btnJumpToStart->setVisible(true);
    ui_->btnStepBackward->setVisible(true);
    ui_->btnTogglePlayback->setVisible(true);
    ui_->btnStepForward->setVisible(true);
    ui_->btnJumpToEnd->setVisible(true);
    ui_->hslStepsPerSecond->setVisible(true);
    ui_->hslPlaybackBar->setEnabled(true);
    ui_->btnToggleDebugInfo->setVisible(false);

    // Store the timestamps and load the chart data
    if (timestamps) {
        timestamps_ = std::move(timestamps);
        simulation_duration_ = timestamps_->back();
    }

    if (diagnostics_series) {
        ui_->wgtDiagnostics->fillCharts(*diagnostics_series);
    }
}

void SimulationWindow::saveSettings() {
    if (ui_->wgtSettings->isHidden()) return;
    settings_ = ui_->wgtSettings->getRenderSettings();
}

void SimulationWindow::toggleSidebar() {
    if (ui_->wgtSidebar->isVisible()) {
        ui_->wgtSidebar->setVisible(false);
        ui_->btnToggleSidebar->setArrowType(Qt::LeftArrow);
    } else {
        ui_->wgtSidebar->setVisible(true);
        ui_->btnToggleSidebar->setArrowType(Qt::RightArrow);
    }
}

void SimulationWindow::toggleSettings() {
    if (ui_->wgtSettings->isVisible()) {
        ui_->wgtSettings->setVisible(false);
    } else {
        ui_->wgtSettings->loadRenderSettings(settings_);
        ui_->wgtSidebar->setVisible(true);
        ui_->wgtSettings->setVisible(true);
        ui_->btnToggleSidebar->setArrowType(Qt::RightArrow);
    }
}

void SimulationWindow::toggleDebugInfo() {
    if (ui_->wgtDebugInfo->isVisible()) {
        ui_->wgtDebugInfo->setVisible(false);
    } else {
        ui_->wgtSidebar->setVisible(true);
        ui_->wgtDebugInfo->setVisible(true);
        ui_->btnToggleSidebar->setArrowType(Qt::RightArrow);
    }
}

void SimulationWindow::toggleMovie(bool checked) {
    if (checked) {
        movie_timer->start();
    } else {
        movie_timer->stop();
    }
}

void SimulationWindow::onTogglePlayback() {
    ui_->btnStepForward->setEnabled(playback_active_);
    ui_->btnStepBackward->setEnabled(playback_active_);
    playback_active_ = !playback_active_;

    if (playback_active_) {
        ui_->btnTogglePlayback->setIcon(QIcon(":/controls/icons/pause.png"));
    } else {
        ui_->btnTogglePlayback->setIcon(QIcon(":/controls/icons/play.png"));
    }

    emit togglePlayback();
}

void SimulationWindow::updateSystemRendering(SystemSnapshotPtr system_snapshot) {
    if (system_snapshot) {
        ui_->oglParticleRenderer->updateData(system_snapshot);
    }

    // Redraw Particles even if no system snapshot is available, in order to avoid frame drops.
    ui_->oglParticleRenderer->redraw(settings_);

    if (!system_snapshot) {
        // If no system snapshot is available we cannot update the time progress.
        return;
    }

    // Update time progress
    const double time = system_snapshot->time;
    const auto time_text = QString("%1 / %2").arg(time).arg(simulation_duration_);
    ui_->lblTime->setText(time_text);

    // Update horizontal progress slider
    ui_->hslPlaybackBar->setValue(1000.0 * time / simulation_duration_);

    // Update the debug info
    if (live_debug_info_) {
        ui_->wgtDebugInfo->updateInfo<LiveDebugInfo>(*live_debug_info_);
    }
}

void SimulationWindow::updateDebugInfo(int fps, int sps) {
    const auto fps_text = QString::number(fps) + " FPS";
    ui_->lblFPS->setText(fps_text);

    if (sps < 0) {
        return;  // SPS is not applicable in replay mode
    }
    const auto sps_text = QString::number(sps) + " SPS";
    ui_->lblSPS->setText(sps_text);
}

void SimulationWindow::updateDiagnostics(DiagnosticsSnapshotPtr diagnostics_snapshot) {
    if (!diagnostics_snapshot) return;
    ui_->wgtDiagnostics->updateData(*diagnostics_snapshot);
    ui_->oglParticleRenderer->updateCenterOfMass(diagnostics_snapshot->data->com_pos);
}

void SimulationWindow::fillCharts(const DiagnosticsSeries& series) {
    ui_->wgtDiagnostics->fillCharts(series);
}

void SimulationWindow::closeEvent(QCloseEvent* event) {
    emit windowClosed();
    QMainWindow::closeEvent(event);
}

void SimulationWindow::setupCharts() {
    std::vector<DiagnosticsWidget::ChartDefinition> charts;

    charts.push_back(
        {.title = "Total Energy", .unit = "J", .value_extractor = [](DiagnosticsSnapshot& s) {
             return s.data->e_kin + s.data->e_pot;
         }});

    charts.push_back({.title = "Total Energy Change",
                      .unit = "%",
                      .value_extractor = createPercentageChangeExtractor(
                          [](DiagnosticsSnapshot& s) { return s.data->e_kin + s.data->e_pot; })});

    charts.push_back({.title = "Kinetic Energy",
                      .unit = "J",
                      .value_extractor = [](DiagnosticsSnapshot& s) { return s.data->e_kin; }});

    charts.push_back({.title = "Potential Energy",
                      .unit = "J",
                      .value_extractor = [](DiagnosticsSnapshot& s) { return s.data->e_pot; }});

    charts.push_back({.title = "Total Angular Momentum",
                      .unit = "kg*m^2/s",
                      .value_extractor = [](DiagnosticsSnapshot& s) { return s.data->L_tot; }});

    charts.push_back({.title = "Total Angular Momentum Change",
                      .unit = "%",
                      .value_extractor = createPercentageChangeExtractor(
                          [](DiagnosticsSnapshot& s) { return s.data->L_tot; })});

    charts.push_back({.title = "Virial Radius",
                      .unit = "kpc",
                      .value_extractor = [](DiagnosticsSnapshot& s) { return s.data->r_vir; }});

    charts.push_back({.title = "Mean Square Velocity",
                      .unit = "H_L/H_T",
                      .value_extractor = [](DiagnosticsSnapshot& s) { return s.data->ms_vel; }});

    charts.push_back({.title = "Crossing Time",
                      .unit = "H_T",
                      .value_extractor = [](DiagnosticsSnapshot& s) { return s.data->t_cr; }});

    ui_->wgtDiagnostics->setupCharts(std::move(charts), "H_T");
}

void SimulationWindow::onStepsPerSecondChanged(int sps) {
    const auto sps_text = QString::number(sps) + " SPS";
    ui_->lblSPS->setText(sps_text);

    emit stepsPerSecondChanged(sps);
}

void SimulationWindow::updateBufferValue(int buffer_value) {
    ui_->hslPlaybackBar->setBufferValue(buffer_value);
}