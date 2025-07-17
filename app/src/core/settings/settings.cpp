#include "settings.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include "generation_method.h"
#include "setting_key.h"
#include "simulation_method.h"

std::optional<Settings> Settings::create(
    std::initializer_list<std::pair<SettingKey, SettingValue>> items) {
    std::unordered_set<SettingKey> unique_keys;
    for (const auto& [key, _] : items) {
        if (!unique_keys.insert(key).second) {
            return std::nullopt;  // Duplicate key found
        }
    }
    return Settings(items);
}

std::optional<Settings> Settings::create(const nlohmann::json& json) {
    if (!json.is_object()) {
        return std::nullopt;
    }

    std::vector<std::pair<SettingKey, SettingValue>> items;
    items.reserve(json.size());
    std::unordered_set<SettingKey> unique_keys;

    try {
        for (auto it = json.begin(); it != json.end(); ++it) {
            const SettingKey key = stringToSettingKey(it.key());

            if (!unique_keys.insert(key).second) {
                return std::nullopt;  // Duplicate key in JSON object
            }

            const nlohmann::json& val = it.value();
            SettingValue setting_value;

            if (key == SettingKey::GenerationMethod) {
                if (!val.is_string()) return std::nullopt;
                setting_value = stringToGenerationMethod(val.get<std::string>());
            } else if (key == SettingKey::SimulationMethod) {
                if (!val.is_string()) return std::nullopt;
                setting_value = stringToSimulationMethod(val.get<std::string>());
            } else {
                // Handle primitive types based on JSON type.
                if (val.is_number_integer()) {
                    setting_value = val.get<int>();
                } else if (val.is_number_float()) {
                    setting_value = val.get<double>();
                } else if (val.is_boolean()) {
                    setting_value = val.get<bool>();
                } else if (val.is_string()) {
                    setting_value = val.get<std::string>();
                } else {
                    return std::nullopt;  // Unsupported JSON type
                }
            }
            items.emplace_back(key, std::move(setting_value));
        }
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }

    return Settings(items);
}

Settings::Settings(std::initializer_list<std::pair<SettingKey, SettingValue>> items) {
    keys_.reserve(items.size());
    settings_.reserve(items.size());
    for (const auto& [key, value] : items) {
        keys_.push_back(key);
        settings_.emplace(key, value);
    }
}

Settings::Settings(const std::vector<std::pair<SettingKey, SettingValue>>& items) {
    keys_.reserve(items.size());
    settings_.reserve(items.size());
    for (const auto& [key, value] : items) {
        keys_.push_back(key);
        settings_.emplace(key, value);
    }
}

bool Settings::addSetting(SettingKey key, SettingValue&& value) {
    auto [_, inserted] = settings_.try_emplace(key, std::move(value));
    if (!inserted) {
        return false;  // Key already exists
    }
    keys_.push_back(key);
    return true;
}

bool Settings::removeSetting(SettingKey key) {
    if (settings_.erase(key) == 0) {
        return false;  // Key was not found
    }
    keys_.erase(std::remove(keys_.begin(), keys_.end(), key), keys_.end());
    return true;
}

bool Settings::has(SettingKey key) const { return settings_.contains(key); }

void Settings::set(SettingKey key, SettingValue&& new_value) {
    if (settings_.find(key) == settings_.end()) {
        keys_.push_back(key);
    }
    settings_[key] = std::move(new_value);
}

std::optional<nlohmann::json> Settings::toJson() const {
    nlohmann::json json_obj = nlohmann::json::object();

    try {
        for (const auto& key : keys_) {  // Iterate over keys_ to preserve insertion order
            const auto& value = settings_.at(key);
            const std::string_view key_str = settingKeyToString(key);

            std::visit(
                [&](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    const std::string json_key(key_str);
                    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double> ||
                                  std::is_same_v<T, bool> || std::is_same_v<T, std::string>) {
                        json_obj[json_key] = v;
                    } else if constexpr (std::is_same_v<T, GenerationMethod>) {
                        json_obj[json_key] = generationMethodToString(v);
                    } else if constexpr (std::is_same_v<T, SimulationMethod>) {
                        json_obj[json_key] = simulationMethodToString(v);
                    }
                },
                value);
        }
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }

    return json_obj;
}

void Settings::merge(const Settings& other) {
    for (const auto& key : other.keys_) {
        const auto& value_to_copy = other.settings_.at(key);
        set(key, SettingValue(value_to_copy));
    }
}
