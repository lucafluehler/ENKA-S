#include "settings.h"

Settings::Settings()
{
    auto G_DAT = Setting::Group::Data;
    auto G_GEN = Setting::Group::Generation;
    auto G_SIM = Setting::Group::Simulation;

    auto T_INT = Setting::Type::Int;
    auto T_DBL = Setting::Type::Double;
    auto T_BOL = Setting::Type::Bool;
    auto T_FIL = Setting::Type::File;
    auto T_GME = Setting::Type::GenerationMethod;
    auto T_SME = Setting::Type::SimulationMethod;

    settings =
    {
          // Data Settings
          {"RenderDataStep",      {G_DAT, T_DBL, 0.0}}
        , {"DiagnosticsDataStep", {G_DAT, T_DBL, 0.0}}
        , {"AnalyticsDataStep",   {G_DAT, T_DBL, 0.0}}

        , {"SaveRenderData",      {G_DAT, T_BOL, false}}
        , {"SaveDiagnsoticsData", {G_DAT, T_BOL, false}}
        , {"SaveAnalyticsData",   {G_DAT, T_BOL, false}}
        , {"SaveFolder",          {G_DAT, T_BOL, false}}

          // Generation Settings
        , {"GenerationMethod", {G_GEN, T_GME, "Plummer-Modell"}}
        , {"Seed",             {G_SIM, T_INT, 42}}

        , {"FileFolder", {G_GEN, T_FIL, ""}}

        , {"NormalSphereN",        {G_GEN, T_INT, 1000}}
        , {"NormalSpherePosStd",   {G_GEN, T_DBL, 0.7}}
        , {"NormalSphereVelStd",   {G_GEN, T_DBL, 0.1}}
        , {"NormalSphereMassMean", {G_GEN, T_DBL, 50.0}}
        , {"NormalSphereMassStd",  {G_GEN, T_DBL, 1.0}}

        , {"UniformCubeN",    {G_GEN, T_INT, 1000}}
        , {"UniformCubeSide", {G_GEN, T_DBL, 3.0}}
        , {"UniformCubeVel",  {G_GEN, T_DBL, 0.1}}
        , {"UniformCubeMass", {G_GEN, T_DBL, 50.0}}

        , {"UniformSphereN",      {G_GEN, T_INT, 1000}}
        , {"UniformSphereRadius", {G_GEN, T_DBL, 2.0}}
        , {"UniformSphereVel",    {G_GEN, T_DBL, 0.1}}
        , {"UniformSphereMass",   {G_GEN, T_DBL, 50.0}}

        , {"PlummerSphereN",      {G_GEN, T_INT, 1000}}
        , {"PlummerSphereRadius", {G_GEN, T_DBL, 0.6}}
        , {"PlummerSphereMass",   {G_GEN, T_DBL, 5000.0}}

        , {"SpiralGalaxyN",      {G_GEN, T_INT, 1000}}
        , {"SpiralGalaxyArms",   {G_GEN, T_INT, 2.0}}
        , {"SpiralGalaxyRadius", {G_GEN, T_DBL, 4.0}}
        , {"SpiralGalaxyMass",   {G_GEN, T_DBL, 10.0}}
        , {"SpiralGalaxyTwist",  {G_GEN, T_DBL, 2.4}}
        , {"SpiralGalaxyBHMass", {G_GEN, T_DBL, 10000.0}}

        , {"CollisionModelN1",      {G_GEN, T_INT, 600}}
        , {"CollisionModelRadius1", {G_GEN, T_DBL, 0.3}}
        , {"CollisionModelMass1",   {G_GEN, T_DBL, 3000.0}}
        , {"CollisionModelN2",      {G_GEN, T_INT, 400}}
        , {"CollisionModelRadius2", {G_GEN, T_DBL, 0.2}}
        , {"CollisionModelMass2",   {G_GEN, T_DBL, 2000.0}}

        , {"FlybyModelN",        {G_GEN, T_INT, 1000}}
        , {"FlybyModelRadius",   {G_GEN, T_DBL, 0.3}}
        , {"FlybyModelMass",     {G_GEN, T_DBL, 5000.0}}
        , {"FlybyModelBodyMass", {G_GEN, T_DBL, 10000.0}}

          // Simulation Settings
        , {"SimulationMethod", {G_GEN, T_SME, "Hermite"}}
        , {"Duration",         {G_SIM, T_DBL, 100.0}}

        , {"EulerStep", {G_SIM, T_DBL, 0.001}}
        , {"EulerSoft", {G_SIM, T_DBL, 0.1}}

        , {"LeapfrogStep", {G_SIM, T_DBL, 0.001}}
        , {"LeapfrogSoft", {G_SIM, T_DBL, 0.1}}

        , {"HermiteStep", {G_SIM, T_DBL, 0.001}}
        , {"HermiteSoft", {G_SIM, T_DBL, 0.1}}

        , {"HITSStepParam", {G_SIM, T_DBL, 0.001}}
        , {"HITSSoft",      {G_SIM, T_DBL, 0.1}}

        , {"HACSIrregularStepParam", {G_SIM, T_DBL, 0.001}}
        , {"HACSRegularStepParam",   {G_SIM, T_DBL, 0.01}}
        , {"HACSMaxNeighbors",       {G_SIM, T_INT, 16}}
        , {"HACSSoft",               {G_SIM, T_DBL, 0.01}}

        , {"BHLeapfrogStep", {G_SIM, T_DBL, 0.001}}
        , {"BHLeapfrogMAC",  {G_SIM, T_DBL, 0.5}}
        , {"BHLeapfrogSoft", {G_SIM, T_DBL, 0.1}}
    };
}


