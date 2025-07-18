#include "euler_settings_widget.h"

#include <QObject>

#include "core/settings/settings.h"
#include "forms/new_simulation_tab/simulation_settings_widgets/ui_euler_settings_widget.h"

EulerSettingsWidget::EulerSettingsWidget(QWidget* parent)
    : QWidget(parent), ui_(new Ui::EulerSettingsWidget) {
    ui_->setupUi(this);

    default_settings_ =
        Settings::create({{SettingKey::EulerTimeStep, 0.01}, {SettingKey::EulerSoftening, 0.001}})
            .value();

    resetSettings();
}

Settings EulerSettingsWidget::getSettings() const {
    return Settings::create({{SettingKey::EulerTimeStep, ui_->dsbEulerStep->value()},
                             {SettingKey::EulerSoftening, ui_->dsbEulerSoft->value()}})
        .value();
}

void EulerSettingsWidget::setSettings(const Settings& settings) {
    if (settings.has(SettingKey::EulerTimeStep)) {
        ui_->dsbEulerStep->setValue(settings.get<double>(SettingKey::EulerTimeStep));
    }
    if (settings.has(SettingKey::EulerSoftening)) {
        ui_->dsbEulerSoft->setValue(settings.get<double>(SettingKey::EulerSoftening));
    }
}

void EulerSettingsWidget::setDefaultSettings(const Settings& settings) {
    default_settings_ = settings;
}

void EulerSettingsWidget::resetSettings() { setSettings(default_settings_); }
