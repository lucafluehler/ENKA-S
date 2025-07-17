#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generation_method.h>
#include <enkas/simulation/simulation_method.h>
#include <qcontainerfwd.h>

#include <QHash>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QWidget>

#include "core/settings.h"
#include "i_new_simulation_view.h"
#include "settings_widgets/settings_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class NewSimulationTab;
}
QT_END_NAMESPACE

class NewSimulationPresenter;

class NewSimulationTab : public QWidget, INewSimulationView {
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

signals:
    void checkInitialSystemFile();
    void checkSettingsFile();

private slots:
    void openSystemDataDialog();
    void openSettingsDialog();

    void startSimulation();
    void abortSimulation();

private:
    Settings fetchSettings() const;
    void setupSettingsWidgets();
    void setupMethodSelection();
    void initializePreview();
    void setupSimulationProgressElements();
    void setupFileManagement();
    void resetSettings();
    void updateDefaultSettings(const Settings& settings);

    Ui::NewSimulationTab* ui_;
    NewSimulationPresenter* presenter_;
    QHash<enkas::simulation::Method, SettingsWidget*> simulation_settings_widgets_;
    QHash<enkas::generation::Method, SettingsWidget*> generation_settings_widgets_;

    QString initial_system_path_;
    QString settings_path_;
};
