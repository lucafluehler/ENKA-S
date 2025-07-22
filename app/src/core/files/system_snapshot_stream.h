#pragma once

#include <enkas/data/system.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <vector>

#include "core/snapshot.h"

/**
 * @brief An efficient provider for reading SystemSnapshots from a large CSV file.
 *
 * This class performs a one-time indexing pass on the file to enable fast,
 * random-access retrieval of snapshots by timestamp. It is NOT thread-safe and
 * is designed to be owned and operated by a single worker thread.
 */
class SystemSnapshotStream {
public:
    /**
     * @brief Constructs a provider for the given file path.
     * @note The file is not opened or indexed until initialize() is called.
     */
    explicit SystemSnapshotStream(std::filesystem::path file_path);
    ~SystemSnapshotStream();

    // Disable copy/move to ensure single ownership of the file handle.
    SystemSnapshotStream(const SystemSnapshotStream&) = delete;
    SystemSnapshotStream& operator=(const SystemSnapshotStream&) = delete;
    SystemSnapshotStream(SystemSnapshotStream&&) = delete;
    SystemSnapshotStream& operator=(SystemSnapshotStream&&) = delete;

    /**
     * @brief Opens the file and builds the timestamp index. Must be called before any other method.
     * @return True on success, false on failure (e.g., file not found, invalid format).
     */
    bool initialize();

    /**
     * @brief Checks if the provider is successfully initialized.
     */
    bool isInitialized() const { return is_initialized_; }

    /**
     * @brief Retrieves a snapshot at or just after the specified timestamp.
     * @param timestamp The target time.
     * @return The corresponding snapshot, or std::nullopt if none is found.
     */
    std::optional<SystemSnapshot> getSnapshotAt(double timestamp);

    /**
     * @brief Retrieves the snapshot immediately following the last one that was read.
     * @return The next snapshot, or std::nullopt if at the end.
     */
    std::optional<SystemSnapshot> getNextSnapshot();

    /**
     * @brief Retrieves the snapshot immediately preceding the passed timestamp.
     * @param timestamp The target time.
     * @return The previous snapshot, or std::nullopt if at the beginning.
     */
    std::optional<SystemSnapshot> getPrecedingSnapshot(double timestamp);

    /**
     * @brief Retrieves the very first snapshot in the file.
     * @return The first snapshot, or std::nullopt if the file is empty.
     */
    std::optional<SystemSnapshot> getFirstSnapshot();

    /**
     * @brief Returns a sorted list of all unique timestamps found in the file.
     * @return A vector of timestamps. Fast operation after initialization.
     */
    std::vector<double> getAllTimestamps() const;

private:
    struct SnapshotIndexEntry {
        std::streampos file_offset;  // Byte offset where the first row of the snapshot starts
        int row_count;               // Number of rows (particles) in this snapshot
    };

    /**
     * @brief Performs a single pass over the file to map timestamps to file positions.
     * @return True on success, false on failure.
     */
    bool buildIndex();

    /**
     * @brief Parses a single snapshot given its index entry.
     * @param entry The index entry describing where to find the snapshot data.
     * @param timestamp The timestamp for this snapshot.
     * @return The parsed snapshot.
     */
    std::optional<SystemSnapshot> parseSnapshotFromIndex(const SnapshotIndexEntry& entry,
                                                         double timestamp);

    std::filesystem::path file_path_;
    std::ifstream file_stream_;
    bool is_initialized_ = false;

    // This map stores the 0-based index for each column name found in the header.
    // It's populated once during buildIndex() for fast lookups later.
    std::map<std::string, size_t> column_indices_;

    std::map<double, SnapshotIndexEntry> index_;
    std::map<double, SnapshotIndexEntry>::const_iterator current_index_iterator_;
};
