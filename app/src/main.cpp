#include <QApplication>

#include "app_initializer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);

    AppInitializer initializer;
    initializer.run();

    return app.exec();
}
