#include "load_simulation_tab.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QPushButton>
#include <QTimer>

#include "./ui_load_simulation_tab.h"
#include "core/file_constants.h"
#include "presenters/load_simulation_presenter.h"
#include "widgets/file_check_icon.h"

LoadSimulationTab::LoadSimulationTab(QWidget* parent)
    : QWidget(parent), ui_(new Ui::LoadSimulationTab) {
    ui_->setupUi(this);

    // More ui adjustments
    ui_->btnOpenSettings->setEnabled(false);
    ui_->btnOpenInitialSystem->setEnabled(false);
    ui_->btnRun->setEnabled(false);

    // File checks
    connect(this,
            &LoadSimulationTab::requestFilesCheck,
            presenter_,
            &LoadSimulationPresenter::checkFiles);

    // Signal management
    connect(ui_->btnLoadSimulationData,
            &QPushButton::clicked,
            this,
            &LoadSimulationTab::openFolderDialog);
}

void LoadSimulationTab::updateInitialSystemPreview() { ui_->oglSystemPreview->update(); }

void LoadSimulationTab::openFolderDialog() {
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath(folder_names::output);
    if (QDir(output_path).exists()) open_this_path = output_path;

    QString folder_path = QFileDialog::getExistingDirectory(
        this,
        "Simulations-Ordner wählen...",
        open_this_path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (folder_path.isEmpty()) return;

    QString folder_name = QDir(folder_path).dirName();

    // Ensure the folder name is not too long when displayed
    QString n = folder_name.left(24).append(folder_name.length() > 24 ? "..." : "");
    ui_->lblFolderName->setText(n);
    ui_->lblFolderName->setToolTip(folder_path);

    checkFiles(folder_path);
}

void LoadSimulationTab::onSettingsParsed(bool success) {
    if (success) {
        ui_->btnOpenSettings->setEnabled(true);
        ui_->btnOpenSettings->setToolTip(settings_file_path_);
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Checked);
    } else {
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Corrupted);
        ui_->btnOpenSettings->setEnabled(false);
    }
}

void LoadSimulationTab::onInitialSystemParsed(std::optional<enkas::data::System> system) {
    if (system.has_value()) {
        ui_->btnOpenInitialSystem->setEnabled(true);
        ui_->btnOpenInitialSystem->setToolTip(system_file_path_);
        ui_->lblInitialSystemIcon->setMode(FileCheckIcon::Mode::Checked);
        ui_->oglSystemPreview->initializeFromFile(system_file_path_);
        ui_->btnRun->setEnabled(true);
    } else {
        ui_->lblInitialSystemIcon->setMode(FileCheckIcon::Mode::Corrupted);
        ui_->btnOpenInitialSystem->setEnabled(false);
    }
}

void LoadSimulationTab::onDiagnosticsSeriesParsed(bool success) {
    if (success) {
        ui_->lblDiagnosticsIcon->setMode(FileCheckIcon::Mode::Checked);
        ui_->btnRun->setEnabled(true);
    } else {
        ui_->lblDiagnosticsIcon->setMode(FileCheckIcon::Mode::Corrupted);
    }
}

void LoadSimulationTab::resetSimulationFilePaths() {
    // Reset settings data
    ui_->lblN->setText("");
    ui_->lblDuration->setText("");
    ui_->lblGenerationMethod->setText("");
    ui_->lblSimulationMethod->setText("");

    // Disable buttons and reset tooltip to path
    ui_->btnOpenInitialSystem->setEnabled(false);
    ui_->btnOpenInitialSystem->setToolTip("");
    ui_->btnOpenSettings->setEnabled(false);
    ui_->btnOpenSettings->setToolTip("");

    // Reset icons
    auto mode = FileCheckIcon::Mode::NotFound;
    ui_->lblSettingsIcon->setMode(mode);
    ui_->lblInitialSystemIcon->setMode(mode);
    ui_->lblRenderIcon->setMode(mode);
    ui_->lblDiagnosticsIcon->setMode(mode);
    ui_->lblAnalyticsIcon->setMode(mode);
}

void LoadSimulationTab::checkFiles(const QString& dir_path) {
    QVector<QString> file_paths;

    // Settings file
    QString settings_path = QDir(dir_path).filePath(file_names::settings);
    QFileInfo settings_file(settings_path);
    if (settings_file.exists()) {
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(settings_path);
        settings_file_path_ = settings_path;
    }

    // System file
    QString system_path = QDir(dir_path).filePath(file_names::system);
    QFileInfo system_file(system_path);
    if (system_file.exists()) {
        ui_->lblInitialSystemIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(system_path);
        system_file_path_ = system_path;
    }

    // Diagnostics file
    QString diagnostics_path = QDir(dir_path).filePath(file_names::diagnostics);
    QFileInfo diagnostics_file(diagnostics_path);
    if (diagnostics_file.exists()) {
        ui_->lblDiagnosticsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(diagnostics_path);
        diagnostics_file_path_ = diagnostics_path;
    }

    if (!files.isEmpty()) emit requestFilesCheck(file_paths);
}

void LoadSimulationTab::resetSimulationFilePaths() {
    // Reset settings data
    ui_->lblN->setText("");
    ui_->lblDuration->setText("");
    ui_->lblGenerationMethod->setText("");
    ui_->lblSimulationMethod->setText("");

    // Disable buttons and reset tooltip to path
    ui_->btnOpenInitialSystem->setEnabled(false);
    ui_->btnOpenInitialSystem->setToolTip("");
    ui_->btnOpenSettings->setEnabled(false);
    ui_->btnOpenSettings->setToolTip("");

    // Reset icons
    auto mode = FileCheckIcon::Mode::NotFound;
    ui_->lblSettingsIcon->setMode(mode);
    ui_->lblInitialSystemIcon->setMode(mode);
    ui_->lblRenderIcon->setMode(mode);
    ui_->lblDiagnosticsIcon->setMode(mode);
    ui_->lblAnalyticsIcon->setMode(mode);

    // Reset file paths
    settings_file_path_.clear();
    system_file_path_.clear();
    diagnostics_file_path_.clear();
}
