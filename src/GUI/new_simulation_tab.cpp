#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QScreen>
#include <QRandomGenerator>

#include "new_simulation_tab.h"
#include "./ui_new_simulation_tab.h"

NewSimulationTab::NewSimulationTab(QWidget *parent)
    : QWidget(parent)
    , default_settings(Settings())
    , simulation_manager(nullptr)
    , file_checker(nullptr)
    , preview_timer(new QTimer)
    , progress_timer(new QTimer)
    , ui(new Ui::NewSimulationTab)
{
    ui->setupUi(this);

    // Initialize System preview
    setupInitialSystemPreviews();

    // Map Settings strings to their respective ui element
    mapStringsToUiPtrs();

    // Fill Comboboxes with Methods
    ui->cobGenerationMethod->addItems(Settings().getGenerationMethods().values());
    ui->cobSimulationMethod->addItems(Settings().getSimulationMethods().values());

    // Reset page indices
    ui->stwGenerationSettings->setCurrentIndex(0);
    ui->stwSimulationSettings->setCurrentIndex(0);
    ui->stwProgress->setCurrentIndex(0);

    // Show Loading animations
    ui->lblPreparationLoading->setMode(FileCheckIcon::Mode::Loading);
    ui->lblAbortionLoading->setMode(FileCheckIcon::Mode::Loading);

    // Set all settings to default values
    resetSettings();
    randomizeSeed();

    // Signal managment
    connect( ui->btnRandomizeSeed, &QPushButton::clicked
           , this, &NewSimulationTab::randomizeSeed );
    connect( ui->btnLoadSystemData, &QPushButton::clicked
           , this, &NewSimulationTab::openSystemDataDialog );
    connect( ui->btnLoadSettings, &QPushButton::clicked
           , this, &NewSimulationTab::openSettingsDialog );
    connect( ui->btnResetSettings, &QPushButton::clicked
           , this, &NewSimulationTab::resetSettings );
    connect( ui->btnStartSimulation, &QPushButton::clicked
           , this, &NewSimulationTab::createSimulationManager );

    connect( ui->btnAbortPreparation, &QPushButton::clicked
           , this, &NewSimulationTab::abortSimulation );
    connect( ui->btnAbortSimulation, &QPushButton::clicked
           , this, &NewSimulationTab::abortSimulation );

    connect( ui->cobGenerationMethod, &QComboBox::activated
           , ui->stwGenerationSettings, &QStackedWidget::setCurrentIndex );
    connect( ui->cobSimulationMethod, &QComboBox::activated
           , ui->stwSimulationSettings, &QStackedWidget::setCurrentIndex );

    // If any data is saved, we want to enforce the creation of a
    // simulation folder.
    connect( ui->cbxRender, &QCheckBox::stateChanged
           , this, &NewSimulationTab::checkSaveFolder );
    connect( ui->cbxDiagnostics, &QCheckBox::stateChanged
            , this, &NewSimulationTab::checkSaveFolder );
    connect( ui->cbxAnalytics, &QCheckBox::stateChanged
            , this, &NewSimulationTab::checkSaveFolder );

    // File Checker for checking loaded settings and initial system
    file_checker = new FileChecker(this);
    connect( file_checker, &FileChecker::fileChecked
            , this, &NewSimulationTab::onFileChecked );
}

NewSimulationTab::~NewSimulationTab()
{
    abortSimulation();
}


void NewSimulationTab::resetSettings()
{
    int index = 0;

    const QStringList identifiers = default_settings.getIdentifiers();

    for (const auto& identifier : identifiers) {
        Setting setting = default_settings.getSetting(identifier);

        switch (setting.type) {
        case Setting::Type::Int:
            if (auto* w = qobject_cast<QSpinBox*>(setting_ptrs[identifier])) {
                w->setValue(setting.value.toInt());
            }
            break;
        case Setting::Type::Double:
            if (auto* w = qobject_cast<QDoubleSpinBox*>(setting_ptrs[identifier])) {
                w->setValue(setting.value.toDouble());
            }
            break;
        case Setting::Type::Bool:
            if (auto* w = qobject_cast<QCheckBox*>(setting_ptrs[identifier])) {
                if (setting.value.toBool()) {
                    w->setCheckState(Qt::Checked);
                } else {
                    w->setCheckState(Qt::Unchecked);
                }
            }
            break;
        case Setting::Type::File:
            loadSystemData(setting.value.toString());
            break;
        case Setting::Type::GenerationMethod:
            ui->cobGenerationMethod->setCurrentText(setting.value.toString());
            index = ui->cobGenerationMethod->currentIndex();
            ui->stwGenerationSettings->setCurrentIndex(index);
            break;
        case Setting::Type::SimulationMethod:
            ui->cobSimulationMethod->setCurrentText(setting.value.toString());
            index = ui->cobSimulationMethod->currentIndex();
            ui->stwSimulationSettings->setCurrentIndex(index);
            break;
        default:
            break;
        }
    }
}

