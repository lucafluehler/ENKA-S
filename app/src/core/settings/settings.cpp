#include "settings.h"

#include <initializer_list>
#include <json/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

std::optional<Settings> Settings::create(
    std::initializer_list<std::pair<std::string_view, SettingValue>> items) {
    std::unordered_set<std::string_view> unique_ids;
    for (const auto& [key, _] : items) {
        if (!unique_ids.insert(key).second) {
            return std::nullopt;
        }
    }
    return Settings(items);
}

std::optional<Settings> Settings::create(
    std::initializer_list<std::pair<std::string, SettingValue>> items) {
    std::unordered_set<std::string_view> unique_ids;
    for (const auto& [key, _] : items) {
        if (!unique_ids.insert(key).second) {
            return std::nullopt;
        }
    }
    return Settings(items);
}

std::optional<Settings> Settings::create(const nlohmann::json& json) {
    if (!json.is_object()) return std::nullopt;

    std::vector<std::pair<std::string_view, SettingValue>> items;
    items.reserve(json.size());

    for (auto it = json.begin(); it != json.end(); ++it) {
        const std::string& key = it.key();
        const nlohmann::json& val = it.value();

        if (val.is_number_integer()) {
            items.emplace_back(key, val.get<int>());
        } else if (val.is_number_float()) {
            items.emplace_back(key, val.get<double>());
        } else if (val.is_boolean()) {
            items.emplace_back(key, val.get<bool>());
        } else if (val.is_string()) {
            items.emplace_back(key, val.get<std::string>());
        } else {
            return std::nullopt;  // Unsupported type
        }
    }

    return Settings(items);
}

Settings::Settings(std::initializer_list<std::pair<std::string_view, SettingValue>> items) {
    ids_.reserve(items.size());
    settings_.reserve(items.size());
    for (const auto& [key, value] : items) {
        ids_.emplace_back(key);
        settings_.emplace(std::string(key), value);
    }
}

Settings::Settings(std::initializer_list<std::pair<std::string, SettingValue>> items) {
    ids_.reserve(items.size());
    settings_.reserve(items.size());
    for (const auto& [key, value] : items) {
        ids_.emplace_back(key);
        settings_.emplace(key, value);
    }
}

Settings::Settings(const std::vector<std::pair<std::string_view, SettingValue>>& items) {
    ids_.reserve(items.size());
    settings_.reserve(items.size());
    for (const auto& pair : items) {
        ids_.push_back(std::string(pair.first));
        settings_.emplace(std::string(pair.first), pair.second);
    }
}

bool Settings::has(std::string_view id) const { return settings_.count(std::string(id)) > 0; }

bool Settings::addSetting(const std::string& id, SettingValue&& value) {
    if (has(id)) {
        return false;  // Key already exists
    }
    ids_.push_back(id);
    settings_.emplace(id, std::move(value));
    return true;
}

void Settings::set(const std::string& id, SettingValue&& new_value) {
    if (!has(id)) {
        ids_.push_back(id);
    }
    settings_[id] = std::move(new_value);
}

bool Settings::removeSetting(std::string_view id) {
    if (!has(id)) {
        return false;
    }
    settings_.erase(std::string{id});
    ids_.erase(std::remove(ids_.begin(), ids_.end(), id), ids_.end());
    return true;
}

std::optional<nlohmann::json> Settings::toJson() const {
    nlohmann::json json_obj;

    for (const auto& [key, value] : settings_) {
        bool supported = std::visit(
            [&](auto&& v) -> bool {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, int>) {
                    json_obj[key] = v;
                    return true;
                } else if constexpr (std::is_same_v<T, double>) {
                    json_obj[key] = v;
                    return true;
                } else if constexpr (std::is_same_v<T, bool>) {
                    json_obj[key] = v;
                    return true;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    json_obj[key] = v;
                    return true;
                } else {
                    return false;  // unsupported type
                }
            },
            value);

        if (!supported) {
            return std::nullopt;
        }
    }

    return json_obj;
}

void Settings::merge(const Settings& other) {
    for (const auto& id : other.identifiers()) {
        auto it = other.settings_.find(id);
        if (it != other.settings_.end()) {
            set(id, SettingValue(it->second));
        }
    }
}
