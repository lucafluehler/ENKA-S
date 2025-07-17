#pragma once

#include <QWidget>
#include <QMap>

#include "render_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RenderSettingsWidget; }
QT_END_NAMESPACE

class RenderSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    RenderSettingsWidget(QWidget *parent = nullptr);
    ~RenderSettingsWidget();

    void loadRenderSettings(const RenderSettings& settings);
    RenderSettings getRenderSettings() const;

signals:
    void settingsChanged();

private slots:
    void disableAnimationSpeed(int idx);

private:
    void addAnimationStyle(QString str, AnimationStyle style);
    void addColoringMethod(QString str, ColoringMethod method);

    QMap<QString, AnimationStyle> animation_styles;
    QMap<QString, ColoringMethod> coloring_methods;

    Ui::RenderSettingsWidget *ui;
};
