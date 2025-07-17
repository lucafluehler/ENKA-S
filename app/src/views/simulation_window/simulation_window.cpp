#include "simulation_window.h"

#include <QDebug>
#include <QVBoxLayout>

#include "./ui_simulation_window.h"

SimulationWindow::SimulationWindow(QWidget* parent)
    : QMainWindow(parent),
      mode(Mode::Uninitialized),
      simulation_duration(0.0),
      update_timer(new QTimer),
      update_in_progress(false),
      renderer_timer(new QElapsedTimer),
      movie_timer(new QTimer),
      ui(new Ui::SimulationWindow) {
    // Ui Setup
    ui->setupUi(this);
    ui->wgtSidebar->hide();
    ui->wgtSettings->hide();

    // Maximize window on startup. MUST be after ui setup
    setWindowState(windowState() | Qt::WindowMaximized);

    // Movie timer
    movie_timer->setInterval(1000 / 2);

    // Signal Management
    connect(ui->wgtSettings,
            &RenderSettingsWidget::settingsChanged,
            this,
            &SimulationWindow::saveSettings);
    connect(ui->btnToggleSettings, &QToolButton::clicked, this, &SimulationWindow::toggleSettings);
    connect(ui->btnToggleSidebar, &QToolButton::clicked, this, &SimulationWindow::toggleSidebar);
    connect(ui->btnScreenshot,
            &QToolButton::clicked,
            ui->oglParticleRenderer,
            &ParticleRenderer::saveScreenshot);
    connect(ui->btnMovie, &QToolButton::toggled, this, &SimulationWindow::toggleMovie);

    connect(update_timer, &QTimer::timeout, this, &SimulationWindow::update);
    connect(
        movie_timer, &QTimer::timeout, ui->oglParticleRenderer, &ParticleRenderer::saveScreenshot);
}

SimulationWindow::Mode SimulationWindow::getMode() const { return mode; }

void SimulationWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    update_timer->stop();
    hide();
}

void SimulationWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);

    update_timer->start();
    renderer_timer->restart();
}

void SimulationWindow::renderDataUpdate() {
    if (mode == Mode::Uninitialized) return;
    ui->oglParticleRenderer->updateData(data_ptr->render_data);
}

void SimulationWindow::diagnosticsDataUpdate() {}

void SimulationWindow::initLiveMode(const std::shared_ptr<DataPtr>& p_data_ptr,
                                    double p_simulation_duration) {
    if (mode != Mode::Uninitialized) return;

    // Disable or hide ui elements for file rendering
    ui->btnJumpToStart->setVisible(false);
    ui->btnStepBack->setVisible(false);
    ui->btnPlayStop->setVisible(false);
    ui->btnStepForward->setVisible(false);
    ui->btnJumpToEnd->setVisible(false);
    ui->btnChangeSpeed->setVisible(false);
    ui->hslNavigation->setEnabled(false);

    data_ptr = p_data_ptr;
    simulation_duration = p_simulation_duration;

    update_timer->start(1000 / settings.max_fps);
    renderer_timer->start();

    mode = Mode::Live;
}

void SimulationWindow::initFileMode(QString p_render_data_path,
                                    QString p_diagnostics_data_path,
                                    QString p_analytics_data_path) {
    if (mode != Mode::Uninitialized) return;

    if (p_render_data_path.isEmpty() && p_diagnostics_data_path.isEmpty() &&
        p_analytics_data_path.isEmpty())
        return;

    render_data_path = p_render_data_path;
    diagnostics_data_path = p_diagnostics_data_path;
    analytics_data_path = p_analytics_data_path;

    update_timer->start(1000 / settings.max_fps);
    renderer_timer->start();

    mode = Mode::File;
}

void SimulationWindow::saveSettings() {
    if (ui->wgtSettings->isHidden()) return;
    settings = ui->wgtSettings->getRenderSettings();
    update_timer->setInterval(1000 / 120);
}

void SimulationWindow::toggleSidebar() {
    if (ui->wgtSidebar->isVisible()) {
        ui->wgtSidebar->setVisible(false);
        ui->btnToggleSidebar->setArrowType(Qt::LeftArrow);
    } else {
        ui->wgtSidebar->setVisible(true);
        ui->btnToggleSidebar->setArrowType(Qt::RightArrow);
    }
}

void SimulationWindow::toggleSettings() {
    if (ui->wgtSettings->isVisible()) {
        ui->wgtSettings->setVisible(false);
    } else {
        ui->wgtSettings->loadRenderSettings(settings);
        ui->wgtSidebar->setVisible(true);
        ui->wgtSettings->setVisible(true);
        ui->btnToggleSidebar->setArrowType(Qt::RightArrow);
    }
}

void SimulationWindow::toggleMovie(bool checked) {
    if (checked) {
        movie_timer->start();
    } else {
        movie_timer->stop();
    }
}

void SimulationWindow::update() {
    if (update_in_progress) return;
    update_in_progress = true;

    // Update renderer
    qint64 elapsed_time = renderer_timer->elapsed();
    // qDebug() << elapsed_time;
    if (elapsed_time >= 1000.0 / settings.max_fps && data_ptr->render_data) {
        renderer_timer->restart();
        double fps = 1000.0 / elapsed_time;
        QString fps_text = QString::number(fps, 'f', 1) + " FPS";
        ui->lblFPS->setText(fps_text);

        // Redraw Particles
        ui->oglParticleRenderer->redraw(settings);

        double time = data_ptr->render_data->time;

        // Update time_progress label
        QString time_text = QString("%1 / %2").arg(time).arg(simulation_duration);
        ui->lblTime->setText(time_text);

        // Update horizontal progress slider
        ui->hslNavigation->setValue(1000.0 * time / simulation_duration);
    }

    update_in_progress = false;
}
