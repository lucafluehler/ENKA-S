#include "main_window.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), home_timer(new QTimer), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->oglHomeScreen->initializeHomeScreen();

    home_timer->start(1000 / 30);
    connect(home_timer, &QTimer::timeout, this, &MainWindow::updatePreview);

    // Signal management
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int i) {
        if (i == 2) {
            ui->tabLoadSimulation->onTabSelected();
        } else {
            ui->tabLoadSimulation->onTabClosed();
        }
    });
}

void MainWindow::updatePreview() {
    if (ui->tabWidget->currentIndex() == 0) ui->oglHomeScreen->update();
}
