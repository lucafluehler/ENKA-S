#include "main_window.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    ui_->oglHomeScreen->initializeHomeScreen();
}

void MainWindow::updateHomeScreen() {
    if (ui_->tabWidget->currentIndex() == 0) {
        ui_->oglHomeScreen->update();
    }
}
