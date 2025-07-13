#pragma once

#include <enkas/generation/generation_config.h>
#include <enkas/generation/generation_method.h>
#include <enkas/simulation/simulation_config.h>
#include <enkas/simulation/simulation_method.h>

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct DataSettings {
    double diagnostics_step;
    double system_step;

    bool save_diagnostics_data;
    bool save_system_data;
    bool save_folder;
};

using SettingValue =
    std::variant<int, double, bool, enkas::generation::Method, enkas::simulation::Method>;

struct Setting {
    enum class Group { Data, Generation, Simulation };
    enum class Type { Int, Double, Bool, GenerationMethod, SimulationMethod };

    Group group;
    Type type;
    SettingValue value;

    Setting(Group g = Group::Data, Type t = Type::Int, SettingValue v = 0)
        : group(g), type(t), value(v) {}
};

class Settings {
public:
    Settings();

    bool setSetting(const std::string& identifier, const Setting& setting);
    bool setGroup(const std::string& identifier, const Setting::Group& group);
    bool setType(const std::string& identifier, const Setting::Type& type);
    bool setValue(const std::string& identifier, const SettingValue& value);

    std::vector<std::string> getIdentifiers() const;
    bool hasSetting(const std::string& identifier) const;
    Setting getSetting(const std::string& identifier, bool* is_setting = nullptr) const;

    DataSettings getDataSettings() const;

    enkas::generation::Config getGenerationConfig() const;
    enkas::generation::NormalSphereSettings getNormalSphereSettings() const;
    enkas::generation::UniformSphereSettings getUniformSphereSettings() const;
    enkas::generation::UniformCubeSettings getUniformCubeSettings() const;
    enkas::generation::PlummerSphereSettings getPlummerSphereSettings() const;
    enkas::generation::SpiralGalaxySettings getSpiralGalaxySettings() const;
    enkas::generation::CollisionModelSettings getCollisionModelSettings() const;

    enkas::simulation::Config getSimulationConfig() const;
    enkas::simulation::EulerSettings getEulerSettings() const;
    enkas::simulation::LeapfrogSettings getLeapfrogSettings() const;
    enkas::simulation::HermiteSettings getHermiteSettings() const;
    enkas::simulation::HitsSettings getHitsSettings() const;
    enkas::simulation::BarnesHutLeapfrogSettings getBarnesHutLeapfrogSettings() const;

private:
    bool isTypeCompatible(const SettingValue& value, const Setting::Type& type) const;

    std::unordered_map<std::string, Setting> settings_;
};
