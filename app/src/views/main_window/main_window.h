#pragma once

#include <QMainWindow>

#include "i_main_window_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class LoadSimulationTab;
class NewSimulationTab;
class LogsTab;

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
     * @param new_simulation_tab_ The new simulation ui tab.
     * @param load_simulation_tab_ The load simulation ui tab.
     * @param logs_tab_ The logs ui tab.
     * @param parent The parent widget.
     */
    explicit MainWindow(NewSimulationTab *new_simulation_tab_,
                        LoadSimulationTab *load_simulation_tab_,
                        LogsTab *logs_tab_,
                        QWidget *parent = nullptr);
    ~MainWindow() override = default;

    void updateHomeScreen() override;

    int getCurrentTabIndex() const override;

signals:
    void tabSwitched();

private:
    Ui::MainWindow *ui_;

    NewSimulationTab *new_simulation_tab_;
    LoadSimulationTab *load_simulation_tab_;
    LogsTab *logs_tab_;
};
