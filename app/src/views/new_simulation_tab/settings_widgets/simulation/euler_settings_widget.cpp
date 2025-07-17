#include "euler_settings_widget.h"

#include <QObject>
#include <string_view>

#include "forms/new_simulation_tab/simulation_settings_widgets/ui_euler_settings_widget.h"

namespace {
constexpr std::string_view EulerStepId = "EulerStep";
constexpr std::string_view EulerSofteningId = "EulerSoftening";
}  // namespace

EulerSettingsWidget::EulerSettingsWidget(QWidget* parent)
    : QWidget(parent), ui_(new Ui::EulerSettingsWidget) {
    ui_->setupUi(this);

    default_settings_ = Settings::create({{EulerStepId, 0.01}, {EulerSofteningId, 0.001}}).value();

    resetSettings();
}

Settings EulerSettingsWidget::getSettings() const {
    return Settings::create({{EulerStepId, ui_->dsbEulerStep->value()},
                             {EulerSofteningId, ui_->dsbEulerSoftening->value()}})
        .value();
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
