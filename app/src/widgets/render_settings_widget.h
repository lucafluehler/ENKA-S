#pragma once

#include <QMap>
#include <QWidget>

#include "rendering/render_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RenderSettingsWidget;
}
QT_END_NAMESPACE

/**
 * @brief A widget for adjusting render settings.
 */
class RenderSettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit RenderSettingsWidget(QWidget *parent = nullptr);
    ~RenderSettingsWidget() override = default;

    void loadRenderSettings(const RenderSettings &settings);
    RenderSettings getRenderSettings() const;

signals:
    void settingsChanged();
    void fpsChanged(int fps);

private slots:
    void disableAnimationSpeed(int idx);

private:
    void addAnimationStyle(QString str, AnimationStyle style);
    void addColoringMethod(QString str, ColoringMethod method);

    QMap<QString, AnimationStyle> animation_styles;
    QMap<QString, ColoringMethod> coloring_methods;

    Ui::RenderSettingsWidget *ui;
};
