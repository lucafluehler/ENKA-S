#pragma once

#include <QObject>
#include <filesystem>
#include <memory>
#include <optional>

#include "core/dataflow/snapshot.h"

class ISimulationPlayer : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Struct representing required system data for the simulation replay.
     */
    struct SystemData {
        std::filesystem::path file_path = "";
        double simulation_duration = 0.0;
        std::size_t total_snapshots_count = 0;
    };

    /**
     * @brief Struct representing required diagnostics data for the simulation replay.
     */
    struct DiagnosticsData {
        std::shared_ptr<DiagnosticsSeries> diagnostics_series = nullptr;
    };

    explicit ISimulationPlayer(QObject* parent = nullptr) : QObject(parent) {}
    ~ISimulationPlayer() override = default;

    /**
     * @brief Initializes the simulation player with the loaded data.
     * @param system_data Optional system data containing the file path, duration, and total
     * snapshot count.
     * @param diagnostics_data Optional diagnostics data containing the shared pointer to a
     * DiagnosticsSeries.
     */
    virtual void run(std::optional<SystemData> system_data,
                     std::optional<DiagnosticsData> diagnostics_data) = 0;

signals:
    /** @signal
     * @brief Emitted when the simulation window is closed.
     */
    void windowClosed();
};
