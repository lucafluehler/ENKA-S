#include <QFileDialog>
#include <QDesktopServices>
#include <QTimer>

#include "load_simulation_tab.h"
#include "./ui_load_simulation_tab.h"

LoadSimulationTab::LoadSimulationTab(QWidget *parent)
    : QWidget(parent)
    , preview_timer(new QTimer)
    , ui(new Ui::LoadSimulationTab)
{
    ui->setupUi(this);

    // More ui adjustments
    ui->btnOpenSettings->setEnabled(false);
    ui->btnOpenInitialSystem->setEnabled(false);
    ui->btnRun->setEnabled(false);

    // Initialize file checker
    file_checker = new FileChecker();
    file_checker_thread = new QThread(this);
    file_checker->moveToThread(file_checker_thread);

    connect( this, &LoadSimulationTab::requestFilesCheck
           , file_checker, &FileChecker::checkFiles);
    connect( file_checker, &FileChecker::fileChecked
           , this, &LoadSimulationTab::onFileChecked );
    connect( file_checker, &FileChecker::filesChecked
            , this, &LoadSimulationTab::onFilesChecked );

    file_checker_thread->start();

    // Signal management
    connect( ui->btnLoadSimulationData, &QPushButton::clicked
           , this, &LoadSimulationTab::openFolderDialog );
    connect( ui->btnOpenSettings, &QPushButton::clicked
            , this, &LoadSimulationTab::openSettings );
    connect( ui->btnOpenInitialSystem, &QPushButton::clicked
            , this, &LoadSimulationTab::openInitialSystem );
    connect( ui->btnRun, &QPushButton::clicked
            , this, &LoadSimulationTab::run );

    // System preview
    connect(preview_timer, &QTimer::timeout, this, &LoadSimulationTab::updatePreview);
}

LoadSimulationTab::~LoadSimulationTab()
{
    file_checker_thread->quit();
    file_checker_thread->wait();
}

void LoadSimulationTab::onTabSelected()
{
    preview_timer->start(1000/30);
}

void LoadSimulationTab::onTabClosed()
{
    preview_timer->stop();
}


void LoadSimulationTab::updatePreview()
{
    ui->oglSystemPreview->update();
}

void LoadSimulationTab::openFolderDialog()
{
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath("output");
    if (QDir(output_path).exists()) open_this_path = output_path;

    QString folder_path =
        QFileDialog::getExistingDirectory( this, "Simulations-Ordner wählen..."
                                         , open_this_path
                                         ,   QFileDialog::ShowDirsOnly
                                           | QFileDialog::DontResolveSymlinks );

    if (folder_path.isEmpty()) return;

    QString folder_name = QDir(folder_path).dirName();

    QString n = folder_name.left(24).append(folder_name.length() > 24 ? "..." : "");
    ui->lblFolderName->setText(n);
    ui->lblFolderName->setToolTip(folder_path);

    checkFiles(folder_path);
}

void LoadSimulationTab::onFileChecked( FileType file, const QString& path
                                     , bool result )
{
    FileCheckIcon::Mode mode;

    if (result) {
        mode = FileCheckIcon::Mode::Checked;

        switch (file) {
        case FileType::Settings:
            ui->btnOpenSettings->setEnabled(true);
            ui->btnOpenSettings->setToolTip(path);
            ui->lblSettingsIcon->setMode(mode);
            loadSettings(path);
            file_paths.settings_path = path;
            break;
        case FileType::InitialSystem:
            ui->btnOpenInitialSystem->setEnabled(true);
            ui->btnOpenInitialSystem->setToolTip(path);
            ui->lblInitialSystemIcon->setMode(mode);
            file_paths.initial_system_path = path;
            ui->oglSystemPreview->initializeMethod(GenerationMethod::File, path);
            break;
        case FileType::RenderData:
            ui->lblRenderIcon->setMode(mode);
            file_paths.render_data_path = path;
            break;
        case FileType::DiagnosticsData:
            ui->lblDiagnosticsIcon->setMode(mode);
            file_paths.diagnostics_data_path = path;
            break;
        case FileType::AnalyticsData:
            ui->lblAnalyticsIcon->setMode(mode);
            file_paths.analytics_data_path = path;
            break;
        default: break;
        }

        return;
    }

    mode = FileCheckIcon::Mode::Corrupted;

    switch (file) {
    case FileType::Settings:        ui->lblSettingsIcon->setMode(mode);      break;
    case FileType::InitialSystem:   ui->lblInitialSystemIcon->setMode(mode); break;
    case FileType::RenderData:      ui->lblRenderIcon->setMode(mode);        break;
    case FileType::DiagnosticsData: ui->lblDiagnosticsIcon->setMode(mode);   break;
    case FileType::AnalyticsData:   ui->lblAnalyticsIcon->setMode(mode);     break;
    default: break;
    }
}

