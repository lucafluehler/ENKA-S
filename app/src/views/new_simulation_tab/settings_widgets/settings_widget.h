#pragma once

#include <QFormLayout>
#include <QLayoutItem>
#include <QMap>
#include <QObject>
#include <QVector>
#include <QWidget>

#include "core/settings/settings.h"
#include "setting_descriptor.h"

/**
 * @brief SettingsWidget provides a dynamic form for editing settings based on a schema.
 *
 * It allows users to set various types of settings, including doubles, integers, file paths,
 * and random integers, with appropriate input widgets.
 */
class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a SettingsWidget with an empty form layout.
     * @param parent The parent widget.
     */
    explicit SettingsWidget(QWidget* parent = nullptr) { form_ = new QFormLayout(this); }
    ~SettingsWidget() override = default;

    /**
     * @brief Sets the schema for the settings widget.
     * @param schema The schema to set.
     */
    void setSchema(const QVector<SettingDescriptor>& schema);

    /**
     * @brief Gets the current settings from the widget.
     * @return The current settings.
     */
    Settings getSettings() const;

    /**
     * @brief Sets the settings in the widget.
     * @param s The settings to set.
     */
    void setSettings(const Settings& s);

signals:
    /**
     * @brief Emitted when a setting is changed.
     * @param key The key of the setting that was changed.
     * @param value The new value of the setting.
     */
    void settingChanged(SettingKey key, const QVariant& value);

private:
    void clearLayout(QFormLayout* layout) {
        while (layout->rowCount() > 0) layout->removeRow(0);
    }

    QFormLayout* form_;
    QMap<SettingKey, QWidget*> editors_;
    QMap<SettingKey, SettingDescriptor::Type> types_;
};
