#pragma once

#include <initializer_list>
#include <json/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using SettingValue = std::variant<int, double, bool, std::string>;

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
        std::initializer_list<std::pair<std::string_view, SettingValue>> items);
    static std::optional<Settings> create(
        std::initializer_list<std::pair<std::string, SettingValue>> items);

    /**
     * @brief Creates a Settings object from a JSON object.
     * @param json The JSON object containing settings data.
     * @return A std::optional<Settings> containing the object if the JSON is valid,
     *         otherwise std::nullopt.
     */
    static std::optional<Settings> create(const nlohmann::json& json);

    /**
     * @brief Adds a new setting if one with the same identifier does not already exist.
     * @param id The unique identifier for the new setting.
     * @param value The setting value to be added.
     * @return True if the setting was successfully added, false if a setting with that ID
     *         already exists.
     */
    bool addSetting(const std::string& id, SettingValue&& value);

    /**
     * @brief Removes a setting by its identifier.
     * @param id The identifier of the setting to remove.
     * @return True if the setting was found and removed, false otherwise.
     */
    bool removeSetting(std::string_view id);

    /**
     * @brief Checks if a setting with the given identifier exists.
     * @param id The identifier of the setting.
     * @return True if the setting exists, false otherwise.
     */
    [[nodiscard]] bool has(std::string_view id) const;

    /**
     * @brief Gets the value of a setting by its identifier.
     * @param id The identifier of the setting.
     * @return The value of the setting.
     */
    template <typename T>
    [[nodiscard]] auto&& get(this auto&& self, std::string_view id) {
        return std::get<T>(std::forward<decltype(self)>(self).settings_.at(std::string(id)));
    }

    /**
     * @brief Sets or updates the value of a setting. If the key exists, its value
     *        (and its type) will be overwritten. If it doesn't exist, it will be created.
     * @param id The identifier of the setting.
     * @param new_value The new value to set for the setting.
     */
    void set(const std::string& id, SettingValue&& new_value);

    /**
     * @brief Gets the identifiers of all settings.
     * @return A vector containing the identifiers of all settings.
     */
    [[nodiscard]] const std::vector<std::string>& identifiers() const { return ids_; }

    /**
     * @brief Converts the Settings object to a JSON object.
     * @return A std::optional<nlohmann::json> containing the JSON representation of the settings,
     *         or std::nullopt if the conversion fails.
     */
    std::optional<nlohmann::json> toJson() const;

private:
    Settings(std::initializer_list<std::pair<std::string_view, SettingValue>> items);
    Settings(std::initializer_list<std::pair<std::string, SettingValue>> items);
    Settings(const std::vector<std::pair<std::string_view, SettingValue>>& items);

    std::vector<std::string> ids_;
    std::unordered_map<std::string, SettingValue> settings_;
};
