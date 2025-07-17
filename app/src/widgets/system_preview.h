#pragma once

#include <enkas/generation/generation_method.h>
#include <enkas/generation/generation_settings.h>

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include "rendering/particle_renderer.h"

class SystemPreview : public ParticleRenderer {
    Q_OBJECT

public:
    explicit SystemPreview(QWidget* parent = nullptr);

    /**
     * @brief Initializes the preview with a procedurally generated system based on the given
     * method.
     * @param method The generation method to use.
     */
    void initializeProcedural(enkas::generation::Method method);

    /**
     * @brief Initializes the preview with a system loaded from a file.
     * @param system_path The path to the system file.
     */
    void initializeFromFile(const QString& system_path);

    /**
     * @brief Initializes the preview for the home screen with a default system.
     */
    void initializeHomeScreen();

private:
    void setMethodSettings();
    void setHomeScreenSettings();

    enkas::generation::UniformCubeSettings getUniformCubeSettings();
    enkas::generation::NormalSphereSettings getNormalSphereSettings();
    enkas::generation::UniformSphereSettings getUniformSphereSettings();
    enkas::generation::PlummerSphereSettings getPlummerSphereSettings();
    enkas::generation::SpiralGalaxySettings getSpiralGalaxySettings();
    enkas::generation::CollisionModelSettings getCollisionModelSettings();
};
