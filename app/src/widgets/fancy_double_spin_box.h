#pragma once

#include <QDoubleSpinBox>

class FancyDoubleSpinBox : public QDoubleSpinBox
{
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
