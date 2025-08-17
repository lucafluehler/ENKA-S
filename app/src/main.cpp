#include <QApplication>

#include "composition_root.h"

int main(int argc, char *argv[]) {
    QApplication qt_app(argc, argv);
    Q_INIT_RESOURCE(resources);

    auto application = CompositionRoot::compose();
    application->run();

    return qt_app.exec();
}
