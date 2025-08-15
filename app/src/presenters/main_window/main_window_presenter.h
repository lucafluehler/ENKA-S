#pragma once

#include <QObject>

#include "views/main_window/i_main_window_view.h"

class QTimer;
class ILoadSimulationPresenter;
class INewSimulationPresenter;

/**
 * @brief Responsible for managing the main window's view and updating the home screen.
 */
class MainWindowPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Initializes the presenter with the given view and sets up a timer to update the home
     * screen.
     * @param view The main window view to be managed.
     * @param load_simulation_presenter The presenter for the load simulation tab.
     * @param new_simulation_presenter The presenter for the new simulation tab.
     * @param parent The parent QObject, defaults to nullptr.
     */
    explicit MainWindowPresenter(IMainWindowView* view,
                                 ILoadSimulationPresenter* load_simulation_presenter,
                                 INewSimulationPresenter* new_simulation_presenter,
                                 QObject* parent = nullptr);
    ~MainWindowPresenter() override = default;

public slots:
    /**
     * Activates the current tab and deactivates all others.
     */
    void onTabSwitched();

private slots:
    void updateHomeScreen() { view_->updateHomeScreen(); }

private:
    IMainWindowView* view_ = nullptr;
    QTimer* home_screen_timer_ = nullptr;

    ILoadSimulationPresenter* load_simulation_presenter_ = nullptr;
    INewSimulationPresenter* new_simulation_presenter_ = nullptr;
};
