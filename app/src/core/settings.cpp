#include "settings.h"

#include <enkas/generation/generation_config.h>

#include <stdexcept>

#include "enkas/simulation/simulation_method.h"

template <typename T>
const T& get_value(const std::unordered_map<std::string, Setting>& map, const std::string& key) {
    try {
        const SettingValue& val = map.at(key).value;
        return std::get<T>(val);
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Setting key not found: " + key);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Incorrect type for setting key: " + key);
    }
}

Settings::Settings() {
    // Aliases for readability
    auto G_DAT = Setting::Group::Data;
    auto G_GEN = Setting::Group::Generation;
    auto G_SIM = Setting::Group::Simulation;

    auto T_INT = Setting::Type::Int;
    auto T_DBL = Setting::Type::Double;
    auto T_BOL = Setting::Type::Bool;
    auto T_FIL = Setting::Type::File;
    auto T_GME = Setting::Type::GenerationMethod;
    auto T_SME = Setting::Type::SimulationMethod;

    settings_ = {// Data Settings
                 {"DiagnosticsDataStep", {G_DAT, T_DBL, 0.0}},
                 {"SystemDataStep", {G_DAT, T_DBL, 0.0}},

                 {"SaveDiagnsoticsData", {G_DAT, T_BOL, false}},
                 {"SaveSystemData", {G_DAT, T_BOL, false}},
                 {"SaveFolder", {G_DAT, T_BOL, false}},

                 // Generation Settings
                 {"FilePath", {G_GEN, T_FIL, ""}},
                 {"GenerationMethod", {G_GEN, T_GME, enkas::generation::Method::PlummerSphere}},
                 {"Seed", {G_SIM, T_INT, 42}},

                 {"NormalSphereN", {G_GEN, T_INT, 1000}},
                 {"NormalSpherePosStd", {G_GEN, T_DBL, 0.7}},
                 {"NormalSphereVelStd", {G_GEN, T_DBL, 0.1}},
                 {"NormalSphereMassMean", {G_GEN, T_DBL, 50.0}},
                 {"NormalSphereMassStd", {G_GEN, T_DBL, 1.0}},

                 {"UniformCubeN", {G_GEN, T_INT, 1000}},
                 {"UniformCubeSide", {G_GEN, T_DBL, 3.0}},
                 {"UniformCubeVel", {G_GEN, T_DBL, 0.1}},
                 {"UniformCubeMass", {G_GEN, T_DBL, 50.0}},

                 {"UniformSphereN", {G_GEN, T_INT, 1000}},
                 {"UniformSphereRadius", {G_GEN, T_DBL, 2.0}},
                 {"UniformSphereVel", {G_GEN, T_DBL, 0.1}},
                 {"UniformSphereMass", {G_GEN, T_DBL, 50.0}},

                 {"PlummerSphereN", {G_GEN, T_INT, 1000}},
                 {"PlummerSphereRadius", {G_GEN, T_DBL, 0.6}},
                 {"PlummerSphereMass", {G_GEN, T_DBL, 5000.0}},

                 {"SpiralGalaxyN", {G_GEN, T_INT, 1000}},
                 {"SpiralGalaxyArms", {G_GEN, T_INT, 2.0}},
                 {"SpiralGalaxyRadius", {G_GEN, T_DBL, 4.0}},
                 {"SpiralGalaxyMass", {G_GEN, T_DBL, 10.0}},
                 {"SpiralGalaxyTwist", {G_GEN, T_DBL, 2.4}},
                 {"SpiralGalaxyBHMass", {G_GEN, T_DBL, 10000.0}},

                 {"CollisionModelN1", {G_GEN, T_INT, 600}},
                 {"CollisionModelRadius1", {G_GEN, T_DBL, 0.3}},
                 {"CollisionModelMass1", {G_GEN, T_DBL, 3000.0}},
                 {"CollisionModelN2", {G_GEN, T_INT, 400}},
                 {"CollisionModelRadius2", {G_GEN, T_DBL, 0.2}},
                 {"CollisionModelMass2", {G_GEN, T_DBL, 2000.0}},

                 // Simulation Settings
                 {"SimulationMethod", {G_GEN, T_SME, enkas::simulation::Method::Hermite}},
                 {"Duration", {G_SIM, T_DBL, 100.0}},

                 {"EulerStep", {G_SIM, T_DBL, 0.001}},
                 {"EulerSoft", {G_SIM, T_DBL, 0.1}},

                 {"LeapfrogStep", {G_SIM, T_DBL, 0.001}},
                 {"LeapfrogSoft", {G_SIM, T_DBL, 0.1}},

                 {"HermiteStep", {G_SIM, T_DBL, 0.001}},
                 {"HermiteSoft", {G_SIM, T_DBL, 0.1}},

                 {"HITSStepParam", {G_SIM, T_DBL, 0.001}},
                 {"HITSSoft", {G_SIM, T_DBL, 0.1}},

                 {"HACSIrregularStepParam", {G_SIM, T_DBL, 0.001}},
                 {"HACSRegularStepParam", {G_SIM, T_DBL, 0.01}},
                 {"HACSMaxNeighbors", {G_SIM, T_INT, 16}},
                 {"HACSSoft", {G_SIM, T_DBL, 0.01}},

                 {"BHLeapfrogStep", {G_SIM, T_DBL, 0.001}},
                 {"BHLeapfrogMAC", {G_SIM, T_DBL, 0.5}},
                 {"BHLeapfrogSoft", {G_SIM, T_DBL, 0.1}}};
}

