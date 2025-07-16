#include "settings.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

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

static bool isSettingConsistent(const Setting& setting) {
    // The variant's index corresponds directly to the enum's underlying value
    // 0: int, 1: double, 2: bool, 3: string
    return static_cast<size_t>(setting.type) == setting.value.index();
}

std::optional<Settings> Settings::create(
    std::initializer_list<std::pair<const std::string, Setting>> items) {
    std::unordered_set<std::string> unique_ids;
    unique_ids.reserve(items.size());

    for (const auto& pair : items) {
        const std::string& id = pair.first;

        if (!unique_ids.insert(id).second) {
            return std::nullopt;  // Error: Duplicate key
        }

        if (!isSettingConsistent(pair.second)) {
            return std::nullopt;  // Error: Type enum and value type mismatch
        }
    }

    return Settings(items);
}

bool Settings::addSetting(const std::string& id, Setting&& setting) {
    if (settings_.count(id) > 0) {
        return false;  // Already exists
    }

    if (!isSettingConsistent(setting)) {
        return false;  // Type enum does not match the actual value type in the variant
    }

    ids_.push_back(id);

    settings_.emplace(id, std::move(setting));

    return true;
}

bool Settings::removeSetting(const std::string& id) {
    auto it = settings_.find(id);
    if (it == settings_.end()) {
        return false;  // Not found
    }

    settings_.erase(it);

    auto& vec = ids_;
    vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());

    return true;
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
