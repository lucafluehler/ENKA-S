#include "main_window_presenter.h"

#include <QTimer>

#include "presenters/load_simulation/i_load_simulation_presenter.h"
#include "presenters/new_simulation/i_new_simulation_presenter.h"
#include "views/main_window/i_main_window_view.h"

namespace {
constexpr int kHomeScreenRefreshRateFps = 30;
constexpr int kNewSimulationTabIndex = 1;
constexpr int kLoadSimulationTabIndex = 2;
}  // namespace

MainWindowPresenter::MainWindowPresenter(IMainWindowView* view,
                                         ILoadSimulationPresenter* load_simulation_presenter,
                                         INewSimulationPresenter* new_simulation_presenter,
                                         QObject* parent)
    : QObject(parent),
      view_(view),
      load_simulation_presenter_(load_simulation_presenter),
      new_simulation_presenter_(new_simulation_presenter),
      home_screen_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    // Update the home screen particle preview 30 times per second
    connect(home_screen_timer_, &QTimer::timeout, this, &MainWindowPresenter::updateHomeScreen);
    home_screen_timer_->start(1000 / kHomeScreenRefreshRateFps);
}

void MainWindowPresenter::onTabSwitched() {
    auto index = view_->getCurrentTabIndex();
    if (index == kLoadSimulationTabIndex) {
        load_simulation_presenter_->active();
        new_simulation_presenter_->inactive();
    } else if (index == kNewSimulationTabIndex) {
        load_simulation_presenter_->inactive();
        new_simulation_presenter_->active();
    } else {
        load_simulation_presenter_->inactive();
        new_simulation_presenter_->inactive();
    }
}
