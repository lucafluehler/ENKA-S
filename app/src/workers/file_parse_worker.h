#pragma once

#include <QObject>

#include "core/file_parse_logic.h"
#include "core/settings.h"

class FileParseWorker : public QObject {
    Q_OBJECT

public:
    explicit FileParseWorker(QObject* parent = nullptr);
    ~FileParseWorker() override = default;

public slots:
    /**
     * @brief Asynchronously parses settings from the specified file.
     * Emits settingsParsed() on success or parsingFailed() on error.
     * @param file_path The path to the CSV file.
     */
    void parseSettings(const QString& file_path);

    /**
     * @brief Asynchronously parses the next system frame from the specified file.
     * Emits systemFrameParsed() on success or parsingFailed() on error.
     * @param file_path The path to the CSV file.
     * @param previous_timestamp The timestamp after which to read the next frame.
     */
    void parseNextSystemFrame(const QString& file_path, double previous_timestamp = 0.0);

    /**
     * @brief Asynchronously parses all system timestamps from the specified file.
     * Emits systemTimestampsParsed() on success or parsingFailed() on error.
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
     * @brief Emitted when settings have been successfully parsed.
     * @param settings The parsed settings data.
     */
    void settingsParsed(const Settings& settings);

    /**
     * @brief Emitted when a system frame has been successfully parsed.
     * @param frame The parsed system frame data.
     */
    void systemFrameParsed(const SystemFrame& frame);

    /**
     * @brief Emitted when all system timestamps have been successfully parsed.
     * @param timestamps A vector of all timestamps found in the file.
     */
    void systemTimestampsParsed(const std::vector<double>& timestamps);

    /**
     * @brief Emitted when a diagnostics series has been successfully parsed.
     * @param series The parsed diagnostics data series.
     */
    void diagnosticsSeriesParsed(const DiagnosticsSeries& series);

    /**
     * @brief Emitted when any parsing operation fails.
     * @param error A string describing the failure.
     */
    void parsingFailed(const QString& error);
};

// Register custom types with Qt's Meta-Object System to allow them to be used in
// queued signal/slot connections (i.e., across threads).
Q_DECLARE_METATYPE(Settings);
Q_DECLARE_METATYPE(SystemFrame);
Q_DECLARE_METATYPE(DiagnosticsSeries);
