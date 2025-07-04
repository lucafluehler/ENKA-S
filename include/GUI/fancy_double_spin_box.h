#ifndef FANCY_DOUBLE_SPIN_BOX_H_
#define FANCY_DOUBLE_SPIN_BOX_H_

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

#endif // FANCY_DOUBLE_SPIN_BOX_H_
