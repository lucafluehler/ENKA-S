#include <QApplication>

#include "composition_root.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);

    CompositionRoot composition_root;
    auto application = composition_root.compose();
    application->run();

    return app.exec();
}
