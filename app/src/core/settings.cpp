#include "settings.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

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

Settings::Settings(std::initializer_list<std::pair<const std::string, Setting>> items) {
    ids_.reserve(items.size());
    settings_.reserve(items.size());

    for (const auto& pair : items) {
        registerSetting(pair.first, Setting(pair.second));
    }
}

bool Settings::has(std::string_view id) const { return settings_.contains(std::string(id)); }

Setting::Group Settings::groupOf(std::string_view id) const {
    return settings_.at(std::string(id)).group;
}

Setting::Type Settings::typeOf(std::string_view id) const {
    return settings_.at(std::string(id)).type;
}

std::string Settings::getString(std::string_view id) const {
    auto it = settings_.find(std::string(id));
    if (it == settings_.end()) {
        throw std::out_of_range("Setting with identifier '" + std::string(id) +
                                "' does not exist.");
    }

    const SettingValue& v = it->second.value;

    return std::visit(
        [](const auto& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::setprecision(15) << val;
                return oss.str();
            } else if constexpr (std::is_same_v<T, bool>) {
                return val ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return val;
            }
        },
        v);
}

[[nodiscard]] const SettingValue& Settings::getValue(std::string_view id) const {
    return settings_.at(std::string(id)).value;
}

void Settings::registerSetting(std::string_view id, Setting&& setting) {
    const std::string id_str(id);
    if (settings_.contains(id_str)) {
        return;  // Identifier already exists, do not overwrite.
    }

    ids_.push_back(id_str);
    settings_.emplace(id_str, std::move(setting));
}
