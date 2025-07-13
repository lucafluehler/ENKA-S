#include "file_check_icon.h"

#include <QMovie>

FileCheckIcon::FileCheckIcon(QWidget *parent) : QLabel(parent) { setMode(Mode::NotFound); }

void FileCheckIcon::setMode(Mode new_mode) {
    mode = new_mode;
    updateIcon();
}

FileCheckIcon::Mode FileCheckIcon::getMode() const { return mode; }

void FileCheckIcon::updateIcon() {
    QMovie *movie;

    switch (mode) {
        case Mode::NotFound:
            setPixmap(QPixmap(QString::fromUtf8(":/LoadDiagnosticsTab/img/not_found.png")));
            setToolTip("Daten nicht gefunden.");
            break;
        case Mode::Loading:
            movie = new QMovie(":/LoadDiagnosticsTab/img/loading.gif");
            setMovie(movie);
            movie->start();
            setToolTip("Daten werden überprüft.");
            break;
        case Mode::Checked:
            setPixmap(QPixmap(QString::fromUtf8(":/LoadDiagnosticsTab/img/checked.png")));
            setToolTip("Die Daten wurden erfolgreich geprüft.");
            break;
        case Mode::Corrupted:
            setPixmap(QPixmap(QString::fromUtf8(":/LoadDiagnosticsTab/img/corrupted.png")));
            setToolTip("Die Daten sind fehlerhaft.");
            break;
        default:
            break;
    }
}
