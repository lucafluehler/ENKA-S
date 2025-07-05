#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QOpenGLFunctions>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QStandardPaths>
#include <QDateTime>
#include <QPainter>
#include <QOpenGLFramebufferObject>

#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>

#include "particle_renderer.h"
#include "utils.h"

ParticleRenderer::ParticleRenderer(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);

    setFocusPolicy(Qt::StrongFocus);
}


void ParticleRenderer::updateData(const std::shared_ptr<RenderData>& d)
{
    if (d) data = *d;
    rel_positions.clear();
    rel_positions.reserve(data.positions.size());
}

void ParticleRenderer::updateData(const std::unique_ptr<RenderData>& d)
{
    if (d) data = *d;
    rel_positions.clear();
    rel_positions.reserve(data.positions.size());
}

void ParticleRenderer::redraw(const RenderSettings& p_settings)
{
    settings = p_settings;

    update();
}


void ParticleRenderer::saveScreenshot()
{
    QImage screenshot(size(), QImage::Format_RGB32);

    QPainter painter(&screenshot);

    render(&painter);

    QString download_dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    QString video_dir = QDir(download_dir).filePath("VIDEO");

    if (!QDir(video_dir).exists()) QDir().mkdir(video_dir);

    QString timestamp = QDateTime::currentDateTime().toString("_yyyyMMdd_hhmmss_");
    QString save_path = video_dir + "/ENKAS" + timestamp
                       + QString::number(data.time) + ".png";

    if (QFile(save_path).exists()) return;

    screenshot.save(save_path);
    qDebug() << "Screenshot wurde im Download-Ordner gespeichert:" << save_path;
}


void ParticleRenderer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ParticleRenderer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void ParticleRenderer::paintGL()
{
    setBackgroundColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    animation();

    drawParticles();

    if (settings.show_center_of_mass) {
        setCOMColor();
        ga::Vector3D rel_pos = getRelPos(data.com_position);

        const float c_ASPECT_RATIO = static_cast<float>(width())/height();
        const float c_HALF_TAN_FOV = std::tan(settings.fov*M_PI/180.0f/2.0f);

        bool is_visible;
        QPointF com = convertPosToLoc(c_ASPECT_RATIO, c_HALF_TAN_FOV, rel_pos, &is_visible);
        if (is_visible) drawCross(com.x(), com.y(), 0.02f);
    }

    if (settings.show_center_of_screen && camera.target_distance > 0) {
        setCenterColor();
        drawCross(0.0f, 0.0f, 0.02f);
    }
}


void ParticleRenderer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
        last_mouse_pos = event->pos();
}

void ParticleRenderer::mouseMoveEvent(QMouseEvent* event)
{
    int dx = event->pos().x() - last_mouse_pos.x();
    int dy = event->pos().y() - last_mouse_pos.y();

    float mu = 0.0006;

    if (event->buttons() & Qt::LeftButton)
    {
        camera.rel_rotation = ga::Rotor3D(1.0, 0.0, -dx*mu, dy*mu*1.5).normalize()
                              *camera.rel_rotation;

        last_mouse_pos = event->pos();
    }

    if (event->buttons() & Qt::RightButton)
    {
        camera.rel_rotation = ga::Rotor3D(1.0, dy*mu, 0.0, 0.0).normalize()
                              *camera.rel_rotation;

        last_mouse_pos = event->pos();
    }
}

void ParticleRenderer::wheelEvent(QWheelEvent* event)
{
    camera.target_distance /= std::pow(1.001, event->angleDelta().y());
    if (camera.target_distance < 0) camera.target_distance = 0;
}

void ParticleRenderer::keyPressEvent(QKeyEvent *event)
{
    ga::Vector3D movement;

    const float c_MOVE = 0.02*camera.target_distance;

    switch (event->key())
    {
    case Qt::Key_W: movement.y = -c_MOVE; break;
    case Qt::Key_A: movement.x =  c_MOVE; break;
    case Qt::Key_S: movement.y =  c_MOVE; break;
    case Qt::Key_D: movement.x = -c_MOVE; break;
    case Qt::Key_Q: movement.z = -c_MOVE; break;
    case Qt::Key_E: movement.z =  c_MOVE; break;
    default: break;
    }

    camera.target_pos += camera.rel_rotation.get_reverse().rotate(movement);
}


