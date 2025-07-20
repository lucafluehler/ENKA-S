#include "main_window_presenter.h"

#include <QTimer>

#include "views/main_window/i_main_window_view.h"

MainWindowPresenter::MainWindowPresenter(IMainWindowView* view, QObject* parent)
    : QObject(parent), view_(view), home_screen_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    // Update the home screen particle preview 30 times per second
    connect(home_screen_timer_, &QTimer::timeout, this, &MainWindowPresenter::updateHomeScreen);
    const int fps = 30;  // Frames per second
    home_screen_timer_->start(1000 / fps);
}
