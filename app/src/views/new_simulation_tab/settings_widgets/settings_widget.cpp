#include "settings_widget.h"

#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLayoutItem>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QVector>
#include <QWidget>
#include <string>

#include "core/settings/settings.h"
#include "setting_descriptor.h"
#include "widgets/fancy_double_spin_box.h"

void SettingsWidget::setSchema(const QVector<SettingDescriptor>& schema) {
    qDeleteAll(editors_);
    editors_.clear();
    clearLayout(form_);

    for (auto&& desc : schema) {
        QWidget* editor = nullptr;
        types_[desc.key] = desc.type;
        switch (desc.type) {
            case SettingDescriptor::Double: {
                auto* sb = new FancyDoubleSpinBox;
                sb->setRange(desc.min.toDouble(), desc.max.toDouble());
                sb->setValue(desc.defaultValue.toDouble());
                editor = sb;
            } break;

            case SettingDescriptor::Int: {
                auto* sb = new QSpinBox;
                sb->setRange(desc.min.toInt(), desc.max.toInt());
                sb->setValue(desc.defaultValue.toInt());
                editor = sb;
            } break;

            case SettingDescriptor::FilePath: {
                auto* hh = new QWidget;
                auto* lay = new QHBoxLayout(hh);
                auto* le = new QLineEdit;
                connect(le, &QLineEdit::editingFinished, this, [=, this]() {
                    emit settingChanged(desc.key, le->text());
                });
                auto* btn = new QPushButton("…");
                lay->addWidget(le);
                lay->addWidget(btn);
                connect(btn, &QPushButton::clicked, this, [this, le, desc]() {
                    QString f = QFileDialog::getOpenFileName(le, "Select file");
                    if (!f.isEmpty()) le->setText(f);
                    emit settingChanged(desc.key, f);
                });
                editor = hh;
            } break;

            case SettingDescriptor::RandomInt: {
                auto* container = new QWidget;
                auto* layout = new QHBoxLayout(container);
                layout->setContentsMargins(0, 0, 0, 0);
                layout->setSpacing(4);

                auto* spinBox = new QSpinBox;
                spinBox->setRange(desc.min.toInt(), desc.max.toInt());

                // Randomize on creation
                int randomized =
                    QRandomGenerator::global()->bounded(desc.min.toInt(), desc.max.toInt() + 1);
                spinBox->setValue(randomized);

                auto* button = new QPushButton("Randomize");
                layout->addWidget(spinBox);
                layout->addWidget(button);

                connect(button, &QPushButton::clicked, this, [=]() {
                    int val = QRandomGenerator::global()->bounded(spinBox->minimum(),
                                                                  spinBox->maximum() + 1);
                    spinBox->setValue(val);
                });

                editor = container;
            } break;
        }
        editors_[desc.key] = editor;
        form_->addRow(desc.label, editor);
    }
}

Settings SettingsWidget::getSettings() const {
    Settings out;
    for (auto key : editors_.keys()) {
        QWidget* w = editors_[key];
        auto type = types_[key];
        switch (type) {
            case SettingDescriptor::Double: {
                auto* d = qobject_cast<QDoubleSpinBox*>(w);
                if (d) out.set(key, d->value());
                break;
            }
            case SettingDescriptor::Int: {
                auto* i = qobject_cast<QSpinBox*>(w);
                if (i) out.set(key, i->value());
                break;
            }
            case SettingDescriptor::FilePath: {
                if (auto* container = qobject_cast<QWidget*>(w)) {
                    auto* le = container->findChild<QLineEdit*>();
                    if (le) out.set(key, le->text().toStdString());
                }
                break;
            }
            case SettingDescriptor::RandomInt: {
                if (auto* container = qobject_cast<QWidget*>(w)) {
                    auto* sb = container->findChild<QSpinBox*>();
                    if (sb) out.set(key, sb->value());
                }
                break;
            }
        }
    }
    return out;
}

void SettingsWidget::setSettings(const Settings& settings) {
    for (auto key : editors_.keys()) {
        if (!settings.has(key)) continue;
        QWidget* w = editors_[key];
        auto type = types_[key];
        switch (type) {
            case SettingDescriptor::Double: {
                auto* d = qobject_cast<QDoubleSpinBox*>(w);
                if (d) d->setValue(settings.get<double>(key));
                break;
            }
            case SettingDescriptor::Int: {
                auto* i = qobject_cast<QSpinBox*>(w);
                if (i) i->setValue(settings.get<int>(key));
                break;
            }
            case SettingDescriptor::FilePath: {
                if (auto* container = qobject_cast<QWidget*>(w)) {
                    auto* le = container->findChild<QLineEdit*>();
                    if (le) le->setText(QString::fromStdString(settings.get<std::string>(key)));
                }
                break;
            }
            case SettingDescriptor::RandomInt: {
                if (auto* container = qobject_cast<QWidget*>(w)) {
                    auto* sb = container->findChild<QSpinBox*>();
                    if (sb) sb->setValue(settings.get<int>(key));
                }
                break;
            }
        }
    }
}
