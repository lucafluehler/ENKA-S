#include "fancy_double_spin_box.h"

#include <QRegularExpression>

FancyDoubleSpinBox::FancyDoubleSpinBox(QWidget* parent)
    : QDoubleSpinBox(parent), trailing_zeros("0+$"), trailing_dot("\\.$") {
    setDecimals(15);
    setMinimum(0.0);
    setMaximum(1.0e12);
}

QString FancyDoubleSpinBox::textFromValue(double value) const {
    QString text = QString::number(value, 'f', decimals());
    text.remove(trailing_zeros);
    text.remove(trailing_dot);
    return text;
}
