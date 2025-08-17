#include "application.h"

#include <enkas/logging/logger.h>

Application::Application(std::unique_ptr<MainWindow> main_window,
                         std::unique_ptr<MainWindowPresenter> main_window_presenter,
                         std::unique_ptr<Services> services)
    : main_window_(std::move(main_window)),
      main_window_presenter_(std::move(main_window_presenter)),
      services_(std::move(services)) {}

void Application::run() {
    ENKAS_LOG_INFO("Application starting up...");
    main_window_->show();
}
