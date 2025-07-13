#include "main_window.h"

#include "../../presenters/main_window_presenter.h"
#include "../load_simulation_tab/i_load_simulation_view.h"
#include "../new_simulation_tab/i_new_simulation_view.h"
#include "i_main_window_view.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      presenter_(new MainWindowPresenter(static_cast<IMainWindowView*>(this), this)) {
    ui->setupUi(this);
    ui->oglHomeScreen->initializeHomeScreen();

    // Signal management
    connect(ui->tabWidget,
            &QTabWidget::currentChanged,
            this,
            presenter_,
            &MainWindowPresenter::onTabChanged);
}

void MainWindow::updatePreview() {
    if (ui->tabWidget->currentIndex() == 0) ui->oglHomeScreen->update();
}

ILoadSimulationView* MainWindow::getLoadSimulationView() { return ui->tabLoadSimulation; }

INewSimulationView* MainWindow::getNewSimulationView() { return ui->tabNewSimulation; }