#include "main_window.h"

#include "forms/main_window/ui_main_window.h"
#include "views/load_simulation_tab/load_simulation_tab.h"
#include "views/logs_tab/logs_tab.h"
#include "views/new_simulation_tab/new_simulation_tab.h"

MainWindow::MainWindow(NewSimulationTab* new_simulation_tab_,
                       LoadSimulationTab* load_simulation_tab_,
                       LogsTab* logs_tab_,
                       QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      new_simulation_tab_(new_simulation_tab_),
      load_simulation_tab_(load_simulation_tab_),
      logs_tab_(logs_tab_) {
    ui_->setupUi(this);

    ui_->tabWidget->addTab(new_simulation_tab_, "New Simulation");
    ui_->tabWidget->addTab(load_simulation_tab_, "Load Simulation");
    ui_->tabWidget->addTab(logs_tab_, "Logs");

    ui_->tabWidget->setCurrentIndex(0);
    connect(ui_->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabSwitched);

    ui_->oglHomeScreen->initializeHomeScreen();
}

int MainWindow::getCurrentTabIndex() const { return ui_->tabWidget->currentIndex(); }

void MainWindow::updateHomeScreen() {
    if (ui_->tabWidget->currentIndex() == 0) {
        ui_->oglHomeScreen->update();
    }
}
