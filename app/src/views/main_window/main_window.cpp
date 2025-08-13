#include "main_window.h"

#include "forms/main_window/ui_main_window.h"
#include "views/load_simulation_tab/load_simulation_tab.h"
#include "views/logs_tab/logs_tab.h"
#include "views/new_simulation_tab/new_simulation_tab.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    // Setup tab widget
    ui_->tabWidget->setCurrentIndex(0);
    connect(ui_->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabSwitched);

    ui_->oglHomeScreen->initializeHomeScreen();
}

int MainWindow::getCurrentTabIndex() const { return ui_->tabWidget->currentIndex(); }

LoadSimulationTab* MainWindow::getLoadSimulationTab() const { return ui_->tabLoadSimulation; }

NewSimulationTab* MainWindow::getNewSimulationTab() const { return ui_->tabNewSimulation; }

LogsTab* MainWindow::getLogsTab() const { return ui_->tabLogs; }

void MainWindow::updateHomeScreen() {
    if (ui_->tabWidget->currentIndex() == 0) {
        ui_->oglHomeScreen->update();
    }
}
