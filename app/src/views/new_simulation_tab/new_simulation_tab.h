#pragma once

#include <enkas/data/system.h>

#include <QHash>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <memory>
#include <optional>

#include "core/settings/settings.h"
#include "i_new_simulation_view.h"
#include "settings_widgets/settings_schema.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class NewSimulationTab;
}
QT_END_NAMESPACE

class NewSimulationTab : public QWidget, public INewSimulationView {
    Q_OBJECT

public:
    NewSimulationTab(QWidget* parent = nullptr);
    ~NewSimulationTab();

    void updatePreview() override;
    void processSettings(const std::optional<Settings>& settings) override;
    void processInitialSystem(const std::optional<enkas::data::System>& system) override;
    void showSimulationProgress() override;
    void updateSimulationProgress(double time, double duration) override;
    void simulationAborted() override;
    QString getInitialSystemPath() const override;
    QString getSettingsPath() const override;
    Settings fetchSettings() const override;

signals:
    void checkInitialSystemFile();
    void checkSettingsFile();
    void requestSimulationStart();
    void requestSimulationAbort();
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

    QHash<GenerationMethod, std::shared_ptr<SettingsSchema>> generation_settings_schemas_;
    QHash<GenerationMethod, Settings> stored_generation_settings_;
    QHash<SimulationMethod, std::shared_ptr<SettingsSchema>> simulation_settings_schemas_;
    QHash<SimulationMethod, Settings> stored_simulation_settings_;

    QString initial_system_path_;
    QString settings_path_;
};
