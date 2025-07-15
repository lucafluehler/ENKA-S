#pragma once

#include <QObject>
#include <QWidget>

#include "core/settings.h"
#include "views/new_simulation_tab/settings_widgets/settings_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class EulerSettingsWidget;
}
QT_END_NAMESPACE

class EulerSettingsWidget : public QWidget, public SettingsWidget {
    Q_OBJECT

public:
    explicit EulerSettingsWidget(QWidget* parent = nullptr);

    Settings getSettings() const override;
    void setSettings(const Settings& settings) override;
    void setDefaultSettings(const Settings& settings) override;
    void resetSettings() override;

private:
    Ui::EulerSettingsWidget* ui_;
    Settings default_settings_;
};
