#pragma once

#include <QtWidgets>
#include <any>
#include <atomic>
#include <string_view>
#include <vector>

/**
 * @brief A progress bar that changes color based on its value.
 */
class ColoredProgressBar : public QProgressBar {
public:
    explicit ColoredProgressBar(QWidget* parent = nullptr)
        : QProgressBar(parent), more_is_better_(false) {}

    void setMoreIsBetter(bool b) {
        more_is_better_ = b;
        update();
    }

protected:
    void paintEvent(QPaintEvent* ev) override {
        QStyleOptionProgressBar opt;
        initStyleOption(&opt);
        QPainter p(this);

        style()->drawControl(QStyle::CE_ProgressBarGroove, &opt, &p, this);

        const double width_frac =
            (maximum() > minimum()) ? double(value() - minimum()) / (maximum() - minimum()) : 0.0;

        const double color_frac = more_is_better_ ? (1.0 - width_frac) : width_frac;

        const QColor c0(0, 150, 0);    // deep green
        const QColor c1(200, 200, 0);  // yellow
        const QColor c2(150, 0, 0);    // deep red

        QColor color;
        if (color_frac <= 0.5) {
            double t = color_frac / 0.5;
            color.setRgbF(c0.redF() + (c1.redF() - c0.redF()) * t,
                          c0.greenF() + (c1.greenF() - c0.greenF()) * t,
                          c0.blueF() + (c1.blueF() - c0.blueF()) * t);
        } else {
            double t = (color_frac - 0.5) / 0.5;
            color.setRgbF(c1.redF() + (c2.redF() - c1.redF()) * t,
                          c1.greenF() + (c2.greenF() - c1.greenF()) * t,
                          c1.blueF() + (c2.blueF() - c1.blueF()) * t);
        }

        QRect barRect = style()->subElementRect(QStyle::SE_ProgressBarContents, &opt, this);

        const int fill_width = int(barRect.width() * width_frac);

        QRect fillRect = QRect(barRect.x(), barRect.y(), fill_width, barRect.height());

        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawRect(fillRect);
    }

private:
    bool more_is_better_;
};

/**
 * @brief A widget that displays a value bar with current and maximum values.
 *
 * This widget is used to visualize the size and capacity of a data structure,
 * such as queues or buffers, in a compact form.
 */
class ValueBarWidget : public QWidget {
public:
    explicit ValueBarWidget(bool moreIsBetter, QWidget* parent = nullptr)
        : QWidget(parent), more_is_better_(moreIsBetter) {
        auto* stack = new QStackedLayout(this);
        stack->setContentsMargins(0, 0, 0, 0);

        // Active row
        auto* active = new QWidget(this);
        auto* hbox = new QHBoxLayout(active);
        hbox->setContentsMargins(5, 0, 5, 0);
        hbox->setSpacing(8);

        current_label_ = new QLabel("0", this);
        current_label_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        current_label_->setFixedWidth(40);

        progress_bar_ = new ColoredProgressBar(this);
        progress_bar_->setTextVisible(false);
        progress_bar_->setFixedHeight(6);
        progress_bar_->setMoreIsBetter(more_is_better_);

        max_label_ = new QLabel("0", this);
        max_label_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        max_label_->setFixedWidth(40);

        hbox->addWidget(current_label_);
        hbox->addWidget(progress_bar_, 1);
        hbox->addWidget(max_label_);

        not_used_label_ = new QLabel("Not Used", this);
        not_used_label_->setAlignment(Qt::AlignCenter);
        not_used_label_->setStyleSheet("color:#888;font-style:italic;");

        stack->addWidget(active);
        stack->addWidget(not_used_label_);

        setLayout(stack);
    }

    void updateValues(size_t size, size_t cap) {
        auto* stack = static_cast<QStackedLayout*>(layout());
        stack->setCurrentIndex(0);
        current_label_->setText(QString::number(size));
        max_label_->setText(QString::number(cap));
        progress_bar_->setRange(0, cap > 0 ? int(cap) : 1);
        progress_bar_->setValue(int(size));
    }

    void setNotUsed() {
        auto* stack = static_cast<QStackedLayout*>(layout());
        stack->setCurrentIndex(1);
    }

private:
    bool more_is_better_;
    QLabel* current_label_;
    QLabel* max_label_;
    QLabel* not_used_label_;
    ColoredProgressBar* progress_bar_;
};

template <typename T>
struct DebugInfoRow {
    using DebugStruct = T;
    std::string_view name;
    std::atomic<size_t> T::* size_member;
    size_t T::* capacity_member;
    bool more_is_better = false;  // If true, higher values are better
};

/**
 * @brief A widget that displays debug information for multiple data structures.
 */
class DebugInfoWidget : public QWidget {
    Q_OBJECT

public:
    explicit DebugInfoWidget(QWidget* parent = nullptr);

    void clearInfo();

    template <typename T>
    void setupInfo(std::span<const DebugInfoRow<T>> rows) {
        clearInfo();

        std::vector<UiRow<T>> concrete_ui_rows;
        concrete_ui_rows.reserve(rows.size());

        for (const auto& row : rows) {
            // Name label
            auto* nameLabel = new QLabel(QString::fromStdString(std::string(row.name)), this);
            QFont font = nameLabel->font();
            font.setBold(true);
            nameLabel->setFont(font);
            nameLabel->setFixedHeight(nameLabel->sizeHint().height());
            nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            // Value bar with custom colored progress
            auto* valueBar = new ValueBarWidget(row.more_is_better, this);
            valueBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            layout_->addRow(nameLabel, valueBar);

            concrete_ui_rows.push_back(UiRow<T>{.value_bar = valueBar,
                                                .size_member = row.size_member,
                                                .capacity_member = row.capacity_member});
        }

        ui_rows_.emplace<std::vector<UiRow<T>>>(std::move(concrete_ui_rows));
    }

    template <typename T>
    void updateInfo(const T& info) {
        if (!ui_rows_.has_value() || ui_rows_.type() != typeid(std::vector<UiRow<T>>)) return;

        const auto& rows = std::any_cast<const std::vector<UiRow<T>>&>(ui_rows_);
        for (const auto& r : rows) {
            size_t size = (info.*(r.size_member)).load(std::memory_order_relaxed);
            size_t cap = info.*(r.capacity_member);

            if (size == QUEUE_NOT_PRESENT) {
                r.value_bar->setNotUsed();
            } else {
                r.value_bar->updateValues(size, cap);
            }
        }
    }

private:
    // A single row's UI data
    template <typename T>
    struct UiRow {
        class ValueBarWidget* value_bar;
        std::atomic<size_t> T::* size_member;
        size_t T::* capacity_member;
    };

    std::any ui_rows_;
    QFormLayout* layout_;
    static constexpr size_t QUEUE_NOT_PRESENT = std::numeric_limits<size_t>::max();
};
