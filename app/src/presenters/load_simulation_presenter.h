#pragma once

#include <QObject>
#include <QTimer>
#include <optional>

#include "core/files/file_parse_logic.h"
#include "enkas/data/system.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"

class QThread;
class ILoadSimulationView;
class FileParseWorker;

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

private slots:
    void updateInitialSystemPreview() { view_->updateInitialSystemPreview(); }
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& system);
    void onDiagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);

private:
    ILoadSimulationView* view_ = nullptr;
    QTimer* preview_timer_ = nullptr;
    FileParseWorker* file_parse_worker_ = nullptr;
    QThread* file_parse_thread_ = nullptr;
};
