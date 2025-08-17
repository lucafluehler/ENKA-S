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

/**
 * @class CompositionRoot
 * @brief Implements the Composition Root pattern to construct and wire the application.
 *
 * This class is responsible for creating all the major objects of the application
 * (Services, Views, Presenters) and injecting their dependencies. It ensures that
 * the rest of the application code is decoupled and unaware of how objects are
 * created. This is the only place where concrete types are instantiated.
 *
 * All methods are static as this class should not be instantiated.
 */
class CompositionRoot {
public:
    CompositionRoot() = delete;  // This class is not meant to be instantiated.

    /**
     * @brief Composes and returns a complete, ready-to-run Application instance.
     *
     * This is the single public entry point for creating the application. It
     * orchestrates the setup of services, views, presenters, logging, and
     * signal-slot connections.
     *
     * @return A unique pointer to the fully configured Application object.
     */
    static std::unique_ptr<Application> compose();

private:
    /**
     * @struct Views
     * @brief A temporary container for UI components during the composition process.
     * @note The raw pointers (`new_simulation_tab`, etc.) are non-owning. Their
     *       objects are owned by the `main_window` unique_ptr, which is standard
     *       Qt parent-child ownership.
     */
    struct Views {
        std::unique_ptr<MainWindow> main_window;
        NewSimulationTab* new_simulation_tab;
        LoadSimulationTab* load_simulation_tab;
        LogsTab* logs_tab;
    };

    /**
     * @struct Presenters
     * @brief A temporary container for presenters during the composition process.
     * @note The raw pointers (`new_simulation_presenter`, etc.) are non-owning.
     *       Their objects are owned by the `main_window_presenter` unique_ptr.
     */
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
