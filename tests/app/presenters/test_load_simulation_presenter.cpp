#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QObject>

#include "mocks/mock_file_parser.h"
#include "mocks/mock_load_simulation_view.h"
#include "mocks/mock_simulation_player.h"
#include "mocks/mock_simulation_player_factory.h"
#include "mocks/mock_task_runner.h"
#include "presenters/load_simulation/load_simulation_presenter.h"

using namespace ::testing;

class LoadSimulationPresenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        presenter_ = std::make_unique<LoadSimulationPresenter>(
            &mock_view_, mock_parser_, mock_runner_, mock_factory_, &parent_object_);
    }

    QObject parent_object_;
    NiceMock<MockLoadSimulationView> mock_view_;
    NiceMock<MockFileParser> mock_parser_;
    MockTaskRunner mock_runner_;

    NiceMock<MockSimulationPlayerFactory> mock_factory_;
    std::unique_ptr<LoadSimulationPresenter> presenter_;
};

TEST_F(LoadSimulationPresenterTest, CheckFiles_ParsesSettingsAndUpdatesView) {
    // ARRANGE: View provides path to settings file
    const QString settings_path = "path/to/settings.json";
    EXPECT_CALL(mock_view_, getFilesToCheck()).WillOnce(Return(QVector<QString>{settings_path}));

    // ARRANGE: Parser returns valid settings
    Settings fake_settings;
    fake_settings.set(SettingKey::Duration, 100.0);
    EXPECT_CALL(mock_parser_, parseSettings(_)).WillOnce(Return(fake_settings));

    // ASSERT: View is updated with parsed settings
    EXPECT_CALL(mock_view_, onSettingsParsed(Optional(fake_settings)));

    // ACT
    presenter_->checkFiles();
    QCoreApplication::processEvents();
}

TEST_F(LoadSimulationPresenterTest, CheckFiles_HandlesAllFileTypes) {
    // ARRANGE: View provides all three file types
    const QString settings_path = "path/to/settings.json";
    const QString system_path = "path/to/system.csv";
    const QString diagnostics_path = "path/to/diagnostics.csv";
    EXPECT_CALL(mock_view_, getFilesToCheck())
        .WillOnce(Return(QVector<QString>{settings_path, system_path, diagnostics_path}));

    // ARRANGE: Parser returns successful results for each file
    Settings fake_settings;
    enkas::data::System fake_system(1);
    DiagnosticsSeries fake_diagnostics_series(1);

    EXPECT_CALL(mock_parser_, parseSettings(_)).WillOnce(Return(fake_settings));
    EXPECT_CALL(mock_parser_, parseInitialSystem(_)).WillOnce(Return(fake_system));
    EXPECT_CALL(mock_parser_, countSnapshots(_)).WillOnce(Return(100));
    EXPECT_CALL(mock_parser_, retrieveSimulationDuration(_)).WillOnce(Return(50.0));
    EXPECT_CALL(mock_parser_, parseDiagnosticsSeries(_)).WillOnce(Return(fake_diagnostics_series));

    // ASSERT: View receives parsed data from all file types
    EXPECT_CALL(mock_view_, onSettingsParsed(Optional(fake_settings)));
    EXPECT_CALL(mock_view_, onInitialSystemParsed(Optional(fake_system)));
    EXPECT_CALL(mock_view_, onDiagnosticsSeriesParsed(true));

    // ACT
    presenter_->checkFiles();
    QCoreApplication::processEvents();
}

TEST_F(LoadSimulationPresenterTest, PlaySimulation_CreatesAndRunsPlayer) {
    // ARRANGE: Factory creates mock player
    auto mock_player_ptr = std::make_unique<NiceMock<MockSimulationPlayer>>();
    auto* raw_mock_player = mock_player_ptr.get();
    EXPECT_CALL(mock_factory_, create()).WillOnce(Return(ByMove(std::move(mock_player_ptr))));

    // ASSERT: Player is run after creation
    EXPECT_CALL(*raw_mock_player, run(_, _));

    // ACT
    presenter_->playSimulation();
}

TEST_F(LoadSimulationPresenterTest, EndSimulationPlayback_CleansUpAndResetsState) {
    // ARRANGE: Start playback to create player instance
    auto mock_player_ptr = std::make_unique<NiceMock<MockSimulationPlayer>>();
    auto* raw_mock_player = mock_player_ptr.get();
    EXPECT_CALL(mock_factory_, create()).WillOnce(Return(ByMove(std::move(mock_player_ptr))));
    presenter_->playSimulation();

    // ACT: Simulate closing simulation window
    raw_mock_player->emitWindowClosed();
    QCoreApplication::processEvents();  // allow deleteLater to run

    // ASSERT: Presenter can start a new simulation after cleanup
    auto new_mock_player_ptr = std::make_unique<NiceMock<MockSimulationPlayer>>();
    EXPECT_CALL(mock_factory_, create()).WillOnce(Return(ByMove(std::move(new_mock_player_ptr))));
    presenter_->playSimulation();  // no crash means reset worked
}
