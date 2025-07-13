#pragma once

#include <enkas/generation/generation_method.h>

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include "../rendering/particle_renderer.h"

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
    /**
     * @brief Sets the render settings for the procedural and file-based systems.
     */
    void setMethodSettings();

    /**
     * @brief Sets the render settings for the home screen system.
     */
    void setHomeScreenSettings();
};
