#include "widgets/debug_info_widget.h"

#include <QFormLayout>
#include <QLabel>
#include <QWidget>

DebugInfoWidget::DebugInfoWidget(QWidget* parent)
    : QWidget(parent), layout_(new QFormLayout(this)) {
    layout_->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout_->setFormAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout_->setContentsMargins(20, 10, 20, 10);
    layout_->setHorizontalSpacing(10);
    setLayout(layout_);
}

void DebugInfoWidget::clearInfo() {
    while (layout_->count() > 0) {
        QLayoutItem* item = layout_->takeAt(0);
        delete item->widget();
        delete item;
    }
    ui_rows_.reset();
}
