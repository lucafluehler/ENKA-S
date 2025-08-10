#pragma once

#include <QDoubleSpinBox>

/**
 * @brief A double spin box with enhanced formatting.
 *
 * Trailing zeros are removed.
 */
class FancyDoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

public:
    FancyDoubleSpinBox(QWidget* parent = nullptr);

protected:
    QString textFromValue(double value) const override;

private:
    QRegularExpression trailingZeros;
    QRegularExpression trailingDot;
    QRegularExpression thousandsSeperators;
};
