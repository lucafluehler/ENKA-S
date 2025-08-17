#pragma once

#include <memory>

#include "application.h"

class MainWindow;
class NewSimulationTab;
class LoadSimulationTab;
class LogsTab;
class MainWindowPresenter;
class NewSimulationPresenter;
class LoadSimulationPresenter;

class CompositionRoot {
public:
    CompositionRoot() = delete;

    static std::unique_ptr<Application> compose();

private:
    struct Views {
        std::unique_ptr<MainWindow> main_window;
        NewSimulationTab* new_simulation_tab;
        LoadSimulationTab* load_simulation_tab;
        LogsTab* logs_tab;
    };

    struct Presenters {
        std::unique_ptr<MainWindowPresenter> main_window_presenter;
        NewSimulationPresenter* new_simulation_presenter;
        LoadSimulationPresenter* load_simulation_presenter;
    };

    static std::unique_ptr<Application::Services> setupServices();
    static std::unique_ptr<Views> setupViews();
    static std::unique_ptr<Presenters> setupPresenters(const Application::Services& services,
                                                       const Views& views);
    static void setupLogging(Application::Services& services);
    static void connectSignals(const Application::Services& services,
                               const Views& views,
                               const Presenters& presenters);
};
