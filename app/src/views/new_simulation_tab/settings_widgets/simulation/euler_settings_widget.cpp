#include "euler_settings_panel.h"

#include <QObject>
#include <string>

#include "./ui_euler_settings_widget.h"

namespace {
std::string EulerStepId = "EulerStep";
std::string EulerSofteningId = "EulerSoftening";
}  // namespace

EulerSettingsWidget::EulerSettingsWidget(QWidget* parent)
    : QWidget(parent), ui_(new Ui::EulerSettingsWidget) {
    ui_->setupUi(this);

    constexpr auto S = Setting::Group::Simulation;
    constexpr auto D = Setting::Type::Double;
    default_settings_ = Settings{{EulerStepId, {S, D, 0.01}}, {EulerSofteningId, {S, D, 0.001}}};

    resetSettings();
}

Settings EulerSettingsWidget::getSettings() const {
    constexpr auto S = Setting::Group::Simulation;
    constexpr auto D = Setting::Type::Double;
    return Settings{{EulerStepId, {S, D, ui_->dsbEulerStep->value()}},
                    {EulerSofteningId, {S, D, ui_->dsbEulerSoftening->value()}}};
}

void EulerSettingsWidget::setSettings(const Settings& settings) {
    if (settings.has(EulerStepId)) {
        ui_->dsbEulerStep->setValue(settings.get<double>(EulerStepId));
    }
    if (settings.has(EulerSofteningId)) {
        ui_->dsbEulerSoftening->setValue(settings.get<double>(EulerSofteningId));
    }
}

void EulerSettingsWidget::setDefaultSettings(const Settings& settings) {
    default_settings_ = settings;
}

void EulerSettingsWidget::resetSettings() { setSettings(default_settings_); }
