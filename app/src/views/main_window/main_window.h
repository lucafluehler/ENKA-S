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
     * @param parent The parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

    void updateHomeScreen() override;

    int getCurrentTabIndex() const;

    LoadSimulationTab *getLoadSimulationTab() const;
    NewSimulationTab *getNewSimulationTab() const;
    LogsTab *getLogsTab() const;

signals:
    void tabSwitched();

private:
    Ui::MainWindow *ui_;
};
