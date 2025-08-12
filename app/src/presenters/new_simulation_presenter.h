#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <QTimer>
#include <optional>

#include "core/settings/settings.h"
#include "views/new_simulation_tab/i_new_simulation_view.h"

class QThread;
class INewSimulationView;
class SimulationRunner;

/**
 * @brief Manages the new simulation view and handles simulation-related operations.
 */
class NewSimulationPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Initializes the presenter with the given view and sets up necessary workers and
     * timers.
     * @param view The new simulation view to be managed.
     * @param parent The parent QObject, defaults to nullptr.
     */
    explicit NewSimulationPresenter(INewSimulationView* view, QObject* parent = nullptr);
    ~NewSimulationPresenter() override = default;

public slots:
    /**
     * @brief Called when the tab becomes active, starting the timer for updating the particle
     * rendering.
     */
    void active() {
        const int fps = 30;
        preview_timer_->start(1000 / fps);
    }

    /**
     * @brief Called when the tab becomes inactive, stopping the timer for updating the
     * particle rendering.
     */
    void inactive() { preview_timer_->stop(); }

    /**
     * @brief Checks the initial system file and starts parsing it.
     */
    void checkInitialSystemFile();

    /**
     * @brief Checks the settings file and starts parsing it.
     */
    void checkSettingsFile();

    /**
     * @brief Starts the simulation using the parsed settings and initial system.
     */
    void startSimulation();

    /**
     * @brief Aborts the simulation if it is running.
     */
    void abortSimulation();

    /**
     * @brief Opens the simulation window to display the simulation results.
     */
    void openSimulationWindow();

private slots:
    void updatePreview();
    void updateProgress();
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& system);
    void onInitializationCompleted();

private:
    void setupFileParseWorker();

    INewSimulationView* view_ = nullptr;

    QTimer* preview_timer_ = nullptr;
    QTimer* progress_timer_ = nullptr;

    SimulationRunner* simulation_runner_ = nullptr;
};
