#include "main_window.h"

#include <QPushButton>

#include "forms/main_window/ui_main_window.h"
#include "presenters/load_simulation_presenter.h"
#include "presenters/new_simulation_presenter.h"
#include "views/load_simulation_tab/load_simulation_tab.h"
#include "views/new_simulation_tab/new_simulation_tab.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    // Initialize the load simulation tab
    load_simulation_presenter_ = new LoadSimulationPresenter(ui_->tabLoadSimulation);
    connect(ui_->tabLoadSimulation,
            &LoadSimulationTab::requestFilesCheck,
            load_simulation_presenter_,
            &LoadSimulationPresenter::checkFiles);

    connect(ui_->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        QWidget* current = ui_->tabWidget->widget(index);
        if (current == ui_->tabLoadSimulation) {
            load_simulation_presenter_->active();
        } else {
            load_simulation_presenter_->inactive();
        }
    });

    // Initialize the new simulation tab
    new_simulation_presenter_ = new NewSimulationPresenter(ui_->tabNewSimulation);
    connect(ui_->tabNewSimulation,
            &NewSimulationTab::checkInitialSystemFile,
            new_simulation_presenter_,
            &NewSimulationPresenter::checkInitialSystemFile);
    connect(ui_->tabNewSimulation,
            &NewSimulationTab::checkSettingsFile,
            new_simulation_presenter_,
            &NewSimulationPresenter::checkSettingsFile);
    connect(ui_->tabNewSimulation,
            &NewSimulationTab::requestSimulationStart,
            new_simulation_presenter_,
            &NewSimulationPresenter::startSimulation);
    connect(ui_->tabNewSimulation,
            &NewSimulationTab::requestSimulationAbort,
            new_simulation_presenter_,
            &NewSimulationPresenter::abortSimulation);
    connect(ui_->tabNewSimulation,
            &NewSimulationTab::requestOpenSimulationWindow,
            new_simulation_presenter_,
            &NewSimulationPresenter::openSimulationWindow);

    ui_->oglHomeScreen->initializeHomeScreen();
}

void MainWindow::updateHomeScreen() {
    if (ui_->tabWidget->currentIndex() == 0) {
        ui_->oglHomeScreen->update();
    }
}
