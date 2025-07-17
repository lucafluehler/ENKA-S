#include "new_simulation_tab.h"

#include <enkas/data/system.h>

#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScreen>
#include <QStackedWidget>

#include "core/settings.h"
#include "forms/new_simulation_tab/ui_new_simulation_tab.h"
#include "settings_widgets/settings_widget.h"
#include "settings_widgets/simulation/euler_settings_widget.h"
#include "widgets/file_check_icon.h"

NewSimulationTab::NewSimulationTab(QWidget* parent)
    : QWidget(parent), ui_(new Ui::NewSimulationTab) {
    ui_->setupUi(this);
    setupSettingsWidgets();
    setupMethodSelection();
    initializePreview();
    setupSimulationProgressElements();
    resetSettings();
    setupFileManagement();

    // Other signals
    connect(ui_->btnResetSettings, &QPushButton::clicked, this, &NewSimulationTab::resetSettings);
    connect(
        ui_->btnStartSimulation, &QPushButton::clicked, this, &NewSimulationTab::startSimulation);
}

NewSimulationTab::~NewSimulationTab() { abortSimulation(); }

void NewSimulationTab::setupSettingsWidgets() {
    simulation_settings_widgets_.insert(enkas::simulation::Method::Euler,
                                        new EulerSettingsWidget(ui_->stwSimulationSettings));
}

void NewSimulationTab::setupMethodSelection() {
    // Populate Generation Method combobox
    for (const auto method : simulation_settings_widgets_.keys()) {
        ui_->cobSimulationMethod->addItem(
            QString::fromStdString(std::string(enkas::simulation::methodToString(method))),
            QVariant::fromValue(method));
    }

    // Populate Simulation Method combobox
    for (const auto method : generation_settings_widgets_.keys()) {
        ui_->cobGenerationMethod->addItem(
            QString::fromStdString(std::string(enkas::generation::methodToString(method))),
            QVariant::fromValue(method));
    }

    // Connect stacked settings widgets to combobox changes
    connect(ui_->cobGenerationMethod,
            &QComboBox::activated,
            ui_->stwGenerationSettings,
            &QStackedWidget::setCurrentIndex);
    connect(ui_->cobSimulationMethod,
            &QComboBox::activated,
            ui_->stwSimulationSettings,
            &QStackedWidget::setCurrentIndex);
}

void NewSimulationTab::initializePreview() {
    ui_->oglNormalSpherePreview->initializeProcedural(enkas::generation::Method::NormalSphere);
    ui_->oglUniformCubePreview->initializeProcedural(enkas::generation::Method::UniformCube);
    ui_->oglUniformSpherePreview->initializeProcedural(enkas::generation::Method::UniformSphere);
    ui_->oglPlummerPreview->initializeProcedural(enkas::generation::Method::PlummerSphere);
    ui_->oglSpiralGalaxyPreview->initializeProcedural(enkas::generation::Method::SpiralGalaxy);
    ui_->oglCollisionPreview->initializeProcedural(enkas::generation::Method::CollisionModel);
}

void NewSimulationTab::setupSimulationProgressElements() {
    ui_->stwProgress->setCurrentIndex(0);
    ui_->lblPreparationLoading->setMode(FileCheckIcon::Mode::Loading);
    ui_->lblAbortionLoading->setMode(FileCheckIcon::Mode::Loading);

    connect(
        ui_->btnAbortPreparation, &QPushButton::clicked, this, &NewSimulationTab::abortSimulation);
    connect(
        ui_->btnAbortSimulation, &QPushButton::clicked, this, &NewSimulationTab::abortSimulation);
}

void NewSimulationTab::setupFileManagement() {
    connect(ui_->btnLoadSystemData,
            &QPushButton::clicked,
            this,
            &NewSimulationTab::openSystemDataDialog);
    connect(
        ui_->btnLoadSettings, &QPushButton::clicked, this, &NewSimulationTab::openSettingsDialog);
}

void NewSimulationTab::resetSettings() {
    for (auto* widget : simulation_settings_widgets_) {
        widget->resetSettings();
    }

    for (auto* widget : generation_settings_widgets_) {
        widget->resetSettings();
    }
}

void NewSimulationTab::updateDefaultSettings(const Settings& settings) {
    for (auto* widget : simulation_settings_widgets_) {
        widget->setDefaultSettings(settings);
    }

    for (auto* widget : generation_settings_widgets_) {
        widget->setDefaultSettings(settings);
    }
}

void NewSimulationTab::openSystemDataDialog() {
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath("output");
    if (QDir(output_path).exists()) open_this_path = output_path;

    QString system_data_path = QFileDialog::getOpenFileName(
        nullptr, "Open CSV File", open_this_path, "CSV system file (system.csv)");

    if (system_data_path.isEmpty()) {
        initial_system_path_.clear();
        ui_->lblSystemDataPath->setText("");
        ui_->lblSystemDataPath->setToolTip("");
        ui_->lblSystemDataIcon->setMode(FileCheckIcon::Mode::NotFound);
        return;
    }

    QString n = QFileInfo(system_data_path).dir().dirName();
    ui_->lblSystemDataPath->setText(n.left(36).append(n.length() > 36 ? "..." : ""));
    ui_->lblSystemDataPath->setToolTip(system_data_path);

    ui_->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Loading);

    initial_system_path_ = system_data_path;
    emit checkInitialSystemFile();
}

