#include "replay_simulation_window.h"

#include <QPushButton>
#include <QSlider>

#include "forms/simulation_window/ui_simulation_window.h"

namespace {
// --- Constants for Steps Per Second Slider ---
constexpr int kMinStepsPerSecond = 1;
constexpr int kMaxStepsPerSecond = 1000;
constexpr int kDefaultStepsPerSecond = 60;
}  // namespace

ReplaySimulationWindow::ReplaySimulationWindow(
    bool enable_playback_elements,
    std::shared_ptr<DiagnosticsSeries> diagnostics_series,
    QWidget* parent)
    : SimulationWindow(parent), enable_playback_elements_(enable_playback_elements) {
    // Setup the UI for replay mode
    ui_->btnToggleDebugInfo->setVisible(false);

    ui_->btnJumpToStart->setVisible(enable_playback_elements);
    ui_->btnStepBackward->setVisible(enable_playback_elements);
    ui_->btnTogglePlayback->setVisible(enable_playback_elements);
    ui_->btnStepForward->setVisible(enable_playback_elements);
    ui_->btnJumpToEnd->setVisible(enable_playback_elements);
    ui_->hslStepsPerSecond->setVisible(enable_playback_elements);
    ui_->hslPlaybackBar->setEnabled(enable_playback_elements);

    // Setup the steps per second slider
    ui_->hslStepsPerSecond->setRange(kMinStepsPerSecond, kMaxStepsPerSecond);
    ui_->hslStepsPerSecond->setValue(kDefaultStepsPerSecond);
    onStepsPerSecondChanged(kDefaultStepsPerSecond);

    // Load the chart data
    if (diagnostics_series) {
        ui_->wgtDiagnostics->fillCharts(*diagnostics_series);
    }

    // Signals
    connect(ui_->btnTogglePlayback,
            &QPushButton::clicked,
            this,
            &ReplaySimulationWindow::onTogglePlayback);
    connect(ui_->btnStepForward, &QPushButton::clicked, this, [this]() { emit stepForward(); });
    connect(ui_->btnStepBackward, &QPushButton::clicked, this, [this]() { emit stepBackward(); });
    connect(ui_->hslPlaybackBar, &PlaybackBar::sliderPressed, this, [this]() {
        if (playback_active_) {
            halt_for_jump = playback_active_;
            emit togglePlayback();
        }
    });
    connect(ui_->hslPlaybackBar, &PlaybackBar::sliderReleased, this, [this]() {
        if (enable_playback_elements_) {
            float fraction = float(ui_->hslPlaybackBar->value() - ui_->hslPlaybackBar->minimum()) /
                             float(ui_->hslPlaybackBar->maximum() - ui_->hslPlaybackBar->minimum());

            emit requestJump(fraction);

            if (halt_for_jump) {
                halt_for_jump = false;
                emit togglePlayback();
            }
        }
    });
    connect(ui_->btnJumpToStart, &QPushButton::clicked, this, [this]() {
        if (enable_playback_elements_) {
            if (playback_active_) onTogglePlayback();
            emit requestJump(0.0f);
        }
    });
    connect(ui_->btnJumpToEnd, &QPushButton::clicked, this, [this]() {
        if (enable_playback_elements_) {
            if (playback_active_) onTogglePlayback();
            emit requestJump(1.0f);
        }
    });
    connect(ui_->hslStepsPerSecond,
            &QSlider::valueChanged,
            this,
            &ReplaySimulationWindow::onStepsPerSecondChanged);
}

void ReplaySimulationWindow::fillCharts(const DiagnosticsSeries& series) {
    ui_->wgtDiagnostics->fillCharts(series);
}

void ReplaySimulationWindow::updateBufferValue(int buffer_value) {
    ui_->hslPlaybackBar->setBufferValue(buffer_value);
}

void ReplaySimulationWindow::onTogglePlayback() {
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

void ReplaySimulationWindow::onStepsPerSecondChanged(int sps) {
    const auto sps_text = QString::number(sps) + " SPS";
    ui_->lblSPS->setText(sps_text);

    emit stepsPerSecondChanged(sps);
}