bool Settings::setSetting(const std::string& identifier, const Setting& setting) {
    if (!settings_.contains(identifier)) return false;
    if (!isTypeCompatible(setting.value, setting.type)) return false;

    settings_[identifier] = setting;
    return true;
}

bool Settings::setGroup(const std::string& identifier, const Setting::Group& group) {
    if (!settings_.contains(identifier)) return false;

    settings_[identifier].group = group;
    return true;
}

bool Settings::setType(const std::string& identifier, const Setting::Type& type) {
    if (!settings_.contains(identifier)) return false;
    if (!isTypeCompatible(settings_[identifier].value, type)) return false;

    settings_[identifier].type = type;
    return true;
}

bool Settings::setValue(const std::string& identifier, const SettingValue& value) {
    if (!settings_.contains(identifier)) return false;
    if (!isTypeCompatible(value, settings_[identifier].type)) return false;

    settings_[identifier].value = value;
    return true;
}

std::vector<std::string> Settings::getIdentifiers() const {
    std::vector<std::string> keys;
    keys.reserve(settings_.size());
    for (const auto& pair : settings_) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Settings::hasSetting(const std::string& identifier) const {
    return settings_.count(identifier) > 0;
}

Setting Settings::getSetting(const std::string& identifier, bool* is_setting) const {
    return settings_.at(identifier);
}

bool Settings::isTypeCompatible(const SettingValue& value, const Setting::Type& type) const {
    switch (type) {
        case Setting::Type::Int:
            return std::holds_alternative<int>(value);
        case Setting::Type::Double:
            return std::holds_alternative<double>(value);
        case Setting::Type::Bool:
            return std::holds_alternative<bool>(value);
        case Setting::Type::File:
            return std::holds_alternative<std::string>(value);
        case Setting::Type::GenerationMethod:
            return std::holds_alternative<enkas::generation::Method>(value);
        case Setting::Type::SimulationMethod:
            return std::holds_alternative<enkas::simulation::Method>(value);
    }
    return false;  // Should not happen
}

DataSettings Settings::getDataSettings() const {
    DataSettings d;

    d.diagnostics_step = get_value<double>(settings_, "DiagnosticsDataStep");
    d.system_step = get_value<double>(settings_, "SystemDataStep");

    d.save_diagnostics_data = get_value<bool>(settings_, "SaveDiagnsoticsData");
    d.save_system_data = get_value<bool>(settings_, "SaveSystemData");
    d.save_folder = get_value<bool>(settings_, "SaveFolder");

    return d;
}

enkas::generation::Config Settings::getGenerationConfig() const {
    enkas::generation::Config config;

    config.seed = get_value<int>(settings_, "Seed");

    auto method = get_value<enkas::generation::Method>(settings_, "GenerationMethod");

    switch (method) {
        case enkas::generation::Method::NormalSphere:
            config.specific_settings = getNormalSphereSettings();
            break;
        case enkas::generation::Method::UniformSphere:
            config.specific_settings = getUniformSphereSettings();
            break;
        case enkas::generation::Method::UniformCube:
            config.specific_settings = getUniformCubeSettings();
            break;
        case enkas::generation::Method::PlummerSphere:
            config.specific_settings = getPlummerSphereSettings();
            break;
        case enkas::generation::Method::SpiralGalaxy:
            config.specific_settings = getSpiralGalaxySettings();
            break;
        case enkas::generation::Method::CollisionModel:
            config.specific_settings = getCollisionModelSettings();
            break;
        default:  // Should never happen
            throw std::invalid_argument("Unknown generation method.");
    }

    return config;
}

enkas::generation::NormalSphereSettings Settings::getNormalSphereSettings() const {
    enkas::generation::NormalSphereSettings s;

    s.particle_count = get_value<int>(settings_, "NormalSphereN");
    s.position_std_dev = get_value<double>(settings_, "NormalSpherePosStd");
    s.velocity_std_dev = get_value<double>(settings_, "NormalSphereVelStd");
    s.mass_mean = get_value<double>(settings_, "NormalSphereMassMean");
    s.mass_std_dev = get_value<double>(settings_, "NormalSphereMassStd");

    return s;
}

enkas::generation::UniformSphereSettings Settings::getUniformSphereSettings() const {
    enkas::generation::UniformSphereSettings s;

    s.particle_count = get_value<int>(settings_, "UniformSphereN");
    s.sphere_radius = get_value<double>(settings_, "UniformSphereRadius");
    s.initial_velocity = get_value<double>(settings_, "UniformSphereVel");
    s.total_mass = get_value<double>(settings_, "UniformSphereMass");

    return s;
}

enkas::generation::UniformCubeSettings Settings::getUniformCubeSettings() const {
    enkas::generation::UniformCubeSettings s;

    s.particle_count = get_value<int>(settings_, "UniformCubeN");
    s.side_length = get_value<double>(settings_, "UniformCubeSide");
    s.initial_velocity = get_value<double>(settings_, "UniformCubeVel");
    s.total_mass = get_value<double>(settings_, "UniformCubeMass");

    return s;
}

enkas::generation::PlummerSphereSettings Settings::getPlummerSphereSettings() const {
    enkas::generation::PlummerSphereSettings s;

    s.particle_count = get_value<int>(settings_, "PlummerSphereN");
    s.sphere_radius = get_value<double>(settings_, "PlummerSphereRadius");
    s.total_mass = get_value<double>(settings_, "PlummerSphereMass");

    return s;
}

enkas::generation::SpiralGalaxySettings Settings::getSpiralGalaxySettings() const {
    enkas::generation::SpiralGalaxySettings s;

    s.particle_count = get_value<int>(settings_, "SpiralGalaxyN");
    s.num_arms = get_value<int>(settings_, "SpiralGalaxyArms");
    s.radius = get_value<double>(settings_, "SpiralGalaxyRadius");
    s.total_mass = get_value<double>(settings_, "SpiralGalaxyMass");
    s.twist = get_value<double>(settings_, "SpiralGalaxyTwist");
    s.black_hole_mass = get_value<double>(settings_, "SpiralGalaxyBHMass");

    return s;
}

enkas::generation::CollisionModelSettings Settings::getCollisionModelSettings() const {
    enkas::generation::CollisionModelSettings s;

    s.particle_count_1 = get_value<int>(settings_, "CollisionModelN1");
    s.sphere_radius_1 = get_value<double>(settings_, "CollisionModelRadius1");
    s.total_mass_1 = get_value<double>(settings_, "CollisionModelMass1");
    s.particle_count_2 = get_value<int>(settings_, "CollisionModelN2");
    s.sphere_radius_2 = get_value<double>(settings_, "CollisionModelRadius2");
    s.total_mass_2 = get_value<double>(settings_, "CollisionModelMass2");

    return s;
}

enkas::simulation::Config Settings::getSimulationConfig() const {
    enkas::simulation::Config s;

    s.duration = get_value<double>(settings_, "Duration");

    auto method = get_value<enkas::simulation::Method>(settings_, "SimulationMethod");

    switch (method) {
        case enkas::simulation::Method::Euler:
            s.specific_settings = getEulerSettings();
            break;
        case enkas::simulation::Method::Leapfrog:
            s.specific_settings = getLeapfrogSettings();
            break;
        case enkas::simulation::Method::Hermite:
            s.specific_settings = getHermiteSettings();
            break;
        case enkas::simulation::Method::HITS:
            s.specific_settings = getHitsSettings();
            break;
        case enkas::simulation::Method::BarnesHutLeapfrog:
            s.specific_settings = getBarnesHutLeapfrogSettings();
            break;
        default:  // Should never happen
            throw std::invalid_argument("Unknown simulation method.");
    }

    return s;
}

enkas::simulation::EulerSettings Settings::getEulerSettings() const {
    enkas::simulation::EulerSettings s;

    s.time_step = get_value<double>(settings_, "EulerStep");
    s.softening_parameter = get_value<double>(settings_, "EulerSoft");

    return s;
}

enkas::simulation::LeapfrogSettings Settings::getLeapfrogSettings() const {
    enkas::simulation::LeapfrogSettings s;

    s.time_step = get_value<double>(settings_, "LeapfrogStep");
    s.softening_parameter = get_value<double>(settings_, "LeapfrogSoft");

    return s;
}

enkas::simulation::HermiteSettings Settings::getHermiteSettings() const {
    enkas::simulation::HermiteSettings s;

    s.time_step = get_value<double>(settings_, "HermiteStep");
    s.softening_parameter = get_value<double>(settings_, "HermiteSoft");

    return s;
}

enkas::simulation::HitsSettings Settings::getHitsSettings() const {
    enkas::simulation::HitsSettings s;

    s.time_step_parameter = get_value<double>(settings_, "HITSStepParam");
    s.softening_parameter = get_value<double>(settings_, "HITSSoft");

    return s;
}

enkas::simulation::BarnesHutLeapfrogSettings Settings::getBarnesHutLeapfrogSettings() const {
    enkas::simulation::BarnesHutLeapfrogSettings s;

    s.time_step = get_value<double>(settings_, "BHLeapfrogStep");
    s.theta_mac = get_value<double>(settings_, "BHLeapfrogMAC");
    s.softening_parameter = get_value<double>(settings_, "BHLeapfrogSoft");

    return s;
}
