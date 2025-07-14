#include "load_simulation_tab.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QPushButton>
#include <QTimer>

#include "../../presenters/load_simulation_presenter.h"
#include "../../widgets/file_check_icon.h"
#include "./ui_load_simulation_tab.h"

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
    connect(ui_->btnOpenSettings, &QPushButton::clicked, this, &LoadSimulationTab::openSettings);
    connect(ui_->btnOpenInitialSystem,
            &QPushButton::clicked,
            this,
            &LoadSimulationTab::openInitialSystem);
    connect(ui_->btnRun, &QPushButton::clicked, this, &LoadSimulationTab::run);
}

void LoadSimulationTab::updatePreview() { ui_->oglSystemPreview->update(); }

void LoadSimulationTab::openFolderDialog() {
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath("output");
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

void LoadSimulationTab::onFileChecked(const FileType& file, const QString& path, bool result) {
    FileCheckIcon::Mode mode;

    if (result) {
        mode = FileCheckIcon::Mode::Checked;

        switch (file) {
            case FileType::Settings:
                ui_->btnOpenSettings->setEnabled(true);
                ui_->btnOpenSettings->setToolTip(path);
                ui_->lblSettingsIcon->setMode(mode);
                loadSettings(path);
                file_paths.settings_path = path;
                break;
            case FileType::System:
                ui_->btnOpenSystem->setEnabled(true);
                ui_->btnOpenSystem->setToolTip(path);
                ui_->lblSystemIcon->setMode(mode);
                file_paths.system_path = path;
                ui_->oglSystemPreview->initializeFromFile(path);
                ui_->btnRun->setEnabled(true);
                break;
            case FileType::DiagnosticsData:
                ui_->lblDiagnosticsIcon->setMode(mode);
                file_paths.diagnostics_data_path = path;
                ui_->btnRun->setEnabled(true);
                break;
            default:
                break;
        }

        return;
    }

    mode = FileCheckIcon::Mode::Corrupted;

    switch (file) {
        case FileType::Settings:
            ui_->lblSettingsIcon->setMode(mode);
            break;
        case FileType::System:
            ui_->lblSystemIcon->setMode(mode);
            break;
        case FileType::DiagnosticsData:
            ui_->lblDiagnosticsIcon->setMode(mode);
            break;
        default:
            break;
    }
}

void LoadSimulationTab::openSettings() {
    QUrl file_url = QUrl::fromLocalFile(file_paths.settings_path);

    QDesktopServices::openUrl(file_url);
}

void LoadSimulationTab::openInitialSystem() {}

void LoadSimulationTab::run() {}

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

    file_paths = FilePaths();
}

void LoadSimulationTab::loadSettings(const QString& file_path) {
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);

    QStringList keys = in.readLine().split(',');
    QStringList values = in.readLine().split(',');

    QMap<QString, QString> setting_value;
    for (size_t i = 0; i < keys.size(); i++) {
        setting_value[keys[i]] = values[i];
    }

    ui_->lblN->setText(setting_value["N"]);
    ui_->lblDuration->setText(setting_value["Duration"]);
    ui_->lblGenerationMethod->setText(setting_value["GenerationMethod"]);
    ui_->lblSimulationMethod->setText(setting_value["SimulationMethod"]);

    file.close();

    return;
}

void LoadSimulationTab::checkFiles(const QString& dir_path) {
    QVector<FileType> files;

    // Settings file
    QString settings_path = QDir(dir_path).filePath("settings.csv");
    QFileInfo settings_file(settings_path);
    if (settings_file.exists()) {
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::Settings);
    }

    // System file
    QString system_path = QDir(dir_path).filePath("system.csv");
    QFileInfo system_file(system_path);
    if (system_file.exists()) {
        ui_->lblInitialSystemIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::System);
    }

    // Diagnostics file
    QString diagnostics_path = QDir(dir_path).filePath("diagnostics_data.csv");
    QFileInfo diagnostics_file(diagnostics_path);
    if (diagnostics_file.exists()) {
        ui_->lblDiagnosticsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::DiagnosticsData);
    }

    if (!files.isEmpty()) emit requestFilesCheck(files, dir_path);
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

    file_paths = FilePaths();
}
