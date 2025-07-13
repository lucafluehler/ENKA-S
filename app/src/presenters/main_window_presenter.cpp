#include "main_window_presenter.h"

#include <QDebug>
#include <QTimer>

#include "../views/main_window/i_main_window_view.h"

MainWindowPresenter::MainWindowPresenter(IMainWindowView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      home_timer_(new QTimer(this)),
      load_simulation_presenter_(new LoadSimulationPresenter(view->getLoadSimulationView(), this)),
      new_simulation_presenter_(new NewSimulationPresenter(view->getNewSimulationView(), this)) {
    // Update the home screen particle preview 30 times per second
    connect(home_timer_, &QTimer::timeout, this, &MainWindowPresenter::onTimerTimeout);
    const int fps = 30;  // Frames per second
    home_timer_->start(1000 / fps);
}

void MainWindowPresenter::onTimerTimeout() {
    if (!view_) return;
    view_->updatePreview();
}

void MainWindowPresenter::onTabChanged(int index) {
    load_simulation_presenter_->isSelected(index == 2);
}
