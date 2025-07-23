#include <cmath>
#include <functional>
#include <limits>
#include <memory>

#include "core/snapshot.h"

/**
 * @brief Creates a stateful value extractor that calculates the percentage change
 *        of a value relative to its first observed value.
 *
 * This function is a factory that returns a lambda. The returned lambda captures a
 * shared_ptr to store the initial value. On its first call, it records the
 * value. On all subsequent calls, it computes the percentage change.
 *
 * @param base_extractor A function that extracts the raw value (e.g., total energy).
 * @return A new std::function that can be used as a value_extractor in ChartDefinition.
 */
inline std::function<double(const DiagnosticsSnapshot&)> createPercentageChangeExtractor(
    const std::function<double(const DiagnosticsSnapshot&)>& base_extractor) {
    auto initial_value = std::make_shared<double>(std::numeric_limits<double>::quiet_NaN());

    return [initial_value, base_extractor](const DiagnosticsSnapshot& diag) -> double {
        const double current_value = base_extractor(diag);

        if (std::isnan(*initial_value)) {
            *initial_value = current_value;
            return 0.0;
        }

        if (*initial_value == 0.0) {
            return 0.0;
        }

        return ((current_value - *initial_value) / *initial_value) * 100.0;
    };
}