bool Settings::setSetting(const QString& identifier, const Setting& setting)
{
    if (!settings.contains(identifier)) return false;
    if (!isConvertible(setting.value, setting.type)) return false;

    settings[identifier] = setting;
    return true;
}

bool Settings::setGroup(const QString& identifier, const Setting::Group& group)
{
    if (!settings.contains(identifier)) return false;

    settings[identifier].group = group;
    return true;
}

bool Settings::setType(const QString& identifier, const Setting::Type& type)
{
    if (!settings.contains(identifier)) return false;
    if (!isConvertible(settings[identifier].value, type)) return false;

    settings[identifier].type = type;
    return true;
}

bool Settings::setValue(const QString& identifier, const QVariant& value)
{
    if (!settings.contains(identifier)) return false;
    if (!isConvertible(value, settings[identifier].type)) return false;

    settings[identifier].value = value;
    return true;
}


QStringList Settings::getIdentifiers() const
{
    return settings.keys();
}

bool Settings::isSetting(const QString& identifier) const
{
    return settings.contains(identifier);
}

Setting Settings::getSetting(const QString& identifier, bool* is_setting) const
{
    if (is_setting) *is_setting = settings.contains(identifier);
    return settings.value(identifier);
}


DataSettings Settings::getDataSettings() const
{
    DataSettings d;

    d.render_step      = settings["RenderDataStep"].value.toDouble();
    d.diagnostics_step = settings["DiagnosticsDataStep"].value.toDouble();
    d.analytics_step   = settings["AnalyticsDataStep"].value.toDouble();

    d.save_render_data      = settings["SaveRenderData"].value.toBool();
    d.save_diagnostics_data = settings["SaveDiagnsoticsData"].value.toBool();
    d.save_analytics_data   = settings["SaveAnalyticsData"].value.toBool();
    d.save_folder           = settings["SaveFolder"].value.toBool();

    return d;
}

GenerationSettings Settings::getGenerationSettings() const
{
    GenerationSettings g;

    auto method = settings["GenerationMethod"].value.toString();
    g.method = getGenerationMethods().key(method);
    g.seed   = settings["Seed"].value.toUInt();

    auto& gfile = g.file_settings;
    gfile.system_data_path = settings["FileFolder"].value.toString().toStdString();

    auto& gnors = g.normal_sphere_settings;
    gnors.N         = settings["NormalSphereN"].value.toInt();
    gnors.pos_std   = settings["NormalSpherePosStd"].value.toDouble();
    gnors.vel_std   = settings["NormalSphereVelStd"].value.toDouble();
    gnors.mass_mean = settings["NormalSphereMassMean"].value.toDouble();
    gnors.mass_std  = settings["NormalSphereMassStd"].value.toDouble();

    auto& gunic = g.uniform_cube_settings;
    gunic.N           = settings["UniformCubeN"].value.toInt();
    gunic.side_length = settings["UniformCubeSide"].value.toDouble();
    gunic.vel         = settings["UniformCubeVel"].value.toDouble();
    gunic.total_mass  = settings["UniformCubeMass"].value.toDouble();

    auto& gunis = g.uniform_sphere_settings;
    gunis.N          = settings["UniformSphereN"].value.toInt();
    gunis.radius     = settings["UniformSphereRadius"].value.toDouble();
    gunis.vel        = settings["UniformSphereVel"].value.toDouble();
    gunis.total_mass = settings["UniformSphereMass"].value.toDouble();

    auto& gplum = g.plummer_sphere_settings;
    gplum.N          = settings["PlummerSphereN"].value.toInt();
    gplum.radius     = settings["PlummerSphereRadius"].value.toDouble();
    gplum.total_mass = settings["PlummerSphereMass"].value.toDouble();


    auto& gspir = g.spiral_galaxy_settings;
    gspir.N          = settings["SpiralGalaxyN"].value.toInt();
    gspir.arms       = settings["SpiralGalaxyArms"].value.toInt();
    gspir.radius     = settings["SpiralGalaxyRadius"].value.toDouble();
    gspir.total_mass = settings["SpiralGalaxyMass"].value.toDouble();
    gspir.twist      = settings["SpiralGalaxyTwist"].value.toDouble();
    gspir.bh_mass    = settings["SpiralGalaxyBHMass"].value.toDouble();

    auto& gcoll = g.collision_model_settings;
    gcoll.N_1          = settings["CollisionModelN1"].value.toInt();
    gcoll.radius_1     = settings["CollisionModelRadius1"].value.toDouble();
    gcoll.total_mass_1 = settings["CollisionModelMass1"].value.toDouble();
    gcoll.N_2          = settings["CollisionModelN2"].value.toInt();
    gcoll.radius_2     = settings["CollisionModelRadius2"].value.toDouble();
    gcoll.total_mass_2 = settings["CollisionModelMass2"].value.toDouble();

    auto& gflyb = g.flyby_model_settings;
    gflyb.N          = settings["FlybyModelN"].value.toInt();
    gflyb.radius     = settings["FlybyModelRadius"].value.toDouble();
    gflyb.total_mass = settings["FlybyModelMass"].value.toDouble();
    gflyb.body_mass  = settings["FlybyModelBodyMass"].value.toDouble();

    return g;
}

