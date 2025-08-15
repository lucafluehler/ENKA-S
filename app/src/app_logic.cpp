#include "app_logic.h"

#include <enkas/logging/logger.h>

AppLogic::AppLogic(std::unique_ptr<MainWindow> main_window,
                   std::unique_ptr<MainWindowPresenter> main_window_presenter)
    : main_window_(std::move(main_window)),
      main_window_presenter_(std::move(main_window_presenter)) {}

void AppLogic::run() {
    ENKAS_LOG_INFO("Application starting up...");
    main_window_->show();
}
