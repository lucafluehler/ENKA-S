#include <QApplication>

#include "presenters/main_window_presenter.h"
#include "views/main_window/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow main_window;
    MainWindowPresenter presenter(&main_window);

    main_window.show();
    return app.exec();
}
