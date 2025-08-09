#include "render_settings_widget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>

#include "forms/simulation_window/ui_render_settings_widget.h"
#include "rendering/render_settings.h"

RenderSettingsWidget::RenderSettingsWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::RenderSettingsWidget) {
    ui->setupUi(this);

    // Map Animation Styles
    addAnimationStyle("None", AnimationStyle::None);
    addAnimationStyle("Right", AnimationStyle::Right);
    addAnimationStyle("Left", AnimationStyle::Left);
    addAnimationStyle("Up", AnimationStyle::Up);
    addAnimationStyle("Down", AnimationStyle::Down);
    addAnimationStyle("Clockwise", AnimationStyle::Clockwise);
    addAnimationStyle("Counterclockwise", AnimationStyle::Counterclockwise);
    addAnimationStyle("Tutti", AnimationStyle::Tutti);

    // Map Coloring Methods
    addColoringMethod("Black Fog", ColoringMethod::BlackFog);
    addColoringMethod("White Fog", ColoringMethod::WhiteFog);

    // Load defaults
    loadRenderSettings(RenderSettings());

    // Signal Management
    connect(ui->cbxAnimation,
            &QComboBox::activated,
            this,
            &RenderSettingsWidget::disableAnimationSpeed);
    connect(ui->cbxColoringMethod,
            &QComboBox::activated,
            ui->stwColoringSettings,
            &QStackedWidget::setCurrentIndex);

    // Settings changed
    connect(ui->chbShowCOM,
            &QCheckBox::checkStateChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->chbShowCenter,
            &QCheckBox::checkStateChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->cbxAnimation,
            &QComboBox::currentIndexChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->dsbAnimationSpeed,
            &QDoubleSpinBox::valueChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->cbxColoringMethod,
            &QComboBox::currentIndexChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->dsbBlackFogParam,
            &QDoubleSpinBox::valueChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->dsbWhiteFogParam,
            &QDoubleSpinBox::valueChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
    connect(ui->sbxFOV, &QSpinBox::valueChanged, this, &RenderSettingsWidget::settingsChanged);
    connect(ui->sbxFPS, &QSpinBox::valueChanged, this, &RenderSettingsWidget::fpsChanged);
    connect(ui->dsbParticleSize,
            &QDoubleSpinBox::valueChanged,
            this,
            &RenderSettingsWidget::settingsChanged);
}

void RenderSettingsWidget::disableAnimationSpeed(int idx) {
    ui->dsbAnimationSpeed->setEnabled(idx != 0);
}

void RenderSettingsWidget::loadRenderSettings(const RenderSettings& settings) {
    if (settings.show_center_of_mass) {
        ui->chbShowCOM->setCheckState(Qt::Checked);
    } else {
        ui->chbShowCOM->setCheckState(Qt::Unchecked);
    }

    if (settings.show_center_of_screen) {
        ui->chbShowCenter->setCheckState(Qt::Checked);
    } else {
        ui->chbShowCenter->setCheckState(Qt::Unchecked);
    }

    QString as = animation_styles.key(settings.animation_style);
    ui->cbxAnimation->setCurrentText(as);

    ui->dsbAnimationSpeed->setValue(settings.animation_speed);

    QString cm = coloring_methods.key(settings.coloring_method);
    ui->cbxColoringMethod->setCurrentText(cm);

    ui->stwColoringSettings->setCurrentIndex(ui->cbxColoringMethod->currentIndex());
    ui->dsbBlackFogParam->setValue(settings.black_fog_param);
    ui->dsbWhiteFogParam->setValue(settings.white_fog_param);

    ui->sbxFOV->setValue(settings.fov);
    ui->dsbParticleSize->setValue(settings.particle_size_param);
}

RenderSettings RenderSettingsWidget::getRenderSettings() const {
    RenderSettings settings;

    settings.show_center_of_mass = ui->chbShowCOM->checkState() == Qt::Checked;
    settings.show_center_of_screen = ui->chbShowCenter->checkState() == Qt::Checked;

    QString as = ui->cbxAnimation->currentText();
    settings.animation_style = animation_styles.value(as);

    settings.animation_speed = ui->dsbAnimationSpeed->value();

    QString cm = ui->cbxColoringMethod->currentText();
    settings.coloring_method = coloring_methods.value(cm);

    settings.black_fog_param = ui->dsbBlackFogParam->value();
    settings.white_fog_param = ui->dsbWhiteFogParam->value();

    settings.fov = ui->sbxFOV->value();
    settings.particle_size_param = ui->dsbParticleSize->value();

    return settings;
}

void RenderSettingsWidget::addAnimationStyle(QString str, AnimationStyle style) {
    ui->cbxAnimation->addItem(str);
    animation_styles[str] = style;
}

void RenderSettingsWidget::addColoringMethod(QString str, ColoringMethod method) {
    ui->cbxColoringMethod->addItem(str);
    coloring_methods[str] = method;
}
