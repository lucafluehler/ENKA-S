#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>

#include <memory>

#include "render_settings.h"
#include "render_data.h"
#include "camera.h"
#include "geometric_algebra.h"

class ParticleRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ParticleRenderer(QWidget* parent = nullptr);

    void updateData(const std::shared_ptr<RenderData>& data);
    void updateData(const std::unique_ptr<RenderData>& data);
    void redraw(const RenderSettings& p_settings);

public slots:
    void saveScreenshot();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void drawParticles();
    void drawCircle( std::vector<GLfloat>& vertices
                   , const int c_NUM_TRANGLES
                   , const float c_X_ASPECT, const float c_Y_ASPECT
                   , float x, float y, float radius );
    void drawCross(float x, float y, float size);
    ga::Vector3D getRelPos(const ga::Vector3D& pos);
    QPointF convertPosToLoc( const float c_ASPECT_RATIO, const float c_HALF_TAN_FOV
                           , const ga::Vector3D& rel_pos, bool* is_visible);
    void animation();

    void setBackgroundColor();
    void setCOMColor();
    void setCenterColor();
    void setParticleColor(double distance);

    QPoint last_mouse_pos;

    std::vector<std::tuple<double, ga::Vector3D>> rel_positions;

    RenderSettings settings;
    RenderData data;
    Camera camera;

    GLuint circleVBO;
};
