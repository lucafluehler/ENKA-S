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

    // Maximize window on startup. MUST be after ui setup
    setWindowState(windowState() | Qt::WindowMaximized);

    // Movie timer
    connect(ui_->btnMovie, &QToolButton::toggled, this, &SimulationWindow::toggleMovie);
    connect(
        movie_timer, &QTimer::timeout, ui_->oglParticleRenderer, &ParticleRenderer::saveScreenshot);
    movie_timer->setInterval(1000 / 2);

    // Handle settings changes
    connect(ui_->wgtSettings,
            &RenderSettingsWidget::settingsChanged,
            this,
            &SimulationWindow::saveSettings);
    connect(
        ui_->wgtSettings, &RenderSettingsWidget::fpsChanged, this, &SimulationWindow::onFpsChanged);

    // Buttons
    connect(ui_->btnToggleSettings, &QToolButton::clicked, this, &SimulationWindow::toggleSettings);
    connect(ui_->btnToggleSidebar, &QToolButton::clicked, this, &SimulationWindow::toggleSidebar);
    connect(ui_->btnScreenshot,
            &QToolButton::clicked,
            ui_->oglParticleRenderer,
            &ParticleRenderer::saveScreenshot);

    // Add charts for displaying the diagnostics data
    setupCharts();
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

void SimulationWindow::toggleMovie(bool checked) {
    if (checked) {
        movie_timer->start();
    } else {
        movie_timer->stop();
    }
}

void SimulationWindow::updateFPS(int fps) { ui_->lblFPS->setText(QString::number(fps) + " FPS"); }

void SimulationWindow::updateSystemRendering(SystemSnapshotPtr system_snapshot,
                                             double simulation_duration) {
    if (system_snapshot) {
        ui_->oglParticleRenderer->updateData(system_snapshot);
    }

    // Redraw Particles even if no system snapshot is available, in order to avoid frame drops.
    ui_->oglParticleRenderer->redraw(settings_);

    // If no system snapshot is available we cannot update the time progress.
    // Also, if the playback is currently halted due to a jump, we do not want to update the slider.
    if (!system_snapshot || !updateProgress()) return;

    // Update time progress
    const double time = system_snapshot->time;
    const auto time_text = QString("%1 / %2").arg(time).arg(simulation_duration);
    ui_->lblTime->setText(time_text);

    // Update horizontal progress slider
    ui_->hslPlaybackBar->setValue(1000.0 * time / simulation_duration);
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
