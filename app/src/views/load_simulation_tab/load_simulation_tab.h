#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVector>
#include <QWidget>

#include "forms/load_simulation_tab/ui_load_simulation_tab.h"
#include "i_load_simulation_view.h"

/**
 * @brief The LoadSimulationTab class provides a user interface for loading simulation data.
 * It allows users to select a folder containing simulation files and checks for the presence of
 * required files such as settings, system, and diagnostics.
 */
class LoadSimulationTab : public QWidget, public ILoadSimulationView {
    Q_OBJECT

public:
    /**
     * @brief Initializes the LoadSimulationTab UI and connects signals to slots.
     * @param parent The parent widget.
     */
    explicit LoadSimulationTab(QWidget *parent = nullptr);
    ~LoadSimulationTab() override = default;

    void updateInitialSystemPreview() override { ui_->oglSystemPreview->update(); }
    void onSettingsParsed(std::optional<Settings> settings) override;
    void onInitialSystemParsed(std::optional<enkas::data::System> system) override;
    void onDiagnosticsSeriesParsed(bool success) override;
    QVector<QString> getFilesToCheck() const override;

signals:
    /** @signal
     * @brief Emitted when the user selects a folder to load simulation files.
     * If the folder contains expected files, their paths are stored.
     */
    void requestFilesCheck();

    /** @signal
     * @brief Emitted when the user requests to play the simulation with the loaded data.
     */
    void playSimulation();

private slots:
    void openFolderDialog();
    void openSettingsFile();

private:
    void resetUI();
    void checkFiles(const QString &dir_path);

    Ui::LoadSimulationTab *ui_;

    QString settings_file_path_;
    QString system_file_path_;
    QString diagnostics_file_path_;
};