void NewSimulationTab::checkSaveFolder(int state)
{
    if (state == Qt::Checked) {
        ui->cbxFolder->setChecked(true);
        ui->cbxFolder->setEnabled(false);
    }

    if (   ui->cbxRender->checkState() == Qt::Unchecked
        && ui->cbxDiagnostics->checkState() == Qt::Unchecked
        && ui->cbxAnalytics->checkState() == Qt::Unchecked ) {
        ui->cbxFolder->setEnabled(true);
    }
}

void NewSimulationTab::randomizeSeed()
{
    const unsigned int c_MAX = 999999998;
    const unsigned int c_NEW_SEED = QRandomGenerator::global()->bounded(0u, c_MAX + 1);
    ui->sbxSeed->setValue(c_NEW_SEED);
}


void NewSimulationTab::openSystemDataDialog()
{
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath("output");
    if (QDir(output_path).exists()) open_this_path = output_path;

    QString system_data_path = QFileDialog::getOpenFileName(
          nullptr, "Open CSV File", open_this_path
        , "CSV initial_system file (initial_system.csv)" );

    if (system_data_path.isEmpty()) {
        initial_system_path = "";
        ui->lblSystemDataPath->setText("");
        ui->lblSystemDataPath->setToolTip("");
        ui->lblSystemDataIcon->setMode(FileCheckIcon::Mode::NotFound);
        return;
    }

    QString n = QFileInfo(system_data_path).dir().dirName();
    ui->lblSystemDataPath->setText(n.left(36).append(n.length() > 36 ? "..." : ""));
    ui->lblSystemDataPath->setToolTip(system_data_path);

    ui->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Loading);
    file_checker->checkInitialSystemFile(system_data_path);
}

void NewSimulationTab::openSettingsDialog()
{
    QString open_this_path = QCoreApplication::applicationDirPath();

    QString output_path = QDir(open_this_path).filePath("output");
    if (QDir(output_path).exists()) open_this_path = output_path;

    QString settings_path = QFileDialog::getOpenFileName(
          nullptr, "Open CSV File", open_this_path
        , "CSV settings file (settings.csv)" );

    if (settings_path.isEmpty()) {
        default_settings = Settings();
        ui->lblSettingsPath->setText("");
        ui->lblSettingsPath->setToolTip("");
        ui->lblSettingsIcon->setMode(FileCheckIcon::Mode::NotFound);
        return;
    }

    QString n = QFileInfo(settings_path).dir().dirName();
    ui->lblSettingsPath->setText(n.left(28).append(n.length() > 28 ? "..." : ""));
    ui->lblSettingsPath->setToolTip(settings_path);

    ui->lblSettingsIcon->setMode(FileCheckIcon::Mode::Loading);
    file_checker->checkSettingsFile(settings_path);
}

void NewSimulationTab::onFileChecked(FileType file, const QString& path, bool result)
{
    if (file == FileType::Settings){
        bool settings_valid;
        auto settings = parseSettingsCSV(path, &settings_valid);

        if (settings_valid) {
            default_settings = settings;
            resetSettings();
            ui->lblSettingsIcon->setMode(FileCheckIcon::Mode::Checked);
        } else {
            ui->lblSettingsIcon->setMode(FileCheckIcon::Mode::Corrupted);
        }
    }

    if (file == FileType::InitialSystem){
        if (result) {
            initial_system_path = path;
            ui->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Checked);
            ui->oglFilePreview->initializeMethod(GenerationMethod::File, path);
        } else {
            ui->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Corrupted);
        }
    }
}

void NewSimulationTab::createSimulationManager()
{
    if (simulation_manager) return;

    // Fetch settings and check validity
    Settings settings = fetchSettings();

    DataSettings data_settings = settings.getDataSettings();
    GenerationSettings generation_settings = settings.getGenerationSettings();
    SimulationSettings simulation_settings = settings.getSimulationSettings();

    if (!generation_settings.isValid()|| !simulation_settings.isValid()) {
        QMessageBox::warning(this, tr("Fehler"), tr("Einstellungen unvollständig"));
        return;
    }

    // Initialize Simulation Manager
    simulation_manager = new SimulationManager( settings, data_settings
                                              , generation_settings
                                              , simulation_settings);

    connect( simulation_manager, &SimulationManager::destroyed
           , this, &NewSimulationTab::simulationAborted );
    connect( simulation_manager, &SimulationManager::initializationCompleted
            , this, &NewSimulationTab::showSimulationProgress );
    connect( ui->btnOpenLiveWindow, &QPushButton::clicked
           , simulation_manager, &SimulationManager::openSimulationWindow );

    simulation_manager->startSimulationProcedere();

    // Stop preview_timer to halt preview animation
    preview_timer->stop();

    // Show fancy simulation preparation loading gif
    ui->stwProgress->setCurrentIndex(1);
}