void LoadSimulationTab::onFilesChecked()
{
    if (   !file_paths.render_data_path.isEmpty()
        || !file_paths.diagnostics_data_path.isEmpty()
        || !file_paths.analytics_data_path.isEmpty()) {
        ui->btnRun->setEnabled(true);
    }
}

void LoadSimulationTab::openSettings()
{
    QUrl file_url = QUrl::fromLocalFile(file_paths.settings_path);;
    QDesktopServices::openUrl(file_url);
}

void LoadSimulationTab::openInitialSystem()
{

}

void LoadSimulationTab::run()
{

}


void LoadSimulationTab::resetSimulationFilePaths()
{
    // Reset settings data
    ui->lblN->setText("");
    ui->lblDuration->setText("");
    ui->lblGenerationMethod->setText("");
    ui->lblSimulationMethod->setText("");

    // Disable buttons and reset tooltip to path
    ui->btnOpenInitialSystem->setEnabled(false);
    ui->btnOpenInitialSystem->setToolTip("");
    ui->btnOpenSettings->setEnabled(false);
    ui->btnOpenSettings->setToolTip("");

    // Reset icons
    auto mode = FileCheckIcon::Mode::NotFound;
    ui->lblSettingsIcon->setMode(mode);
    ui->lblInitialSystemIcon->setMode(mode);
    ui->lblRenderIcon->setMode(mode);
    ui->lblDiagnosticsIcon->setMode(mode);
    ui->lblAnalyticsIcon->setMode(mode);

    file_paths = FilePaths();
}

void LoadSimulationTab::loadSettings(const QString& file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);

    QStringList keys = in.readLine().split(',');
    QStringList values = in.readLine().split(',');

    QMap<QString, QString> setting_value;
    for (size_t i = 0; i < keys.size(); i++) {
        setting_value[keys[i]] = values[i];
    }

    ui->lblN->setText(setting_value["N"]);
    ui->lblDuration->setText(setting_value["Duration"]);
    ui->lblGenerationMethod->setText(setting_value["GenerationMethod"]);
    ui->lblSimulationMethod->setText(setting_value["SimulationMethod"]);

    file.close();

    return;
}

void LoadSimulationTab::checkFiles(const QString& dir_path)
{
    QVector<FileType> files;

    // Settings file
    QString settings_path = QDir(dir_path).filePath("settings.csv");
    QFileInfo settings_file(settings_path);
    if (settings_file.exists()){
        ui->lblSettingsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::Settings);
    }

    // Initial System file
    QString initial_system_path = QDir(dir_path).filePath("initial_system.csv");
    QFileInfo initial_system_file(initial_system_path);
    if (initial_system_file.exists()){
        ui->lblInitialSystemIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::InitialSystem);
    }

    // Render file
    QString render_path = QDir(dir_path).filePath("render_data.csv");
    QFileInfo render_file(render_path);
    if (render_file.exists()){
        ui->lblRenderIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::RenderData);
    }

    // Diagnostics file
    QString diagnostics_path = QDir(dir_path).filePath("diagnostics_data.csv");
    QFileInfo diagnostics_file(diagnostics_path);
    if (diagnostics_file.exists()){
        ui->lblDiagnosticsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::DiagnosticsData);
    }

    // Analytics file
    QString analytics_path = QDir(dir_path).filePath("analytics_data.csv");
    QFileInfo analytics_file(analytics_path);
    if (analytics_file.exists()){
        ui->lblAnalyticsIcon->setMode(FileCheckIcon::Mode::Loading);
        files.push_back(FileType::AnalyticsData);
    }

    if (!files.isEmpty()) emit requestFilesCheck(files, dir_path);
}
