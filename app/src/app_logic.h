#include <memory>

#include "presenters/main_window_presenter.h"
#include "views/main_window/main_window.h"

class AppLogic {
public:
    AppLogic(std::unique_ptr<MainWindow> main_window,
             std::unique_ptr<MainWindowPresenter> main_window_presenter);

    void run();

private:
    std::unique_ptr<MainWindow> main_window_;
    std::unique_ptr<MainWindowPresenter> main_window_presenter_;
};
