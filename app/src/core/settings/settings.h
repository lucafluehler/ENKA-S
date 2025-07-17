#pragma once

#include <initializer_list>
#include <json/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "generation_method.h"
#include "setting_key.h"
#include "simulation_method.h"

using SettingValue =
    std::variant<int, double, bool, std::string, GenerationMethod, SimulationMethod>;

class Settings {
public:
    Settings() = default;

    /**
     * @brief Safely creates a Settings object from an initializer list.
     * @param items An initializer list of pairs, where each pair contains a unique identifier
     *              and a SettingValue.
     * @return A std::optional<Settings> containing the object if all items are valid,
     *         otherwise std::nullopt.
     */
    static std::optional<Settings> create(
        std::initializer_list<std::pair<SettingKey, SettingValue>> items);

    /**
     * @brief Creates a Settings object from a JSON object.
     * @param json The JSON object containing settings data.
     * @return A std::optional<Settings> containing the object if the JSON is valid,
     *         otherwise std::nullopt.
     */
    static std::optional<Settings> create(const nlohmann::json& json);

    /**
     * @brief Adds a new setting if one with the same identifier does not already exist.
     * @param key The identifier for the setting.
     * @param value The setting value to be added.
     * @return True if the setting was successfully added, false if a setting with that ID
     *         already exists.
     */
    bool addSetting(SettingKey key, SettingValue&& value);

    /**
     * @brief Removes a setting by its identifier.
     * @param key The identifier of the setting to remove.
     * @return True if the setting was found and removed, false otherwise.
     */
    bool removeSetting(SettingKey key);

    /**
     * @brief Checks if a setting with the given identifier exists.
     * @param key The identifier of the setting.
     * @return True if the setting exists, false otherwise.
     */
    [[nodiscard]] bool has(SettingKey key) const;

    /**
     * @brief Gets the value of a setting by its identifier.
     * @param key The identifier of the setting.
     * @return The value of the setting.
     */
    template <typename T>
    [[nodiscard]] auto&& get(SettingKey key) const {
        return std::get<T>(settings_.at(key));
    }

    /**
     * @brief Sets or updates the value of a setting. If the key exists, its value
     *        (and its type) will be overwritten. If it doesn't exist, it will be created.
     * @param key The identifier of the setting.
     * @param new_value The new value to set for the setting.
     */
    void set(SettingKey key, SettingValue&& new_value);

    /**
     * @brief Gets the identifiers of all settings.
     * @return A vector containing the identifiers of all settings.
     */
    [[nodiscard]] const std::vector<SettingKey>& identifiers() const { return keys_; }

    /**
     * @brief Converts the Settings object to a JSON object.
     * @return A std::optional<nlohmann::json> containing the JSON representation of the settings,
     *         or std::nullopt if the conversion fails.
     */
    std::optional<nlohmann::json> toJson() const;

    /**
     * @brief Merges another Settings object into this one.
     * @param other The Settings object to merge.
     */
    void merge(const Settings& other);

private:
    Settings(std::initializer_list<std::pair<SettingKey, SettingValue>> items);
    Settings(const std::vector<std::pair<SettingKey, SettingValue>>& items);

    std::vector<SettingKey> keys_;
    std::unordered_map<SettingKey, SettingValue> settings_;
};
