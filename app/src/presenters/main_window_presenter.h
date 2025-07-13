#pragma once

#include <QObject>

#include "../views/main_window/i_main_window_view.h"
#include "load_simulation_presenter.h"
#include "new_simulation_presenter.h"

class QTimer;

class MainWindowPresenter : public QObject {
    Q_OBJECT
public:
    explicit MainWindowPresenter(IMainWindowView* view, QObject* parent = nullptr);

public slots:
    void onTabChanged(int index);

private slots:
    void onTimerTimeout();

private:
    IMainWindowView* view_ = nullptr;
    QTimer* home_timer_ = nullptr;

    LoadSimulationPresenter* load_simulation_presenter_ = nullptr;
    NewSimulationPresenter* new_simulation_presenter_ = nullptr;
};
