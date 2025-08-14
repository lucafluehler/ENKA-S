#pragma once

#include <QLabel>

/**
 * @brief A label that displays file states with different icons.
 */
class FileCheckIcon : public QLabel {
    Q_OBJECT

public:
    explicit FileCheckIcon(QWidget *parent = nullptr) : QLabel(parent) {}
    ~FileCheckIcon() override = default;

    enum class Mode { NotFound, Loading, Checked, Corrupted };

    void setMode(Mode new_mode);
    Mode getMode() const;

private:
    void updateIcon();

    Mode mode = Mode::NotFound;
};
