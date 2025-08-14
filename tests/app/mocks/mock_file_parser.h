#pragma once

#include <gmock/gmock.h>

#include "core/files/i_file_parse_logic.h"

class MockFileParser : public IFileParseLogic {
public:
    MOCK_METHOD(std::optional<Settings>, parseSettings, (const std::filesystem::path&), (override));
    MOCK_METHOD(std::optional<DiagnosticsSeries>,
                parseDiagnosticsSeries,
                (const std::filesystem::path&),
                (override));
    MOCK_METHOD(std::optional<enkas::data::System>,
                parseInitialSystem,
                (const std::filesystem::path&),
                (override));
    MOCK_METHOD(std::optional<int>, countSnapshots, (const std::filesystem::path&), (override));
    MOCK_METHOD(std::optional<double>,
                retrieveSimulationDuration,
                (const std::filesystem::path&),
                (override));
};
