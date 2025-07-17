#pragma once

#include <QObject>

#include "views/main_window/i_main_window_view.h"

class QTimer;

class MainWindowPresenter : public QObject {
    Q_OBJECT
public:
    explicit MainWindowPresenter(IMainWindowView* view, QObject* parent = nullptr);

private slots:
    void updateHomeScreen();

private:
    IMainWindowView* view_ = nullptr;
    QTimer* home_screen_timer_ = nullptr;
};
