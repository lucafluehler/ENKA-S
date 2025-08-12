#pragma once

#include <enkas/data/system.h>
#include <qobject.h>

#include <QObject>
#include <optional>

#include "core/dataflow/snapshot.h"
#include "core/settings/settings.h"

class FileParseWorker : public QObject {
    Q_OBJECT

public:
    explicit FileParseWorker(QObject* parent = nullptr) : QObject(parent) {};
    ~FileParseWorker() override = default;

public slots:
    /**
     * @brief Parses settings from the specified file.
     * Emits settingsParsed() with an optional Settings object.
     * @param file_path The path to the CSV file.
     */
    void parseSettings(const QString& file_path);

    /**
     * @brief Parses the diagnostics data series from the specified file.
     * Emits diagnosticsSeriesParsed() with an optional DiagnosticsSeries.
     * @param file_path The path to the CSV file.
     */
    void parseDiagnosticsSeries(const QString& file_path);

    /**
     * @brief Opens the system file and initializes the snapshot provider.
     * Emits initialSystemParsed() with the parsed initial system data.
     * @param file_path The path to the CSV file.
     */
    void parseInitialSystem(const QString& file_path);

    /**
     * @brief Counts the number of snapshots in the specified file.
     * Emits snapshotCounted() with the count.
     * @param file_path The path to the CSV file.
     */
    void countSnapshots(const QString& file_path);

    /**
     * @brief Retrieves the simulation duration from the specified file.
     * Emits simulationDurationRetrieved() with the duration if successful.
     * @param file_path The path to the CSV file.
     */
    void retrieveSimulationDuration(const QString& file_path);

signals:
    /** @signal
     * @brief Emitted when settings have been parsed.
     * @param settings The parsed settings data if successful.
     */
    void settingsParsed(const std::optional<Settings>& settings);

    /** @signal
     * @brief Emitted when a diagnostics series has been parsed.
     * @param series The parsed diagnostics data series if successful.
     */
    void diagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);

    /** @signal
     * @brief Emitted when the system file is opened successfully.
     * @param timestamps The timestamps of the system snapshots.
     */
    void initialSystemParsed(const std::optional<enkas::data::System>& initial_system);

    /** @signal
     * @brief Emitted when the number of snapshots has been counted.
     * @param count The number of snapshots found.
     */
    void snapshotsCounted(std::optional<int> count);

    /** @signal
     * @brief Emitted when the simulation duration has been retrieved.
     * @param duration The simulation duration in seconds.
     */
    void simulationDurationRetrieved(std::optional<double> duration);
};