void NewSimulationTab::showSimulationProgress()
{
    progress_timer->start(1000/20);
    ui->stwProgress->setCurrentIndex(2);

    connect( progress_timer, &QTimer::timeout
            , this, &NewSimulationTab::updateSimulationProgress );
}


void NewSimulationTab::abortSimulation()
{
    if (!simulation_manager) return;
    ui->stwProgress->setCurrentIndex(3);

    simulation_manager->abortSimulation();
}

void NewSimulationTab::simulationAborted()
{
    // Restart preview timer -> restarts preview animations
    preview_timer->start();

    simulation_manager = nullptr;
    ui->stwProgress->setCurrentIndex(0);
}

void NewSimulationTab::updateSimulationProgress()
{
    if (!simulation_manager) return;
    double time = simulation_manager->getTime();
    double duration = simulation_manager->getDuration();

    QString time_text = QString("%1 / %2").arg(time).arg(duration);
    ui->lblSimulationProgress->setText(time_text);
    ui->pbaSimulationProgress->setValue(time/duration*100000);
}

void NewSimulationTab::updatePreview()
{
    auto method = static_cast<GenerationMethod>(ui->stwGenerationSettings->currentIndex());

    switch (method) {
    case GenerationMethod::File:
        if (!initial_system_path.isEmpty()) { ui->oglFilePreview->update(); } break;
    case GenerationMethod::NormalSphere: ui->oglNormalSpherePreview->update(); break;
    case GenerationMethod::UniformCube: ui->oglUniformCubePreview->update(); break;
    case GenerationMethod::UniformSphere: ui->oglUniformSpherePreview->update(); break;
    case GenerationMethod::PlummerSphere: ui->oglPlummerPreview->update(); break;
    case GenerationMethod::SpiralGalaxy: ui->oglSpiralGalaxyPreview->update(); break;
    case GenerationMethod::CollisionModel: ui->oglCollisionPreview->update(); break;
    case GenerationMethod::FlybyModel: ui->oglFlybyPreview->update(); break;
    default: break;
    }
}


void NewSimulationTab::setupInitialSystemPreviews()
{
    preview_timer->start(1000/30);
    connect(preview_timer, &QTimer::timeout, this, &NewSimulationTab::updatePreview);

    ui->oglNormalSpherePreview->initializeMethod(GenerationMethod::NormalSphere);
    ui->oglUniformCubePreview->initializeMethod(GenerationMethod::UniformCube);
    ui->oglUniformSpherePreview->initializeMethod(GenerationMethod::UniformSphere);
    ui->oglPlummerPreview->initializeMethod(GenerationMethod::PlummerSphere);
    ui->oglSpiralGalaxyPreview->initializeMethod(GenerationMethod::SpiralGalaxy);
    ui->oglCollisionPreview->initializeMethod(GenerationMethod::CollisionModel);
    ui->oglFlybyPreview->initializeMethod(GenerationMethod::FlybyModel);
}

void NewSimulationTab::loadSystemData(const QString& folder_name)
{
    if (folder_name.isEmpty()) {
        ui->lblSystemDataPath->setText("");
        ui->lblSystemDataIcon->setMode(FileCheckIcon::Mode::NotFound);
        return;
    }

    QString n = folder_name.left(36).append(folder_name.length() > 36 ? "..." : "");
    ui->lblSystemDataPath->setText(n);
    ui->lblSystemDataIcon->setMode(FileCheckIcon::Mode::Loading);

    QString app_path = QCoreApplication::applicationDirPath();
    QString system_data_path = app_path + "/output/" + folder_name + "/initial_system.csv";
    ui->lblSystemDataPath->setToolTip(system_data_path);
    file_checker->checkInitialSystemFile(system_data_path);

}

