#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QTest>
#include <QTimer>
#include <QSignalSpy>

#include "mocks/mock_main_window_view.h"
#include "presenters/main_window_presenter.h"

class MainWindowPresenterTest : public ::testing::Test {
protected:
    MockMainWindowView mock_view;
};

TEST_F(MainWindowPresenterTest, ConstructorStartsTimer) {
    MainWindowPresenter presenter(&mock_view);

    QTimer* timer = presenter.findChild<QTimer*>();
    ASSERT_NE(timer, nullptr);
    QCoreApplication::processEvents();

    EXPECT_TRUE(timer->isActive());
    EXPECT_EQ(timer->interval(), 1000 / 30);
}

TEST_F(MainWindowPresenterTest, TimerTimeoutUpdatesHomeScreen) {
    const int expected_updates = 3;
    EXPECT_CALL(mock_view, updateHomeScreen()).Times(expected_updates);

    MainWindowPresenter presenter(&mock_view);
    QTimer* timer = presenter.findChild<QTimer*>();
    ASSERT_NE(timer, nullptr);

    QSignalSpy spy(timer, &QTimer::timeout);

    // Wait until at least 3 timeout signals are emitted or timeout
    const int max_wait_ms = 200;

    int elapsed = 0;
    while (spy.count() < expected_updates && elapsed < max_wait_ms) {
        QTest::qWait(10);
        elapsed += 10;
    }

    EXPECT_GE(spy.count(), expected_updates);
}
