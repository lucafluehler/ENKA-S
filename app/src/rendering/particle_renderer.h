#pragma once

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>

#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include "core/dataflow/snapshot.h"
#include "rendering/camera.h"
#include "rendering/render_settings.h"

/**
 * @brief ParticleRenderer is a QOpenGLWidget that renders particles in a 3D space.
 * It supports mouse interactions, camera movements, and rendering settings.
 */
class ParticleRenderer : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    /**
     * @brief Initializes OpenGL settings.
     * @param parent The parent widget.
     */
    explicit ParticleRenderer(QWidget* parent = nullptr);
    ~ParticleRenderer() override = default;

    /**
     * @brief Updates the particle system data.
     * @param system The new particle system snapshot.
     * @note This does not trigger a redraw.
     */
    void updateData(SystemSnapshotPtr system);

    /**
     * @brief Updates the center of mass position.
     * @param com_position The new center of mass position in world coordinates.
     */
    void updateCenterOfMass(const enkas::math::Vector3D& com_position) {
        com_position_ = com_position;
    }

    /**
     * @brief Redraws the widget with the given settings.
     * @param settings The rendering settings to apply.
     */
    void redraw(const RenderSettings& settings);

    /**
     * @brief Clears the current particle data.
     */
    void clearData();

public slots:
    /**
     * @brief Saves a screenshot of the current rendering.
     */
    void saveScreenshot();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void initializeParticleShader();
    void initializeCrossShader();
    void drawParticles();
    void drawCross(const QPointF& center, float size, const QVector3D& color);
    QPointF projectWorldToNdc(const enkas::math::Vector3D& world_pos, bool* is_visible);
    void animation();
    void setBackgroundColor();

    QPoint last_mouse_pos_;

    RenderSettings settings_;
    SystemSnapshotPtr system_;
    enkas::math::Vector3D com_position_;  // Center of mass position in world coordinates
    Camera camera_;

    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer particle_position_vbo_;  // Holds all particle 3D positions
    QOpenGLBuffer quad_vbo_;               // Holds the 4 vertices of a simple square
    QOpenGLShaderProgram shader_program_;

    QOpenGLShaderProgram cross_shader_program_;
    QOpenGLVertexArrayObject cross_vao_;
    QOpenGLBuffer cross_vbo_;
};
