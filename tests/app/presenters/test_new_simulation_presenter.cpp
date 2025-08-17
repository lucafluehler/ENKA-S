#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QObject>

#include "mocks/mock_file_parser.h"
#include "mocks/mock_new_simulation_view.h"
#include "mocks/mock_simulation_runner.h"
#include "mocks/mock_simulation_runner_factory.h"
#include "mocks/mock_task_runner.h"
#include "presenters/new_simulation/new_simulation_presenter.h"

using namespace ::testing;

class NewSimulationPresenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        presenter_ = std::make_unique<NewSimulationPresenter>(
            &mock_view_, mock_parser_, mock_runner_, mock_factory_, &parent_object_);
    }

    QObject parent_object_;
    NiceMock<MockNewSimulationView> mock_view_;
    NiceMock<MockFileParser> mock_parser_;
    MockTaskRunner mock_runner_;

    NiceMock<MockSimulationRunnerFactory> mock_factory_;
    std::unique_ptr<NewSimulationPresenter> presenter_;
};

TEST_F(NewSimulationPresenterTest, CheckSettingsFile_ParsesAndProcesses) {
    // ARRANGE: View provides path to settings file
    const QString settings_path = "path/to/settings.json";
    EXPECT_CALL(mock_view_, getSettingsPath()).WillOnce(Return(settings_path));

    // ARRANGE: Parser returns valid settings
    Settings fake_settings;
    EXPECT_CALL(mock_parser_, parseSettings(_)).WillOnce(Return(fake_settings));

    // ASSERT: Presenter passes parsed settings back to view
    EXPECT_CALL(mock_view_, processSettings(Optional(fake_settings)));

    // ACT: Trigger the file check.
    presenter_->checkSettingsFile();
    QCoreApplication::processEvents();
}

TEST_F(NewSimulationPresenterTest, StartSimulation_CreatesAndStartsRunner) {
    // ARRANGE: View provides settings
    Settings fake_settings;
    fake_settings.set(SettingKey::Duration, 100.0);
    EXPECT_CALL(mock_view_, fetchSettings()).WillOnce(Return(fake_settings));

    // ARRANGE: Factory creates mock runner
    auto mock_runner_ptr = std::make_unique<NiceMock<MockSimulationRunner>>(fake_settings);
    auto* raw_mock_runner = mock_runner_ptr.get();
    EXPECT_CALL(mock_factory_, create(_)).WillOnce(Return(ByMove(std::move(mock_runner_ptr))));

    // ASSERT: Runner is started
    EXPECT_CALL(*raw_mock_runner, startSimulationProcedure());

    // ACT
    presenter_->startSimulation();
}

TEST_F(NewSimulationPresenterTest, OnInitializationCompleted_UpdatesView) {
    // ARRANGE: Start simulation to create runner
    Settings fake_settings;
    auto mock_runner_ptr = std::make_unique<NiceMock<MockSimulationRunner>>(fake_settings);
    auto* raw_mock_runner = mock_runner_ptr.get();
    EXPECT_CALL(mock_view_, fetchSettings()).WillOnce(Return(fake_settings));
    EXPECT_CALL(mock_factory_, create(_)).WillOnce(Return(ByMove(std::move(mock_runner_ptr))));
    presenter_->startSimulation();

    // ASSERT: View shows simulation progress when init completes
    EXPECT_CALL(mock_view_, showSimulationProgress());

    // ACT: Simulate initialization completion
    raw_mock_runner->emitInitializationCompleted();
}

TEST_F(NewSimulationPresenterTest, AbortSimulation_CleansUpRunnerAndNotifiesView) {
    // ARRANGE: Start simulation to create runner
    Settings fake_settings;
    auto mock_runner_ptr = std::make_unique<NiceMock<MockSimulationRunner>>(fake_settings);
    EXPECT_CALL(mock_view_, fetchSettings()).WillOnce(Return(fake_settings));
    EXPECT_CALL(mock_factory_, create(_)).WillOnce(Return(ByMove(std::move(mock_runner_ptr))));
    presenter_->startSimulation();

    // ASSERT: View is notified of abort
    EXPECT_CALL(mock_view_, simulationAborted());

    // ACT: Abort simulation
    presenter_->abortSimulation();
    QCoreApplication::processEvents();  // allow deleteLater to run

    // ASSERT (indirect): Runner is null → openSimulationWindow should do nothing
    presenter_->openSimulationWindow();
}
