#include "settings.h"

#include <string>
#include <unordered_set>

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