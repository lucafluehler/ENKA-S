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
            setPixmap(QPixmap(QString::fromUtf8(":/file_states/icons/not_found.png")));
            setToolTip("File not found.");
            break;
        case Mode::Loading:
            movie = new QMovie(":/file_states/icons/loading.gif");
            setMovie(movie);
            movie->start();
            setToolTip("Checking file...");
            break;
        case Mode::Checked:
            setPixmap(QPixmap(QString::fromUtf8(":/file_states/icons/checked.png")));
            setToolTip("File checked successfully.");
            break;
        case Mode::Corrupted:
            setPixmap(QPixmap(QString::fromUtf8(":/file_states/icons/corrupted.png")));
            setToolTip("File is corrupted.");
            break;
        default:
            break;
    }
}
