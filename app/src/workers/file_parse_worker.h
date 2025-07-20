#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <optional>
#include <vector>

#include "core/file_parse_logic.h"
#include "core/settings/settings.h"
#include "core/snapshot.h"

class FileParseWorker : public QObject {
    Q_OBJECT

public:
    explicit FileParseWorker(QObject* parent = nullptr);
    ~FileParseWorker() override = default;

public slots:
    /**
     * @brief Asynchronously parses settings from the specified file.
     * @param file_path The path to the CSV file.
     */
    void parseSettings(const QString& file_path);

    /**
     * @brief Asynchronously parses the next system snapshot from the specified file.
     * @param file_path The path to the CSV file.
     * @param previous_timestamp The timestamp after which to read the next snapshot.
     */
    void parseNextSystemSnapshot(const QString& file_path, double previous_timestamp = 0.0);

    /**
     * @brief Asynchronously parses the initial system from the specified file.
     * @param file_path The path to the CSV file.
     */
    void parseInitialSystem(const QString& file_path);

    /**
     * @brief Asynchronously parses all system timestamps from the specified file.
     * @param file_path The path to the CSV file.
     */
    void parseSystemTimestamps(const QString& file_path);

    /**
     * @brief Asynchronously parses the diagnostics data series from the specified file.
     * Emits diagnosticsSeriesParsed() on success or parsingFailed() on error.
     * @param file_path The path to the CSV file.
     */
    void parseDiagnosticsSeries(const QString& file_path);

signals:
    /**
     * @brief Emitted when settings have been parsed.
     * @param settings The parsed settings data if successful.
     */
    void settingsParsed(const std::optional<Settings>& settings);

    /**
     * @brief Emitted when a system snapshot has been parsed.
     * @param snapshot The parsed system snapshot data if successful.
     */
    void systemSnapshotParsed(const std::optional<SystemSnapshot>& snapshot);

    /**
     * @brief Emitted when the initial system has been parsed.
     * @param system The parsed initial system data if successful.
     */
    void initialSystemParsed(const std::optional<enkas::data::System>& system);

    /**
     * @brief Emitted when all system timestamps have been parsed.
     * @param timestamps A vector of all timestamps found in the file if successful.
     */
    void systemTimestampsParsed(const std::optional<std::vector<double>>& timestamps);

    /**
     * @brief Emitted when a diagnostics series has been parsed.
     * @param series The parsed diagnostics data series if successful.
     */
    void diagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);
};
