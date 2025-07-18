#pragma once

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>

#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <tuple>
#include <vector>

#include "core/snapshot.h"
#include "rendering/camera.h"
#include "rendering/render_settings.h"

class ParticleRenderer : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    explicit ParticleRenderer(QWidget* parent = nullptr);

    void updateData(SystemSnapshotPtr system);
    void redraw(const RenderSettings& settings);

public slots:
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
    void drawParticles();
    void drawCircle(std::vector<GLfloat>& vertices,
                    const int c_NUM_TRANGLES,
                    const float c_X_ASPECT,
                    const float c_Y_ASPECT,
                    float x,
                    float y,
                    float radius);
    void drawCross(float x, float y, float size);
    enkas::math::Vector3D getRelPos(const enkas::math::Vector3D& pos);
    QPointF convertPosToLoc(const float c_ASPECT_RATIO,
                            const float c_HALF_TAN_FOV,
                            const enkas::math::Vector3D& rel_pos,
                            bool* is_visible);
    void animation();

    void setBackgroundColor();
    void setCOMColor();
    void setCenterColor();
    void setParticleColor(double distance);

    QPoint last_mouse_pos_;

    std::vector<std::tuple<double, enkas::math::Vector3D>> rel_positions_;

    RenderSettings settings_;
    SystemSnapshotPtr system_;
    Camera camera_;

    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer particle_position_vbo_;  // Holds all particle 3D positions
    QOpenGLBuffer quad_vbo_;               // Holds the 4 vertices of a simple square
    QOpenGLShaderProgram shader_program_;
};
