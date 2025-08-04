#include "replay_simulation_window.h"

#include <QPushButton>
#include <QSlider>

#include "forms/simulation_window/ui_simulation_window.h"

ReplaySimulationWindow::ReplaySimulationWindow(
    std::shared_ptr<std::vector<double>> timestamps,
    std::shared_ptr<DiagnosticsSeries> diagnostics_series,
    QWidget* parent)
    : SimulationWindow(parent), timestamps_(std::move(timestamps)) {
    // Setup the UI for replay mode
    ui_->btnToggleDebugInfo->setVisible(false);

    const bool has_timestamps = timestamps_ && !timestamps_->empty();
    ui_->btnJumpToStart->setVisible(has_timestamps);
    ui_->btnStepBackward->setVisible(has_timestamps);
    ui_->btnTogglePlayback->setVisible(has_timestamps);
    ui_->btnStepForward->setVisible(has_timestamps);
    ui_->btnJumpToEnd->setVisible(has_timestamps);
    ui_->hslStepsPerSecond->setVisible(has_timestamps);
    ui_->hslPlaybackBar->setEnabled(has_timestamps);

    // Setup the steps per second slider
    const int min_sps = 1;
    const int max_sps = 1000;
    const int default_sps = 60;
    ui_->hslStepsPerSecond->setRange(min_sps, max_sps);
    ui_->hslStepsPerSecond->setValue(default_sps);
    onStepsPerSecondChanged(default_sps);

    // Store the timestamps and load the chart data
    if (timestamps) {
        timestamps_ = std::move(timestamps);
    }

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
        if (timestamps_ && !timestamps_->empty()) {
            const int value = ui_->hslPlaybackBar->value();
            const int timestamps_size = static_cast<int>(timestamps_->size());
            const int max_slider = 1000;

            double fraction = double(value - 1) / double(max_slider - 1);
            int idx = static_cast<int>(std::lround(fraction * (timestamps_size - 1)));

            idx = std::clamp(idx, 0, timestamps_size - 1);

            emit requestJump(timestamps_->at(idx));

            if (halt_for_jump) {
                halt_for_jump = false;
                emit togglePlayback();
            }
        }
    });
    connect(ui_->btnJumpToStart, &QPushButton::clicked, this, [this]() {
        if (timestamps_ && !timestamps_->empty()) {
            if (playback_active_) onTogglePlayback();
            emit requestJump(timestamps_->front());
        }
    });
    connect(ui_->btnJumpToEnd, &QPushButton::clicked, this, [this]() {
        if (timestamps_ && !timestamps_->empty()) {
            if (playback_active_) onTogglePlayback();
            emit requestJump(timestamps_->back());
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
