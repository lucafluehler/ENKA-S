#include "main_window.h"

#include "presenters/load_simulation_presenter.h"
#include "ui_main_window.h"
#include "views/load_simulation_tab/load_simulation_tab.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    // Initialize the load simulation tab
    load_simulation_presenter_ = new LoadSimulationPresenter(ui_->loadSimulationTab);
    connect(ui_->loadSimulationTab,
            &LoadSimulationTab::requestFilesCheck,
            load_simulation_presenter_,
            &LoadSimulationPresenter::checkFiles);

    ui_->oglHomeScreen->initializeHomeScreen();
}

void MainWindow::updateHomeScreen() {
    if (ui_->tabWidget->currentIndex() == 0) {
        ui_->oglHomeScreen->update();
    }
}
