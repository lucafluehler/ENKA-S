#include "live_simulation_window.h"

#include "forms/simulation_window/ui_simulation_window.h"
#include "widgets/debug_info_widget.h"

LiveSimulationWindow::LiveSimulationWindow(std::shared_ptr<LiveDebugInfo> debug_info,
                                           QWidget *parent)
    : SimulationWindow(parent) {
    ui_->btnJumpToStart->setVisible(false);
    ui_->btnStepBackward->setVisible(false);
    ui_->btnTogglePlayback->setVisible(false);
    ui_->btnStepForward->setVisible(false);
    ui_->btnJumpToEnd->setVisible(false);
    ui_->hslStepsPerSecond->setVisible(false);
    ui_->hslPlaybackBar->setEnabled(false);

    connect(ui_->btnToggleDebugInfo,
            &QToolButton::clicked,
            this,
            &LiveSimulationWindow::toggleDebugInfo);

    debug_info_ = debug_info;

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

void LiveSimulationWindow::updateDiagnostics(DiagnosticsSnapshotPtr diagnostics_snapshot) {
    if (!diagnostics_snapshot) return;
    ui_->wgtDiagnostics->updateData(*diagnostics_snapshot);
    ui_->oglParticleRenderer->updateCenterOfMass(diagnostics_snapshot->data->com_pos);
}

void LiveSimulationWindow::updateDebugInfo(int sps) {
    ui_->lblSPS->setText(QString::number(sps) + " SPS");
    ui_->wgtDebugInfo->updateInfo<LiveDebugInfo>(*debug_info_);
}

void LiveSimulationWindow::toggleDebugInfo() {
    if (ui_->wgtDebugInfo->isVisible()) {
        ui_->wgtDebugInfo->setVisible(false);
    } else {
        ui_->wgtSidebar->setVisible(true);
        ui_->wgtDebugInfo->setVisible(true);
        ui_->btnToggleSidebar->setArrowType(Qt::RightArrow);
    }
}