void ParticleRenderer::drawParticles()
{
    // Map particle distance to the camera to its position relative to the camera
    rel_positions.clear();

    for (const auto& pos: data.positions) {
        ga::Vector3D rel_pos = getRelPos(pos);
        rel_positions.push_back(std::make_tuple(rel_pos.norm(), rel_pos));
    }

    // Sort by increasing distance to the camera.
    std::sort( rel_positions.begin(), rel_positions.end()
             , [](const auto& pd1, const auto& pd2)
               { return get<0>(pd1) > get<0>(pd2); } );

    glGenBuffers(1, &circleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

    const float c_ASPECT_RATIO = static_cast<float>(width())/height();
    const float c_HALF_TAN_FOV = std::tan(settings.fov*M_PI/180.0f/2.0f);
    const float c_SIZE_PARAM = settings.particle_size_param/10.0;

    bool is_vertical = width() < height();
    const float c_X_ASPECT = is_vertical ? static_cast<float>(height()) / width() : 1.0f;
    const float c_Y_ASPECT = is_vertical ? 1.0f : static_cast<float>(width()) / height();

    std::vector<GLfloat> vertices;
    const int c_NUM_TRANGLES = 12;
    vertices.reserve(2 * (c_NUM_TRANGLES + 1));

    for (const auto& [distance, pos] : rel_positions) {
        setParticleColor(distance);

        bool is_visible;
        QPointF screen_pos = convertPosToLoc( c_ASPECT_RATIO, c_HALF_TAN_FOV
                                            , pos, &is_visible );

        if (!is_visible) continue;

        drawCircle( vertices, c_NUM_TRANGLES, c_X_ASPECT, c_Y_ASPECT
                  , screen_pos.x(), screen_pos.y(), c_SIZE_PARAM/distance );
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &circleVBO);
}

void ParticleRenderer::drawCircle( std::vector<GLfloat>& vertices
                                 , const int c_NUM_TRANGLES
                                 , const float c_X_ASPECT, const float c_Y_ASPECT
                                 , float x, float y, float radius)
{
    vertices.clear();

    // Center coordinates
    vertices.push_back(x);
    vertices.push_back(y);

    const float c_ANGLE_INCREMENT = 2.0f*M_PI/c_NUM_TRANGLES;

    for (int i = 0; i <= c_NUM_TRANGLES; i++) {
        float angle = i * c_ANGLE_INCREMENT;
        float triangle_x = x + radius*cos(angle)*c_X_ASPECT;
        float triangle_y = y + radius*sin(angle)*c_Y_ASPECT;

        vertices.push_back(triangle_x);
        vertices.push_back(triangle_y);
    }

    // VBO goes brrrrrrrrrrrrrr
    glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat)
                 , vertices.data(), GL_STATIC_DRAW );
    glDrawArrays(GL_TRIANGLE_FAN, 0, c_NUM_TRANGLES + 2);
}

void ParticleRenderer::drawCross(float x, float y, float size)
{
    float thickness = size/6.0;

    float x_aspect = 1.0f;
    float y_aspect = 1.0f;
    if (width() < height()) {
        y_aspect = static_cast<float>(width())/height();
    } else {
        x_aspect = static_cast<float>(height())/width();
    }

    float x_hor = size*x_aspect;
    float x_ver = thickness*x_aspect;
    float y_hor = thickness*y_aspect;
    float y_ver = size*y_aspect;

    // Define vertices for the rectangle using two triangles
    // Horizontal Rectangle
    GLfloat vertices[] = {
        // Horizontal Rectangle
        x - x_hor, y - y_hor,
        x + x_hor, y - y_hor,
        x + x_hor, y + y_hor,
        x - x_hor, y - y_hor,
        x + x_hor, y + y_hor,
        x - x_hor, y + y_hor,

        // Vertical Rectangle
        x - x_ver, y - y_ver,
        x + x_ver, y - y_ver,
        x + x_ver, y + y_ver,
        x - x_ver, y - y_ver,
        x + x_ver, y + y_ver,
        x - x_ver, y + y_ver,
    };

    // VBO goes brrrrrrrrrrrr
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
}