void NewSimulationTab::mapStringsToUiPtrs()
{
    // Data Settings ui elements
    setting_ptrs["RenderDataStep"]      = ui->dsbRenderTime;
    setting_ptrs["DiagnosticsDataStep"] = ui->dsbDiagnosticsTime;
    setting_ptrs["AnalyticsDataStep"]   = ui->dsbAnalyticsTime;

    setting_ptrs["SaveRenderData"]      = ui->cbxRender;
    setting_ptrs["SaveDiagnsoticsData"] = ui->cbxDiagnostics;
    setting_ptrs["SaveAnalyticsData"]   = ui->cbxAnalytics;
    setting_ptrs["SaveFolder"]          = ui->cbxFolder;

    // Generation Settings ui elements
    setting_ptrs["GenerationMethod"] = ui->cobGenerationMethod;
    setting_ptrs["Seed"]             = ui->sbxSeed;

    setting_ptrs["FileFolder"] = ui->lblSystemDataPath;

    setting_ptrs["NormalSphereN"]        = ui->sbxNormalN;
    setting_ptrs["NormalSpherePosStd"]   = ui->dsbNormalPos;
    setting_ptrs["NormalSphereVelStd"]   = ui->dsbNormalVel;
    setting_ptrs["NormalSphereMassMean"] = ui->dsbNormalMassMean;
    setting_ptrs["NormalSphereMassStd"]  = ui->dsbNormalMassStd;

    setting_ptrs["UniformCubeN"]    = ui->sbxUniformN;
    setting_ptrs["UniformCubeSide"] = ui->dsbUniformSide;
    setting_ptrs["UniformCubeVel"]  = ui->dsbUniformVel;
    setting_ptrs["UniformCubeMass"] = ui->dsbUniformMass;

    setting_ptrs["UniformSphereN"]      = ui->sbxUniformSphereN;
    setting_ptrs["UniformSphereRadius"] = ui->dsbUniformSphereRadius;
    setting_ptrs["UniformSphereVel"]    = ui->dsbUniformSphereVel;
    setting_ptrs["UniformSphereMass"]   = ui->dsbUniformSphereMass;

    setting_ptrs["PlummerSphereN"]      = ui->sbxPlummerN;
    setting_ptrs["PlummerSphereRadius"] = ui->dsbPlummerRadius;
    setting_ptrs["PlummerSphereMass"]   = ui->dsbPlummerTotalMass;

    setting_ptrs["SpiralGalaxyN"]       = ui->sbxSpiralGalaxyN;
    setting_ptrs["SpiralGalaxyArms"]    = ui->sbxSpiralGalaxyArms;
    setting_ptrs["SpiralGalaxyRadius"]  = ui->dsbSpiralGalaxyRadius;
    setting_ptrs["SpiralGalaxyMass"]    = ui->dsbSpiralGalaxyMass;
    setting_ptrs["SpiralGalaxyTwist"]   = ui->dsbSpiralGalaxyTwist;
    setting_ptrs["SpiralGalaxyBHMass"]  = ui->dsbSpiralGalaxyBHMass;

    setting_ptrs["CollisionModelN1"]      = ui->sbxCollisionN1;
    setting_ptrs["CollisionModelRadius1"] = ui->dsbCollisionRadius1;
    setting_ptrs["CollisionModelMass1"]   = ui->dsbCollisionMass1;
    setting_ptrs["CollisionModelN2"]      = ui->sbxCollisionN2;
    setting_ptrs["CollisionModelRadius2"] = ui->dsbCollisionRadius2;
    setting_ptrs["CollisionModelMass2"]   = ui->dsbCollisionMass2;

    setting_ptrs["FlybyModelN"]        = ui->sbxFlybyN;
    setting_ptrs["FlybyModelRadius"]   = ui->dsbFlybyRadius;
    setting_ptrs["FlybyModelMass"]     = ui->dsbFlybyTotalMass;
    setting_ptrs["FlybyModelBodyMass"] = ui->dsbFlybyHeavyMass;

    // Simulation Settings ui elements
    setting_ptrs["SimulationMethod"] = ui->cobSimulationMethod;
    setting_ptrs["Duration"] = ui->dsbSimDuration;

    setting_ptrs["EulerStep"] = ui->dsbEulerStep;
    setting_ptrs["EulerSoft"] = ui->dsbEulerSoft;

    setting_ptrs["LeapfrogStep"] = ui->dsbLeapfrogStep;
    setting_ptrs["LeapfrogSoft"] = ui->dsbLeapfrogSoft;

    setting_ptrs["HermiteStep"] = ui->dsbHermiteStep;
    setting_ptrs["HermiteSoft"] = ui->dsbHermiteSoft;

    setting_ptrs["HITSStepParam"] = ui->dsbHITSStep;
    setting_ptrs["HITSSoft"]      = ui->dsbHITSSoft;

    setting_ptrs["HACSIrregularStepParam"] = ui->dsbHACSIrrStep;
    setting_ptrs["HACSRegularStepParam"]   = ui->dsbHACSRegStep;
    setting_ptrs["HACSMaxNeighbors"]       = ui->sbxHACSMaxNeighbors;
    setting_ptrs["HACSSoft"]               = ui->dsbHACSSoft;

    setting_ptrs["BHLeapfrogStep"] = ui->dsbBHLeapfrogStep;
    setting_ptrs["BHLeapfrogMAC"]  = ui->dsbBHLeapfrogMAC;
    setting_ptrs["BHLeapfrogSoft"] = ui->dsbBHLeapfrogSoft;

    QStringList expected_identifiers = Settings().getIdentifiers();
    QStringList identifiers = setting_ptrs.keys();

    for (const auto& identifier : identifiers) {
        if (!expected_identifiers.contains(identifier)) {
            qDebug() << identifier << "of setting_ptrs is invalid.";
        }
    }

    for (const auto& expected_identifier : expected_identifiers) {
        if (!identifiers.contains(expected_identifier)) {
            qDebug() << expected_identifier << "is missing in setting_ptrs.";
        }
    }
}

