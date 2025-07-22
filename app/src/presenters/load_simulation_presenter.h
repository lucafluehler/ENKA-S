#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <QTimer>
#include <memory>
#include <optional>

#include "core/snapshot.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"

class QThread;
class ILoadSimulationView;
class FileParseWorker;
class SimulationPlayer;

/**
 * @brief LoadSimulationPresenter handles the logic for loading and parsing simulation files.
 * It interacts with the view to update the UI based on the parsed data.
 */
class LoadSimulationPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs the presenter with a view and initializes the file parsing worker.
     * @param view The view to interact with.
     * @param parent The parent QObject.
     */
    explicit LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent = nullptr);
    ~LoadSimulationPresenter();

signals:
    void requestParseSettings(const QString& file_path);
    void requestParseDiagnosticsSeries(const QString& file_path);
    void requestOpenSystemFile(const QString& file_path);
    void requestInitialSnapshot();

public slots:
    /**
     * @brief Called when the view requests to check user files.
     */
    void checkFiles();

    /**
     * @brief Called when the tab becomes active.
     * Starts the timer for updating the particle rendering.
     */
    void active() {
        const int fps = 30;
        preview_timer_->start(1000 / fps);
    }

    /**
     * @brief Called when the tab becomes inactive.
     * Stops the timer for updating the particle rendering.
     */
    void inactive() { preview_timer_->stop(); }

    /**
     * @brief Starts the simulation playback with the loaded data.
     */
    void playSimulation();

    /**
     * @brief Ends the simulation playback.
     */
    void endSimulationPlayback();

private slots:
    void updateInitialSystemPreview() { view_->updateInitialSystemPreview(); }
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<SystemSnapshot>& snapshot);
    void onSystemFileOpened(const std::optional<std::vector<double>>& timestamps);
    void onDiagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);

private:
    ILoadSimulationView* view_ = nullptr;

    QTimer* preview_timer_ = nullptr;

    FileParseWorker* file_parse_worker_ = nullptr;
    QThread* file_parse_thread_ = nullptr;

    std::filesystem::path system_file_path_;
    std::shared_ptr<std::vector<double>> timestamps_ = nullptr;
    std::shared_ptr<DiagnosticsSeries> diagnostics_series_ = nullptr;

    SimulationPlayer* simulation_player_ = nullptr;
};
