#pragma once

#include <QColor>
#include <QObject>
#include <QSlider>

class PlaybackBar : public QSlider {
    Q_OBJECT

public:
    explicit PlaybackBar(QWidget* parent = nullptr) : QSlider(parent) {};
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
    const QColor progress_color_ = QRgb(0x77A8D6);
    const QColor buffer_color_ = QRgb(0xAAAAAA);
    const QColor groove_color_ = QRgb(0xDDDDDD);
    const int groove_height_ = 12;
    const int groove_radius_ = 0;

    const QColor handle_color = QRgb(0x333333);
    const QColor handle_hover_color = QRgb(0x666666);
    const QColor handle_pressed_color = QRgb(0x111111);
    const int handle_height = 20;
    const int handle_width = 12;

    int buffer_value_ = 0;
};