Settings NewSimulationTab::parseSettingsCSV(const QString& path, bool* is_valid) const
{
    Settings settings;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (is_valid) *is_valid = false;
        return settings;
    }

    QTextStream in(&file);

    // Header and values need to have same length and mustn't be empty
    QStringList csv_identifiers = in.readLine().split(",");
    QStringList csv_values = in.readLine().split(",");
    if (   csv_identifiers.isEmpty() || csv_values.isEmpty()
        || csv_identifiers.size() != csv_values.size() ) {
        if (is_valid) *is_valid = false;
        return settings;
    }

    // Fill settings. Checks happen inside Settings::setValue() and are returned
    for (size_t i = 0; i < csv_identifiers.size(); ++i) {
        if (!settings.setValue(csv_identifiers.at(i), csv_values.at(i))) {
            if (is_valid) *is_valid = false;
            return settings;
        }
    }

    if (is_valid) *is_valid = true;
    return settings;
}

Settings NewSimulationTab::fetchSettings(bool* is_valid) const
{
    Settings settings;

    const QStringList identifiers = settings.getIdentifiers();

    for (const auto& identifier : identifiers) {
        Setting setting = settings.getSetting(identifier);

        switch (setting.type) {
        case Setting::Type::File:
            if (!settings.setValue(identifier, initial_system_path)) {
                if (is_valid) *is_valid = false;
                return settings;
            }
            break;
        case Setting::Type::Int:
            if (auto* w = qobject_cast<QSpinBox*>(setting_ptrs[identifier])) {
                if (!settings.setValue(identifier, w->value())) {
                    if (is_valid) *is_valid = false;
                    return settings;
                }
            } else {
                if (is_valid) *is_valid = false;
                return settings;
            }
            break;
        case Setting::Type::Double:
            if (auto* w = qobject_cast<QDoubleSpinBox*>(setting_ptrs[identifier])) {
                if (!settings.setValue(identifier, w->value())) {
                    if (is_valid) *is_valid = false;
                    return settings;
                }
            } else {
                if (is_valid) *is_valid = false;
                return settings;
            }
            break;
        case Setting::Type::Bool:
            if (auto* w = qobject_cast<QCheckBox*>(setting_ptrs[identifier])) {
                if (!settings.setValue(identifier, w->isChecked())) {
                    if (is_valid) *is_valid = false;
                    return settings;
                }
            } else {
                if (is_valid) *is_valid = false;
                return settings;
            }
            break;
        case Setting::Type::GenerationMethod:
            if (auto* w = qobject_cast<QComboBox*>(setting_ptrs[identifier])) {
                if (!settings.setValue(identifier, w->currentText())) {
                    if (is_valid) *is_valid = false;
                    return settings;
                }
            } else {
                if (is_valid) *is_valid = false;
                return settings;
            }
            break;
        case Setting::Type::SimulationMethod:
            if (auto* w = qobject_cast<QComboBox*>(setting_ptrs[identifier])) {
                if (!settings.setValue(identifier, w->currentText())) {
                    if (is_valid) *is_valid = false;
                    return settings;
                }
            } else {
                if (is_valid) *is_valid = false;
                return settings;
            }
            break;
        default:
            if (is_valid) *is_valid = false;
            return settings;
            break;
        }
    }

    if (is_valid) *is_valid = true;
    return settings;
}
