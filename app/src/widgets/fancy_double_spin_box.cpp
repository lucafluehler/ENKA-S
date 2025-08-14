#include "fancy_double_spin_box.h"

#include <QRegularExpression>

FancyDoubleSpinBox::FancyDoubleSpinBox(QWidget* parent)
    : QDoubleSpinBox(parent),
      trailingZeros("0+$"),
      trailingDot("\\.$"),
      thousandsSeperators("(?<=\\d)(?=(\\d{3})+(?!\\d|,|$))") {
    setDecimals(15);

    setMinimum(0.0);
    setMaximum(1.0e12);
}

QString FancyDoubleSpinBox::textFromValue(double value) const {
    QString text = QString::number(value, 'f', decimals());
    text.remove(trailingZeros);
    text.remove(trailingDot);
    return text;
}
