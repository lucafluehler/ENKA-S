#include "load_simulation_tab.h"

#include <qmessagebox.h>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>

#include "core/files/file_constants.h"
#include "core/settings/generation_method.h"
#include "forms/load_simulation_tab/ui_load_simulation_tab.h"
#include "widgets/file_check_icon.h"

LoadSimulationTab::LoadSimulationTab(QWidget* parent)
    : QWidget(parent), ui_(new Ui::LoadSimulationTab) {
    ui_->setupUi(this);

    // More ui adjustments
    ui_->btnOpenSettings->setEnabled(false);
    ui_->btnRun->setEnabled(false);

    // Signal management
    connect(ui_->btnLoadSimulationData,
            &QPushButton::clicked,
            this,
            &LoadSimulationTab::openFolderDialog);
    connect(
        ui_->btnOpenSettings, &QPushButton::clicked, this, &LoadSimulationTab::openSettingsFile);
}

void LoadSimulationTab::openFolderDialog() {
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString folder_path = QFileDialog::getExistingDirectory(
        this,
        "Simulations-Ordner wählen...",
        open_this_path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (folder_path.isEmpty()) return;

    resetUI();

    QString folder_name = QDir(folder_path).dirName();

    // Ensure the folder name is not too long when displayed
    QString n = folder_name.left(24).append(folder_name.length() > 24 ? "..." : "");
    ui_->lblFolderName->setText(n);
    ui_->lblFolderName->setToolTip(folder_path);

    checkFiles(folder_path);
}

void LoadSimulationTab::openSettingsFile() {
    if (settings_file_path_.isEmpty()) return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(settings_file_path_));
}

void LoadSimulationTab::onSettingsParsed(std::optional<Settings> settings) {
    if (settings.has_value()) {
        ui_->btnOpenSettings->setEnabled(true);
        ui_->btnOpenSettings->setToolTip(settings_file_path_);
        ui_->lblSettingsStatusIcon->setMode(FileCheckIcon::Mode::Checked);

        auto gen_method = settings->get<GenerationMethod>(SettingKey::GenerationMethod);
        auto gen_method_string =
            QString::fromStdString(std::string(generationMethodToString(gen_method)));
        ui_->lblGenerationMethod->setText(gen_method_string);

        auto sim_method = settings->get<SimulationMethod>(SettingKey::SimulationMethod);
        auto sim_method_string =
            QString::fromStdString(std::string(simulationMethodToString(sim_method)));
        ui_->lblSimulationMethod->setText(sim_method_string);
    } else {
        ui_->lblSettingsStatusIcon->setMode(FileCheckIcon::Mode::Corrupted);
        ui_->btnOpenSettings->setToolTip("");
        ui_->btnOpenSettings->setEnabled(false);
        settings_file_path_.clear();
    }
}

void LoadSimulationTab::onInitialSystemParsed(std::optional<enkas::data::System> system) {
    if (system.has_value()) {
        ui_->lblSystemStatusIcon->setMode(FileCheckIcon::Mode::Checked);
        ui_->oglSystemPreview->initializeSystem(*system);
        ui_->btnRun->setEnabled(true);
    } else {
        ui_->lblSystemStatusIcon->setMode(FileCheckIcon::Mode::Corrupted);
    }
}

void LoadSimulationTab::onDiagnosticsSeriesParsed(bool success) {
    if (success) {
        ui_->lblDiagnosticsStatusIcon->setMode(FileCheckIcon::Mode::Checked);
        ui_->btnRun->setEnabled(true);
    } else {
        ui_->lblDiagnosticsStatusIcon->setMode(FileCheckIcon::Mode::Corrupted);
    }
}

QVector<QString> LoadSimulationTab::getFilesToCheck() const {
    QVector<QString> file_paths;

    if (!settings_file_path_.isEmpty()) {
        file_paths.append(settings_file_path_);
    }

    if (!system_file_path_.isEmpty()) {
        file_paths.append(system_file_path_);
    }

    if (!diagnostics_file_path_.isEmpty()) {
        file_paths.append(diagnostics_file_path_);
    }

    return file_paths;
}

void LoadSimulationTab::checkFiles(const QString& dir_path) {
    bool any_file_found = false;

    // Settings file
    QString settings_path = QDir(dir_path).filePath(file_names::settings);
    QFileInfo settings_file(settings_path);
    if (settings_file.exists()) {
        ui_->lblSettingsStatusIcon->setMode(FileCheckIcon::Mode::Loading);
        settings_file_path_ = settings_path;
        any_file_found = true;
    }

    // System file
    QString system_path = QDir(dir_path).filePath(file_names::system);
    QFileInfo system_file(system_path);
    if (system_file.exists()) {
        ui_->lblSystemStatusIcon->setMode(FileCheckIcon::Mode::Loading);
        system_file_path_ = system_path;
        any_file_found = true;
    }

    // Diagnostics file
    QString diagnostics_path = QDir(dir_path).filePath(file_names::diagnostics);
    QFileInfo diagnostics_file(diagnostics_path);
    if (diagnostics_file.exists()) {
        ui_->lblDiagnosticsStatusIcon->setMode(FileCheckIcon::Mode::Loading);
        diagnostics_file_path_ = diagnostics_path;
        any_file_found = true;
    }

    if (any_file_found) emit requestFilesCheck();
}

void LoadSimulationTab::resetUI() {
    // Reset settings data
    ui_->lblGenerationMethod->setText("");
    ui_->lblSimulationMethod->setText("");

    // Disable buttons and reset tooltip to path
    ui_->btnOpenSettings->setEnabled(false);
    ui_->btnOpenSettings->setToolTip("");
    ui_->btnRun->setEnabled(false);

    // Reset icons
    auto mode = FileCheckIcon::Mode::NotFound;
    ui_->lblSettingsStatusIcon->setMode(mode);
    ui_->lblSystemStatusIcon->setMode(mode);
    ui_->lblDiagnosticsStatusIcon->setMode(mode);

    // Reset file paths
    settings_file_path_.clear();
    system_file_path_.clear();
    diagnostics_file_path_.clear();

    // Reset the system preview
    ui_->oglSystemPreview->clearPreview();
}