void NewSimulationTab::openSettingsDialog() {
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath("output");
    if (QDir(output_path).exists()) open_this_path = output_path;

    QString settings_path = QFileDialog::getOpenFileName(
        nullptr, "Open JSON File", open_this_path, "JSON settings file (settings.json)");

    if (settings_path.isEmpty()) {
        settings_path_.clear();
        ui_->lblSettingsPath->setText("");
        ui_->lblSettingsPath->setToolTip("");
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::NotFound);
        return;
    }

    QString n = QFileInfo(settings_path).dir().dirName();
    ui_->lblSettingsPath->setText(n.left(28).append(n.length() > 28 ? "..." : ""));
    ui_->lblSettingsPath->setToolTip(settings_path);

    ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Loading);

    settings_path_ = settings_path;
    emit checkSettingsFile();
}

void NewSimulationTab::processSettings(const std::optional<Settings>& settings) {
    if (settings.has_value()) {
        updateDefaultSettings(*settings);
        resetSettings();
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Checked);
    } else {
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Corrupted);
    }
}

void NewSimulationTab::processInitialSystem(const std::optional<enkas::data::System>& system) {
    if (system.has_value()) {
        ui_->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Checked);
        ui_->oglFilePreview->initializeFromFile(initial_system_path_);
    } else {
        ui_->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Corrupted);
    }
}

void NewSimulationTab::startSimulation() {
    emit requestSimulationStart();

    // Show fancy simulation preparation loading gif
    ui_->stwProgress->setCurrentIndex(1);
}

void NewSimulationTab::showSimulationProgress() { ui_->stwProgress->setCurrentIndex(2); }

void NewSimulationTab::abortSimulation() {
    emit requestSimulationAbort();

    // Show abortion loading gif
    ui_->stwProgress->setCurrentIndex(3);
}

void NewSimulationTab::simulationAborted() { ui_->stwProgress->setCurrentIndex(0); }

void NewSimulationTab::updateSimulationProgress(double time, double duration) {
    QString time_text = QString("%1 / %2").arg(time).arg(duration);
    ui_->lblSimulationProgress->setText(time_text);
    ui_->pbaSimulationProgress->setValue(time / duration * 100000);
}

void NewSimulationTab::updatePreview() {
    auto method = ui_->cobGenerationMethod->currentData().value<enkas::generation::Method>();

    switch (method) {
        case enkas::generation::Method::File:
            if (!initial_system_path_.isEmpty()) {
                ui_->oglFilePreview->update();
            }
            break;
        case enkas::generation::Method::NormalSphere:
            ui_->oglNormalSpherePreview->update();
            break;
        case enkas::generation::Method::UniformCube:
            ui_->oglUniformCubePreview->update();
            break;
        case enkas::generation::Method::UniformSphere:
            ui_->oglUniformSpherePreview->update();
            break;
        case enkas::generation::Method::PlummerSphere:
            ui_->oglPlummerPreview->update();
            break;
        case enkas::generation::Method::SpiralGalaxy:
            ui_->oglSpiralGalaxyPreview->update();
            break;
        case enkas::generation::Method::CollisionModel:
            ui_->oglCollisionPreview->update();
            break;
        default:
            break;
    }
}

Settings NewSimulationTab::fetchSettings() const {
    const auto& generation_method_str = ui_->cobGenerationMethod->currentText();
    const auto& simulation_method_str = ui_->cobSimulationMethod->currentText();

    // Fetch general settings
    auto settings = Settings::create(  //
                        {"GenerationMethod", generation_method_str},
                        {"SimulationMethod", simulation_method_str},
                        {"Duration", ui_->dsbSimDuration->value()},
                        {"SystemDataStep", ui_->dsbSystemTimeStep->value()},
                        {"DiagnosticsDataStep", ui_->dsbDiagnosticsTimeStep->value()},
                        {"SaveSystemData", ui_->cbxSaveSystem->isChecked()},
                        {"SaveDiagnosticsData", ui_->cbxSaveDiagnostics->isChecked()},
                        {"SaveSettings", ui_->cbxSaveSettings->isChecked()})
                        .value();

    const auto& generation_method =
        ui_->cobGenerationMethod->currentData().value<enkas::generation::Method>();
    const auto& simulation_method =
        ui_->cobSimulationMethod->currentData().value<enkas::simulation::Method>();

    // Fetch generation settings
    auto* generation_settings_widget = generation_settings_widgets_.value(generation_method);
    if (generation_settings_widget) {
        settings.merge(generation_settings_widget->getSettings());
    }

    // Fetch simulation settings
    auto* simulation_settings_widget = simulation_settings_widgets_.value(simulation_method);
    if (simulation_settings_widget) {
        settings.merge(simulation_settings_widget->getSettings());
    }

    return settings;
}

QString NewSimulationTab::getInitialSystemPath() const { return initial_system_path_; }

QString NewSimulationTab::getSettingsPath() const { return settings_path_; }
