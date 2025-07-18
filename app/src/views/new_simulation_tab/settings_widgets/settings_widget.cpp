#include "settings_widget.h"

#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLayoutItem>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
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
                auto* btn = new QPushButton("…");
                lay->addWidget(le);
                lay->addWidget(btn);
                connect(btn, &QPushButton::clicked, this, [le]() {
                    QString f = QFileDialog::getOpenFileName(le, "Select file");
                    if (!f.isEmpty()) le->setText(f);
                });
                editor = hh;
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
        if (auto* d = qobject_cast<QDoubleSpinBox*>(w))
            out.set(key, d->value());
        else if (auto* i = qobject_cast<QSpinBox*>(w))
            out.set(key, i->value());
        else if (auto* h = qobject_cast<QWidget*>(w)) {
            auto* le = h->findChild<QLineEdit*>();
            out.set(key, le->text().toStdString());
        }
    }
    return out;
}

void SettingsWidget::setSettings(const Settings& settings) {
    for (auto key : editors_.keys()) {
        if (!settings.has(key)) continue;
        QWidget* w = editors_[key];
        if (auto* d = qobject_cast<QDoubleSpinBox*>(w))
            d->setValue(settings.get<double>(key));
        else if (auto* i = qobject_cast<QSpinBox*>(w))
            i->setValue(settings.get<int>(key));
        else if (auto* h = qobject_cast<QWidget*>(w)) {
            auto* le = h->findChild<QLineEdit*>();
            le->setText(QString::fromStdString(settings.get<std::string>(key)));
        }
    }
}

void SettingsWidget::clearLayout(QFormLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}
