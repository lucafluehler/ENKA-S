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
#include <memory>

#include "core/settings/generation_method.h"
#include "core/settings/settings.h"
#include "forms/new_simulation_tab/ui_new_simulation_tab.h"
#include "settings_widgets/generation/collision_model_schema.h"
#include "settings_widgets/generation/file_schema.h"
#include "settings_widgets/generation/normal_sphere_schema.h"
#include "settings_widgets/generation/plummer_sphere_schema.h"
#include "settings_widgets/generation/spiral_galaxy_schema.h"
#include "settings_widgets/generation/uniform_cube_schema.h"
#include "settings_widgets/generation/uniform_sphere_schema.h"
#include "settings_widgets/settings_widget.h"
#include "settings_widgets/simulation/barneshut_leapfrog_schema.h"
#include "settings_widgets/simulation/euler_schema.h"
#include "settings_widgets/simulation/hermite_schema.h"
#include "settings_widgets/simulation/hits_schema.h"
#include "settings_widgets/simulation/leapfrog_schema.h"
#include "widgets/file_check_icon.h"

NewSimulationTab::NewSimulationTab(QWidget* parent)
    : QWidget(parent), ui_(new Ui::NewSimulationTab) {
    ui_->setupUi(this);
    setupSettingsWidgets();
    setupMethodSelection();
    setupSimulationProgressElements();
    resetSettings();

    // Other signals
    connect(ui_->btnResetSettings, &QPushButton::clicked, this, &NewSimulationTab::resetSettings);
    connect(
        ui_->btnStartSimulation, &QPushButton::clicked, this, &NewSimulationTab::startSimulation);
    connect(ui_->btnOpenLiveWindow,
            &QPushButton::clicked,
            this,
            &NewSimulationTab::requestOpenSimulationWindow);
    connect(
        ui_->btnLoadSettings, &QPushButton::clicked, this, &NewSimulationTab::openSettingsDialog);
}

NewSimulationTab::~NewSimulationTab() { abortSimulation(); }

void NewSimulationTab::setupSettingsWidgets() {
    // Setup generation settings schemas
    auto& gen = generation_settings_schemas_;
    auto normalSphereSchema = std::make_shared<NormalSphereSchema>();
    gen[GenerationMethod::NormalSphere] = normalSphereSchema;
    ui_->swiGenerationSettings->setSchema(normalSphereSchema->settingsSchema());
    ui_->oglSystemPreview->initializeProcedural(GenerationMethod::NormalSphere);
    gen[GenerationMethod::UniformCube] = std::make_shared<UniformCubeSchema>();
    gen[GenerationMethod::UniformSphere] = std::make_shared<UniformSphereSchema>();
    gen[GenerationMethod::PlummerSphere] = std::make_shared<PlummerSphereSchema>();
    gen[GenerationMethod::CollisionModel] = std::make_shared<CollisionModelSchema>();
    gen[GenerationMethod::SpiralGalaxy] = std::make_shared<SpiralGalaxySchema>();
    gen[GenerationMethod::File] = std::make_shared<FileSchema>();

    connect(ui_->swiGenerationSettings,
            &SettingsWidget::settingChanged,
            this,
            [this](SettingKey key, const QVariant& value) {
                if (key == SettingKey::FilePath) {
                    initial_system_path_ = value.toString();
                    emit checkInitialSystemFile();
                }
            });

    // Setup simulation settings schemas
    auto& sim = simulation_settings_schemas_;
    auto eulerSchema = std::make_shared<EulerSchema>();
    sim[SimulationMethod::Euler] = eulerSchema;
    ui_->swiSimulationSettings->setSchema(eulerSchema->settingsSchema());
    sim[SimulationMethod::Leapfrog] = std::make_shared<LeapfrogSchema>();
    sim[SimulationMethod::Hermite] = std::make_shared<HermiteSchema>();
    sim[SimulationMethod::Hits] = std::make_shared<HitsSchema>();
    sim[SimulationMethod::BarnesHutLeapfrog] = std::make_shared<BarnesHutLeapfrogSchema>();
}

