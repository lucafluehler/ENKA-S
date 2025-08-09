#include "playback_bar.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionSlider>

#include "rendering/is_darkmode.h"

PlaybackBar::PlaybackBar(QWidget* parent) : QSlider(parent) {
    if (isDarkMode()) {
        progress_color_ = QColor(0x6CACDE);
        buffer_color_ = QColor(0x555555);
        groove_color_ = QColor(0x3F3F3F);

        handle_color = QColor(0xD0D0D0);
        handle_hover_color = QColor(0xFFFFFF);
        handle_pressed_color = QColor(0xB0B0B0);
    } else {
        progress_color_ = QRgb(0x77A8D6);
        buffer_color_ = QRgb(0xAAAAAA);
        groove_color_ = QRgb(0xDDDDDD);

        handle_color = QRgb(0x333333);
        handle_hover_color = QRgb(0x666666);
        handle_pressed_color = QRgb(0x111111);
    }
};

QSize PlaybackBar::sizeHint() const {
    QSize base_size = QSlider::sizeHint();

    if (orientation() == Qt::Horizontal) {
        // The hint's height should be the larger of the groove or the handle height.
        base_size.setHeight(std::max(groove_height_, handle_height));
    } else {
        base_size.setWidth(std::max(groove_height_, handle_height));
    }

    return base_size;
}

void PlaybackBar::paintEvent(QPaintEvent* ev) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    if (orientation() != Qt::Horizontal) {
        QSlider::paintEvent(ev);  // Fallback for vertical orientation
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QRect groove_rect =
        style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    groove_rect.setTop(height() / 2 - groove_height_ / 2);
    groove_rect.setHeight(groove_height_);

    painter.setPen(Qt::NoPen);
    painter.setBrush(groove_color_);
    painter.drawRect(groove_rect);

    auto valueToPixel = [&](int val) {
        const int slider_span = maximum() - minimum();
        if (slider_span == 0) return groove_rect.x();
        return groove_rect.x() +
               static_cast<int>((static_cast<double>(val - minimum()) / slider_span) *
                                groove_rect.width());
    };

    if (buffer_value_ > 0) {
        const int buffer_start_pos = valueToPixel(value());

        const int buffer_end_value = std::min(value() + buffer_value_, maximum());
        const int buffer_end_pos = valueToPixel(buffer_end_value);

        QRect buffer_rect(buffer_start_pos,
                          groove_rect.y(),
                          buffer_end_pos - buffer_start_pos,
                          groove_rect.height());

        painter.setBrush(buffer_color_);
        painter.drawRect(buffer_rect.intersected(groove_rect));
    }

    if (value() > minimum()) {
        const int progress_end_pos = valueToPixel(value());
        const int progress_width = progress_end_pos - groove_rect.x() + handle_width / 2;

        QRect progress_rect(groove_rect.x(), groove_rect.y(), progress_width, groove_rect.height());

        painter.setBrush(progress_color_);
        painter.drawRect(progress_rect.intersected(groove_rect));
    }

    const QRect styleHandleRect =
        style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    QRect handleRect(0, 0, handle_width, handle_height);
    handleRect.moveCenter(styleHandleRect.center());

    QColor handleColor = handle_color;
    if (opt.state & QStyle::State_Sunken) {  // Pressed state
        handleColor = handle_pressed_color;
    } else if (opt.state & QStyle::State_MouseOver) {  // Hover state
        handleColor = handle_hover_color;
    }

    painter.setBrush(handleColor);
    painter.drawRect(handleRect);
}
