#include "simulation_window.h"

#include <QDebug>
#include <QFileDialog>
#include <QToolButton>
#include <QVBoxLayout>

#include "core/snapshot.h"
#include "forms/simulation_window/ui_simulation_window.h"
#include "rendering/particle_renderer.h"
#include "rendering/render_settings.h"
#include "widgets/render_settings_widget.h"

SimulationWindow::SimulationWindow(QWidget* parent)
    : QMainWindow(parent), movie_timer(new QTimer), ui_(new Ui::SimulationWindow) {
    // Ui Setup
    ui_->setupUi(this);
    ui_->wgtSidebar->hide();
    ui_->wgtSettings->hide();

    // Maximize window on startup. MUST be after ui setup
    setWindowState(windowState() | Qt::WindowMaximized);

    // Movie timer
    movie_timer->setInterval(1000 / 2);

    // Signal Management
    connect(ui_->wgtSettings,
            &RenderSettingsWidget::settingsChanged,
            this,
            &SimulationWindow::saveSettings);
    connect(ui_->btnToggleSettings, &QToolButton::clicked, this, &SimulationWindow::toggleSettings);
    connect(ui_->btnToggleSidebar, &QToolButton::clicked, this, &SimulationWindow::toggleSidebar);
    connect(ui_->btnScreenshot,
            &QToolButton::clicked,
            ui_->oglParticleRenderer,
            &ParticleRenderer::saveScreenshot);
    connect(ui_->btnMovie, &QToolButton::toggled, this, &SimulationWindow::toggleMovie);

    connect(
        movie_timer, &QTimer::timeout, ui_->oglParticleRenderer, &ParticleRenderer::saveScreenshot);
}

void SimulationWindow::onDiagnosticsDataUpdate() {}

void SimulationWindow::initLiveMode() {
    ui_->btnJumpToStart->setVisible(false);
    ui_->btnStepBack->setVisible(false);
    ui_->btnPlayStop->setVisible(false);
    ui_->btnStepForward->setVisible(false);
    ui_->btnJumpToEnd->setVisible(false);
    ui_->btnChangeSpeed->setVisible(false);
    ui_->hslNavigation->setEnabled(false);
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

void SimulationWindow::updateSystemRendering(SystemSnapshotPtr system_snapshot,
                                             double simulation_duration) {
    if (!system_snapshot) return;

    // Redraw Particles
    ui_->oglParticleRenderer->updateData(system_snapshot);
    ui_->oglParticleRenderer->redraw(settings_);

    // Update time_progress label
    const double time = system_snapshot->time;
    const auto time_text = QString("%1 / %2").arg(time).arg(simulation_duration);
    ui_->lblTime->setText(time_text);

    // Update horizontal progress slider
    ui_->hslNavigation->setValue(1000.0 * time / simulation_duration);
}

void SimulationWindow::updateFPS(int fps) {
    const auto fps_text = QString::number(fps) + " FPS";
    ui_->lblFPS->setText(fps_text);
}
