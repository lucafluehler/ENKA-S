#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <optional>
#include <vector>

#include "core/files/system_snapshot_stream.h"
#include "core/settings/settings.h"
#include "core/snapshot.h"

class FileParseWorker : public QObject {
    Q_OBJECT

public:
    explicit FileParseWorker(QObject* parent = nullptr) : QObject(parent) {};
    ~FileParseWorker() override = default;

public slots:
    /**
     * @brief Parses settings from the specified file.
     * @param file_path The path to the CSV file.
     */
    void parseSettings(const QString& file_path);

    /**
     * @brief Parses the diagnostics data series from the specified file.
     * Emits diagnosticsSeriesParsed() on success or parsingFailed() on error.
     * @param file_path The path to the CSV file.
     */
    void parseDiagnosticsSeries(const QString& file_path);

    /**
     * @brief Opens the system file and initializes the snapshot provider.
     * @param file_path The path to the CSV file.
     */
    void openSystemFile(const QString& file_path);

    /**
     * @brief Requests the initial system snapshot.
     */
    void requestInitialSnapshot();

    /**
     * @brief Requests a system snapshot at the specified timestamp.
     * @param timestamp The timestamp of the snapshot to request.
     */
    void requestSnapshotAt(double timestamp);

    /**
     * @brief Requests the next system snapshot.
     */
    void requestNextSnapshot();

    /**
     * @brief Requests the previous system snapshot.
     * @param timestamp The timestamp to find the preceding snapshot for.
     */
    void requestPrecedingSnapshot(double timestamp);

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
    void systemFileOpened(const std::optional<std::vector<double>>& timestamps);

    /** @signal
     * @brief Emitted when a system snapshot is ready.
     * @param snapshot The requested system snapshot if available.
     */
    void snapshotReady(const std::optional<SystemSnapshot>& snapshot);

private:
    std::unique_ptr<SystemSnapshotStream> snapshot_provider_;
};
