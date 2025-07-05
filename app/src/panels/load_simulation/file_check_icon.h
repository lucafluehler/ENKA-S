#ifndef FILE_CHECK_ICON_H_
#define FILE_CHECK_ICON_H_

#include <QLabel>

class FileCheckIcon : public QLabel
{
    Q_OBJECT

public:
    FileCheckIcon(QWidget *parent = nullptr);

    enum class Mode {
        NotFound,
        Loading,
        Checked,
        Corrupted
    };

    void setMode(Mode new_mode);
    Mode getMode() const;

private:
    void updateIcon();

    Mode mode;
};

#endif // FILE_CHECK_ICON_H_
