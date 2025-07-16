#pragma once

#include <QObject>
#include <optional>
#include <vector>

#include "core/file_parse_logic.h"
#include "core/settings.h"
#include "enkas/data/system.h"

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
     * @brief Asynchronously parses the next system frame from the specified file.
     * @param file_path The path to the CSV file.
     * @param previous_timestamp The timestamp after which to read the next frame.
     */
    void parseNextSystemFrame(const QString& file_path, double previous_timestamp = 0.0);

    /**
     * @brief Asynchronously parses the initial system frame from the specified file.
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
     * @brief Emitted when a system frame has been parsed.
     * @param frame The parsed system frame data if successful.
     */
    void systemFrameParsed(const std::optional<SystemFrame>& frame);

    /**
     * @brief Emitted when the initial system frame has been parsed.
     * @param frame The parsed initial system frame data if successful.
     */
    void initialSystemParsed(const std::optional<enkas::data::System>& frame);

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

Q_DECLARE_METATYPE(std::optional<Settings>);
Q_DECLARE_METATYPE(std::optional<SystemFrame>);
Q_DECLARE_METATYPE(std::optional<std::vector<double>>);
Q_DECLARE_METATYPE(std::optional<DiagnosticsSeries>);