void NewSimulationTab::setupMethodSelection() {
    // Populate Generation Method combobox
    for (const auto& [method, schema] : simulation_settings_schemas_.asKeyValueRange()) {
        ui_->cobSimulationMethod->addItem(schema->name(), QVariant::fromValue(method));
    }

    // Populate Simulation Method combobox
    for (const auto& [method, schema] : generation_settings_schemas_.asKeyValueRange()) {
        ui_->cobGenerationMethod->addItem(schema->name(), QVariant::fromValue(method));
    }

    // Handle selection changes
    connect(ui_->cobGenerationMethod,
            &QComboBox::currentIndexChanged,
            this,
            &NewSimulationTab::onGenerationMethodChanged);
    connect(ui_->cobSimulationMethod,
            &QComboBox::currentIndexChanged,
            this,
            &NewSimulationTab::onSimulationMethodChanged);
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

void NewSimulationTab::resetSettings() {
    auto generation_method = ui_->cobGenerationMethod->currentData().value<GenerationMethod>();
    auto stored_generation_settings = stored_generation_settings_.value(generation_method);
    ui_->swiGenerationSettings->setSettings(stored_generation_settings);

    auto simulation_method = ui_->cobSimulationMethod->currentData().value<SimulationMethod>();
    auto stored_simulation_settings = stored_simulation_settings_.value(simulation_method);
    ui_->swiSimulationSettings->setSettings(stored_simulation_settings);
}

void NewSimulationTab::loadSettings(const Settings& settings) {
    // Stored loaded generation settings as new default settings
    const auto& generation_method = settings.get<GenerationMethod>(SettingKey::GenerationMethod);
    stored_generation_settings_[generation_method] = settings;

    // Update generation method combobox
    int index = ui_->cobGenerationMethod->findData(QVariant::fromValue(generation_method));
    if (index != -1) {
        ui_->cobGenerationMethod->setCurrentIndex(index);
    }

    // Store loaded simulation settings as new default settings
    const auto& simulation_method = settings.get<SimulationMethod>(SettingKey::SimulationMethod);
    stored_simulation_settings_[simulation_method] = settings;

    // Update simulation method combobox
    index = ui_->cobSimulationMethod->findData(QVariant::fromValue(simulation_method));
    if (index != -1) {
        ui_->cobSimulationMethod->setCurrentIndex(index);
    }
}

void NewSimulationTab::onGenerationMethodChanged(int new_index) {
    // Store current settings before changing the method
    const auto current_method = ui_->cobGenerationMethod->currentData().value<GenerationMethod>();
    stored_generation_settings_[current_method] = ui_->swiGenerationSettings->getSettings();

    // Update settings widget based on the new method
    const auto& method = ui_->cobGenerationMethod->itemData(new_index).value<GenerationMethod>();
    const auto& schema = generation_settings_schemas_[method];

    if (!schema) return;

    ui_->swiGenerationSettings->setSchema(schema->settingsSchema());
    const auto& stored_settings = stored_generation_settings_.value(method);
    ui_->swiGenerationSettings->setSettings(stored_settings);

    // Update system preview
    if (method == GenerationMethod::File) {
        if (initial_system_path_.isEmpty()) {
            ui_->oglSystemPreview->clearPreview();
        } else {
            ui_->oglSystemPreview->initializeFromFile(initial_system_path_);
        }
    } else {
        ui_->oglSystemPreview->initializeProcedural(method);
    }
}

void NewSimulationTab::onSimulationMethodChanged(int new_index) {
    // Store current settings before changing the method
    const auto current_method = ui_->cobSimulationMethod->currentData().value<SimulationMethod>();
    stored_simulation_settings_[current_method] = ui_->swiSimulationSettings->getSettings();

    // Update settings widget based on the new method
    const auto& method = ui_->cobSimulationMethod->itemData(new_index).value<SimulationMethod>();
    const auto& schema = simulation_settings_schemas_[method];

    if (!schema) return;

    ui_->swiSimulationSettings->setSchema(schema->settingsSchema());
    const auto& stored_settings = stored_simulation_settings_.value(method);
    ui_->swiSimulationSettings->setSettings(stored_settings);
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
        loadSettings(*settings);
        resetSettings();
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Checked);
    } else {
        ui_->lblSettingsIcon->setMode(FileCheckIcon::Mode::Corrupted);
    }
}

void NewSimulationTab::processInitialSystem(const std::optional<enkas::data::System>& system) {
    if (system.has_value()) {
        // ui_->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Checked);
        ui_->oglSystemPreview->initializeFromFile(initial_system_path_);
    } else {
        // ui_->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Corrupted);
        initial_system_path_.clear();
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

void NewSimulationTab::updatePreview() { ui_->oglSystemPreview->update(); }

Settings NewSimulationTab::fetchSettings() const {
    const auto& generation_method =
        ui_->cobGenerationMethod->currentData().value<GenerationMethod>();
    const auto& simulation_method =
        ui_->cobSimulationMethod->currentData().value<SimulationMethod>();

    // Fetch general settings
    auto settings = Settings::create(  //
                        {{SettingKey::GenerationMethod, generation_method},
                         {SettingKey::SimulationMethod, simulation_method},
                         {SettingKey::Duration, ui_->dsbSimDuration->value()},
                         {SettingKey::SystemDataStep, ui_->dsbSystemTimeStep->value()},
                         {SettingKey::DiagnosticsDataStep, ui_->dsbDiagnosticsTimeStep->value()},
                         {SettingKey::SaveSystemData, ui_->cbxSaveSystem->isChecked()},
                         {SettingKey::SaveDiagnosticsData, ui_->cbxSaveDiagnostics->isChecked()},
                         {SettingKey::SaveSettings, ui_->cbxSaveSettings->isChecked()}})
                        .value();

    // Fetch generation settings
    settings.merge(ui_->swiGenerationSettings->getSettings());

    // Fetch simulation settings
    settings.merge(ui_->swiSimulationSettings->getSettings());

    return settings;
}

QString NewSimulationTab::getInitialSystemPath() const { return initial_system_path_; }

QString NewSimulationTab::getSettingsPath() const { return settings_path_; }
