#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QObject>

#include <memory>

#include "generation_settings.h"
#include "particle_renderer.h"

class SystemPreview : public ParticleRenderer
{
    Q_OBJECT

public:
    SystemPreview(QWidget* parent = nullptr);

    void initializeMethod( GenerationMethod method
                         , const QString& initial_system_path = "" );
    void initializeHomeScreen();

private:
    void setMethodSettings();
    void setHomeScreenSettings();
};