SimulationSettings Settings::getSimulationSettings() const
{
    SimulationSettings s;

    auto method = settings["SimulationMethod"].value.toString();
    s.method = getSimulationMethods().key(method);
    s.duration = settings["Duration"].value.toDouble();

    auto& seule = s.euler_settings;
    seule.time_step  = settings["EulerStep"].value.toDouble();
    seule.soft_param = settings["EulerSoft"].value.toDouble();

    auto& sleap = s.leapfrog_settings;
    sleap.time_step  = settings["LeapfrogStep"].value.toDouble();
    sleap.soft_param = settings["LeapfrogSoft"].value.toDouble();

    auto& sherm = s.hermite_settings;
    sherm.time_step  = settings["HermiteStep"].value.toDouble();
    sherm.soft_param = settings["HermiteSoft"].value.toDouble();

    auto& shits = s.hits_settings; // lol
    shits.time_step_param  = settings["HITSStepParam"].value.toDouble();
    shits.soft_param       = settings["HITSSoft"].value.toDouble();

    auto& shacs = s.hacs_settings;
    shacs.irr_step_param  = settings["HACSIrregularStepParam"].value.toDouble();
    shacs.reg_step_param  = settings["HACSRegularStepParam"].value.toDouble();
    shacs.max_neighbors   = settings["HACSMaxNeighbors"].value.toInt();
    shacs.soft_param      = settings["HACSSoft"].value.toDouble();

    auto& sbhle = s.bhleapfrog_settings;
    sbhle.time_step  = settings["BHLeapfrogStep"].value.toDouble();
    sbhle.MAC        = settings["BHLeapfrogMAC"].value.toDouble();
    sbhle.soft_param = settings["BHLeapfrogSoft"].value.toDouble();

    return s;
}

QMap<GenerationMethod, QString> Settings::getGenerationMethods() const
{
    return { {GenerationMethod::File,           "Aus Datei laden"}
           , {GenerationMethod::NormalSphere,   "Normalverteilte Kugel"}
           , {GenerationMethod::UniformCube,    "Homogener Würfel"}
           , {GenerationMethod::UniformSphere,  "Homogene Kugel"}
           , {GenerationMethod::PlummerSphere,  "Plummer-Modell"}
           , {GenerationMethod::SpiralGalaxy,   "Spiralgalaxie"}
           , {GenerationMethod::CollisionModel, "Kollision"}
           , {GenerationMethod::FlybyModel,     "Vorbeiflug"} };
}

QMap<SimulationMethod, QString> Settings::getSimulationMethods() const
{
    return { {SimulationMethod::Euler,      "Euler"}
           , {SimulationMethod::Leapfrog,   "Leapfrog"}
           , {SimulationMethod::Hermite,    "Hermite"}
           , {SimulationMethod::HITS,       "Hermite-Individuelle-Zeitschritte-Schema"}
           , {SimulationMethod::HACS,       "Hermite-Ahmad-Cohen-Schema"}
           , {SimulationMethod::BHLeapfrog, "Barnes-Hut-Algorithmus"} };
}


bool Settings::isConvertible(const QVariant& value, const Setting::Type& type)
{
    switch (type) {
    case Setting::Type::Int: return value.canConvert<int>(); break;
    case Setting::Type::Double: return value.canConvert<double>(); break;
    case Setting::Type::Bool: return value.canConvert<bool>(); break;
    case Setting::Type::File:
    case Setting::Type::GenerationMethod:
    case Setting::Type::SimulationMethod:
        return value.canConvert<QString>(); break;
    default:
        break;
    }

    return false;
}
