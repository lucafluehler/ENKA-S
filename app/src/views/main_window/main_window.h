#pragma once

#include <QMainWindow>

#include "i_main_window_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class LoadSimulationPresenter;
class NewSimulationPresenter;

/**
 * @brief MainWindow is the main application window.
 *
 * It manages the home screen directly and orchestrates other tabs with presenters.
 */
class MainWindow : public QMainWindow, public IMainWindowView {
    Q_OBJECT

public:
    /**
     * @brief Initializes the MainWindow UI and connects tabs to their presenters.
     * @param parent The parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

    void updateHomeScreen() override;

private:
    Ui::MainWindow *ui_;

    LoadSimulationPresenter *load_simulation_presenter_ = nullptr;
    NewSimulationPresenter *new_simulation_presenter_ = nullptr;
};
