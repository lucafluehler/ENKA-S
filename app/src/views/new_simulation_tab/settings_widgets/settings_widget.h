#pragma once

#include <QFormLayout>
#include <QLayoutItem>
#include <QMap>
#include <QObject>
#include <QVector>
#include <QWidget>

#include "core/settings/settings.h"
#include "setting_descriptor.h"

class SettingsWidget : public QWidget {
    Q_OBJECT

    QFormLayout* form_;
    QMap<SettingKey, QWidget*> editors_;

public:
    SettingsWidget(QWidget* parent = nullptr) { form_ = new QFormLayout(this); }

    void setSchema(const QVector<SettingDescriptor>& schema);

    Settings getSettings() const;

    void setSettings(const Settings& s);

signals:
    void settingChanged(SettingKey key, const QVariant& value);

private:
    void clearLayout(QFormLayout* layout);

    QMap<SettingKey, SettingDescriptor::Type> types_;
};
