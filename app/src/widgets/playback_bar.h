#pragma once

#include <QColor>
#include <QObject>
#include <QSlider>

/**
 * @brief A custom playback bar widget for media controls.
 */
class PlaybackBar : public QSlider {
    Q_OBJECT

public:
    explicit PlaybackBar(QWidget* parent = nullptr);
    ~PlaybackBar() override = default;

    void setBufferValue(int buffer_value) {
        buffer_value_ = std::clamp(buffer_value, minimum(), maximum());
        update();  // Trigger a repaint
    }

    int getBufferValue() const { return buffer_value_; }

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* ev) override;

private:
    QColor progress_color_;
    QColor buffer_color_;
    QColor groove_color_;
    const int groove_height_ = 12;
    const int groove_radius_ = 0;

    QColor handle_color;
    QColor handle_hover_color;
    QColor handle_pressed_color;
    const int handle_height = 20;
    const int handle_width = 12;

    int buffer_value_ = 0;
};
