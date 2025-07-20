#pragma once

#include <QObject>

#include "views/main_window/i_main_window_view.h"

class QTimer;

/**
 * @brief Responsible for managing the main window's view and updating the home screen.
 */
class MainWindowPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Initializes the presenter with the given view and sets up a timer to update the home
     * screen.
     * @param view The main window view to be managed.
     * @param parent The parent QObject, defaults to nullptr.
     */
    explicit MainWindowPresenter(IMainWindowView* view, QObject* parent = nullptr);
    ~MainWindowPresenter() override = default;

private slots:
    void updateHomeScreen() { view_->updateHomeScreen(); }

private:
    IMainWindowView* view_ = nullptr;
    QTimer* home_screen_timer_ = nullptr;
};