ga::Vector3D ParticleRenderer::getRelPos(const ga::Vector3D& pos)
{
    ga::Vector3D rel_pos = camera.rel_rotation.rotate(camera.target_pos - pos)
                           - ga::Vector3D(0.0, 0.0, -camera.target_distance);
    return rel_pos;
}

QPointF ParticleRenderer::convertPosToLoc( const float c_ASPECT_RATIO
                                         , const float c_HALF_TAN_FOV
                                         , const ga::Vector3D& rel_pos
                                         , bool* is_visible )
{
    // Check if particle is behind camera
    if (rel_pos.z <= 0) {
        if (is_visible) *is_visible = false;
        return QPointF();
    }

    double x = rel_pos.x/rel_pos.z/c_HALF_TAN_FOV/c_ASPECT_RATIO;
    double y = rel_pos.y/rel_pos.z/c_HALF_TAN_FOV;

    // Check if particle is outside screen
    if (x < -1.2 || x > 1.2 || y < -1.2 || y > 1.2) {
        if (is_visible) *is_visible = false;
        return QPointF();
    }

    if (is_visible) *is_visible = true;
    return QPointF(x, y);
}

void ParticleRenderer::animation()
{
    double s = 0.002*settings.animation_speed; // speed parameter

    ga::Rotor3D animation;

    switch (settings.animation_style) {
    case AnimationStyle::RIGHT:     animation = ga::Rotor3D(1.0, 0.0,  -s, 0.0); break;
    case AnimationStyle::LEFT:      animation = ga::Rotor3D(1.0, 0.0,   s, 0.0); break;
    case AnimationStyle::UP:        animation = ga::Rotor3D(1.0, 0.0, 0.0,  -s); break;
    case AnimationStyle::DOWN:      animation = ga::Rotor3D(1.0, 0.0, 0.0,   s); break;
    case AnimationStyle::CWISE:     animation = ga::Rotor3D(1.0,   s, 0.0, 0.0); break;
    case AnimationStyle::CNTRCWISE: animation = ga::Rotor3D(1.0,  -s, 0.0, 0.0); break;
    case AnimationStyle::TUTTI:     animation = ga::Rotor3D(1.0,   s,   s,   s); break;
    case AnimationStyle::NONE: return;
    default: break;
    }

    camera.rel_rotation = animation.normalize()*camera.rel_rotation;
}


void ParticleRenderer::setBackgroundColor()
{
    switch (settings.coloring_method) {
    case ColoringMethod::BLACK_FOG: glClearColor(0.0667f, 0.0667f, 0.0667f, 1.0f); break;
    case ColoringMethod::WHITE_FOG: glClearColor(1.0f, 1.0f, 1.0f, 1.0f); break;
    default: break;
    }
}

void ParticleRenderer::setCOMColor()
{
    switch (settings.coloring_method) {
    case ColoringMethod::BLACK_FOG: glColor3ub(255, 255, 0); break;
    case ColoringMethod::WHITE_FOG: glColor3ub(0, 220, 0); break;
    default: break;
    }
}

void ParticleRenderer::setCenterColor()
{
    switch (settings.coloring_method) {
    case ColoringMethod::BLACK_FOG: glColor3ub(255, 255, 255); break;
    case ColoringMethod::WHITE_FOG: glColor3ub(0, 0, 160); break;
    default: break;
    }
}

void ParticleRenderer::setParticleColor(double distance)
{
    double c;
    double mu;
    switch (settings.coloring_method) {
    case ColoringMethod::BLACK_FOG:
        mu = settings.black_fog_param;
        c = 1/(1 + std::exp((distance/(camera.target_distance*1.3) - 1)*mu));
        glColor3ub(255*c, 255*std::pow(c, 6), 255*std::pow(c, 8));
        break;
    case ColoringMethod::WHITE_FOG:
        mu = settings.white_fog_param;
        c = -1/(1 + std::exp((distance/(camera.target_distance*1) - 1)*mu)) + 1;
        glColor3ub(255*c, 255*std::pow(c, 6), 255*std::pow(c, 8));
        break;
    default: break;
    }
}
