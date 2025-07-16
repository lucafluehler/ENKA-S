#pragma once

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using SettingValue = std::variant<int, double, bool, std::string>;

struct Setting {
    enum class Group { Data, Generation, Simulation };
    enum class Type { Int, Double, Bool, String };

    Group group;
    Type type;
    SettingValue value;
};

class Settings {
public:
    Settings(std::initializer_list<std::pair<const std::string, Setting>> items);
    Settings() = default;

    /**
     * @brief Checks if a setting with the given identifier exists.
     * @param id The identifier of the setting.
     * @return True if the setting exists, false otherwise.
     */
    [[nodiscard]] bool has(std::string_view id) const;

    /**
     * @brief Gets the group of a setting by its identifier.
     * @param id The identifier of the setting.
     * @return The group of the setting.
     */
    [[nodiscard]] Setting::Group groupOf(std::string_view id) const;
    /**
     * @brief Gets the type of a setting by its identifier.
     * @param id The identifier of the setting.
     * @return The type of the setting.
     */
    [[nodiscard]] Setting::Type typeOf(std::string_view id) const;

    /**
     * @brief Gets the value of a setting by its identifier.
     * @param id The identifier of the setting.
     * @return The value of the setting.
     */
    template <typename T>
    [[nodiscard]] auto&& get(this auto&& self, std::string_view id) {
        return std::get<T>(self.settings_.at(std::string(id)).value);
    }

    /**
     * @brief Gets the string representation of a setting's value by its identifier.
     * @param id The identifier of the setting.
     * @return A string representation of the setting's value.
     */
    [[nodiscard]] std::string getString(std::string_view id) const;

    /**
     * @brief Gets the value of a setting by its identifier.
     * @param id The identifier of the setting.
     * @return The value of the setting.
     * @throws std::out_of_range If the identifier does not exist.
     */
    [[nodiscard]] const SettingValue& getValue(std::string_view id) const;

    /**
     * @brief Sets the value of a setting by its identifier.
     * @param id The identifier of the setting.
     * @param new_value The new value to set.
     * @throws std::out_of_range If the identifier does not exist.
     * @throws std::invalid_argument If the type of new_value does not match the setting's
     * registered type.
     */
    template <typename T>
    void set(std::string_view id, T&& new_value) {
        auto it = settings_.find(std::string(id));
        if (it == settings_.end()) {
            throw std::out_of_range("Setting with identifier '" + std::string(id) +
                                    "' does not exist.");
        }

        // Proactive type-checking to provide better error messages.
        const auto expectedType = it->second.type;
        bool typeMatch = false;
        if constexpr (std::is_same_v<std::decay_t<T>, int>) {
            if (expectedType == Setting::Type::Int) typeMatch = true;
        } else if constexpr (std::is_same_v<std::decay_t<T>, double>) {
            if (expectedType == Setting::Type::Double) typeMatch = true;
        } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            if (expectedType == Setting::Type::Bool) typeMatch = true;
        } else if constexpr (std::is_constructible_v<std::string, T>) {
            if (expectedType == Setting::Type::String) typeMatch = true;
        }

        if (!typeMatch) {
            throw std::invalid_argument("Type mismatch for setting '" + std::string(id) + "'.");
        }

        // If the types match, assign the new value.
        it->second.value = std::forward<T>(new_value);
    }

    [[nodiscard]] const std::vector<std::string>& identifiers() const { return ids_; }

private:
    void registerSetting(std::string_view id, Setting&& setting);

    std::vector<std::string> ids_;
    std::unordered_map<std::string, Setting> settings_;
};
