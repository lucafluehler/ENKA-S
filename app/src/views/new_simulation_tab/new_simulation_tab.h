#pragma once

#include <enkas/data/system.h>

#include <QMap>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <memory>
#include <optional>

#include "core/settings/settings.h"
#include "forms/new_simulation_tab/ui_new_simulation_tab.h"
#include "i_new_simulation_view.h"
#include "settings_widgets/settings_schema.h"

/**
 * @brief NewSimulationTab allows users to configure and start new simulations.
 *
 * It provides options for selecting generation and simulation methods, loading settings,
 * and previewing the initial system.
 */
class NewSimulationTab : public QWidget, public INewSimulationView {
    Q_OBJECT

public:
    /**
     * @brief Initializes the NewSimulationTab UI and connects signals to slots.
     * @param parent The parent widget.
     */
    explicit NewSimulationTab(QWidget* parent = nullptr);
    ~NewSimulationTab() override = default;

    void updatePreview() override { ui_->oglSystemPreview->update(); }
    void processSettings(const std::optional<Settings>& settings) override;
    void processInitialSystem(const std::optional<enkas::data::System>& system) override;
    void showSimulationProgress() override { ui_->stwProgress->setCurrentIndex(2); }
    void updateSimulationProgress(double time, double duration) override;
    void simulationAborted() override { ui_->stwProgress->setCurrentIndex(0); }
    QString getInitialSystemPath() const override { return initial_system_path_; }
    QString getSettingsPath() const override { return settings_path_; }
    Settings fetchSettings() const override;

signals:
    /** @signal
     * @brief Emitted when the user-provided initial system file must be checked for validity.
     */
    void checkInitialSystemFile();

    /** @signal
     * @brief Emitted when the user-provided settings file must be checked for validity.
     */
    void checkSettingsFile();

    /** @signal
     * @brief Emitted when the user requests to start the simulation.
     */
    void requestSimulationStart();

    /** @signal
     * @brief Emitted when the user requests to abort the simulation.
     */
    void requestSimulationAbort();

    /** @signal
     * @brief Emitted when the user requests to open the simulation window.
     */
    void requestOpenSimulationWindow();

private slots:
    void onGenerationMethodChanged(int index);
    void onSimulationMethodChanged(int index);

    void openSettingsDialog();

    void startSimulation();
    void abortSimulation();

private:
    void setupSettingsWidgets();
    void setupMethodSelection();
    void setupSimulationProgressElements();
    void resetSettings();
    void loadSettings(const Settings& settings);

    Ui::NewSimulationTab* ui_;

    QMap<GenerationMethod, std::shared_ptr<SettingsSchema>> generation_settings_schemas_;
    QMap<GenerationMethod, Settings> stored_generation_settings_;
    GenerationMethod previous_generation_method_;

    QMap<SimulationMethod, std::shared_ptr<SettingsSchema>> simulation_settings_schemas_;
    QMap<SimulationMethod, Settings> stored_simulation_settings_;
    SimulationMethod previous_simulation_method_;

    QString initial_system_path_;
    QString settings_path_;
};
