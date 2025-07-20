#include "rendering/particle_renderer.h"

#include <enkas/math/rotor3d.h>
#include <enkas/math/vector3d.h>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPixmap>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <cmath>
#include <memory>
#include <vector>

#include "core/snapshot.h"

ParticleRenderer::ParticleRenderer(QWidget* parent) : QOpenGLWidget(parent) {
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    setFocusPolicy(Qt::StrongFocus);
}

void ParticleRenderer::updateData(SystemSnapshotPtr system) {
    if (!system) return;
    system_ = std::move(system);
}

void ParticleRenderer::redraw(const RenderSettings& settings) {
    settings_ = settings;
    update();
}

void ParticleRenderer::clearData() {
    system_.reset();
    update();
}

void ParticleRenderer::saveScreenshot() {
    QImage screenshot(size(), QImage::Format_RGB32);

    QPainter painter(&screenshot);

    render(&painter);

    QDir video_dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    video_dir.mkpath("VIDEO");
    video_dir.cd("VIDEO");

    QString filename = QString("ENKAS_%1_%2.png")
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"))
                           .arg(system_->time);
    QString save_path = video_dir.filePath(filename);

    if (QFile(save_path).exists()) return;

    screenshot.save(save_path);
}

void ParticleRenderer::initializeGL() {
    initializeOpenGLFunctions();

    // Load shaders
    shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/particle.vert");
    shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/particle.frag");
    if (!shader_program_.link()) {
        qCritical() << "Shader link error:" << shader_program_.log();
        return;
    }

    // A simple quad that we will draw for each particle
    const GLfloat quad_vertices[] = {
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
    };
    quad_vbo_.create();
    quad_vbo_.bind();
    quad_vbo_.allocate(quad_vertices, sizeof(quad_vertices));
    quad_vbo_.release();

    particle_position_vbo_.create();
    particle_position_vbo_.setUsagePattern(QOpenGLBuffer::StreamDraw);

    vao_.create();
    vao_.bind();

    quad_vbo_.bind();
    shader_program_.enableAttributeArray(0);
    shader_program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);  // location=0, 2 floats

    particle_position_vbo_.bind();
    shader_program_.enableAttributeArray(1);
    shader_program_.setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);  // location=1, 3 floats

    glVertexAttribDivisor(1, 1);

    vao_.release();
}

void ParticleRenderer::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void ParticleRenderer::paintGL() {
    setBackgroundColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    animation();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    drawParticles();

    // if (settings_.show_center_of_mass) {
    //     setCOMColor();
    //     enkas::math::Vector3D rel_pos = getRelPos(data.com_position);

    //     const float c_ASPECT_RATIO = static_cast<float>(width()) / height();
    //     const float c_HALF_TAN_FOV = std::tan(settings_.fov * M_PI / 180.0f / 2.0f);

    //     bool is_visible;
    //     QPointF com = convertPosToLoc(c_ASPECT_RATIO, c_HALF_TAN_FOV, rel_pos, &is_visible);
    //     if (is_visible) drawCross(com.x(), com.y(), 0.02f);
    // }

    // if (settings_.show_center_of_screen && camera_.target_distance > 0) {
    //     setCenterColor();
    //     drawCross(0.0f, 0.0f, 0.02f);
    // }
}

void ParticleRenderer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
        last_mouse_pos_ = event->pos();
}

void ParticleRenderer::mouseMoveEvent(QMouseEvent* event) {
    int dx = event->pos().x() - last_mouse_pos_.x();
    int dy = event->pos().y() - last_mouse_pos_.y();

    const float mu = 0.0006;  // Mouse sensitivity

    if (event->buttons() & Qt::LeftButton) {
        camera_.rel_rotation = enkas::math::Rotor3D(1.0, 0.0, -dx * mu, dy * mu * 1.5).normalize() *
                               camera_.rel_rotation;

        last_mouse_pos_ = event->pos();
    }

    if (event->buttons() & Qt::RightButton) {
        camera_.rel_rotation =
            enkas::math::Rotor3D(1.0, dy * mu, 0.0, 0.0).normalize() * camera_.rel_rotation;

        last_mouse_pos_ = event->pos();
    }
}

void ParticleRenderer::wheelEvent(QWheelEvent* event) {
    camera_.target_distance /= std::pow(1.001, event->angleDelta().y());
    if (camera_.target_distance < 0) camera_.target_distance = 0;
}

void ParticleRenderer::keyPressEvent(QKeyEvent* event) {
    enkas::math::Vector3D movement;

    const float c_MOVE = 0.02 * camera_.target_distance;

    switch (event->key()) {
        case Qt::Key_W:
            movement.y = -c_MOVE;
            break;
        case Qt::Key_A:
            movement.x = c_MOVE;
            break;
        case Qt::Key_S:
            movement.y = c_MOVE;
            break;
        case Qt::Key_D:
            movement.x = -c_MOVE;
            break;
        case Qt::Key_Q:
            movement.z = -c_MOVE;
            break;
        case Qt::Key_E:
            movement.z = c_MOVE;
            break;
        default:
            break;
    }

    camera_.target_pos += camera_.rel_rotation.get_reverse().rotate(movement);
}

void ParticleRenderer::drawParticles() {
    if (!system_ || system_->data.positions.empty()) {
        return;
    }

    const auto& positions_d = system_->data.positions;  // 'd' for double
    const auto num_particles = system_->data.count();

    // Convert to float for GPU processing
    std::vector<float> positions_f;
    positions_f.reserve(num_particles * 3);

    for (const auto& p : positions_d) {
        positions_f.push_back(static_cast<float>(p.x));
        positions_f.push_back(static_cast<float>(p.y));
        positions_f.push_back(static_cast<float>(p.z));
    }

    shader_program_.bind();
    vao_.bind();

    particle_position_vbo_.bind();
    particle_position_vbo_.allocate(positions_f.data(), positions_f.size() * sizeof(float));

    QMatrix4x4 projection_matrix;
    projection_matrix.perspective(
        settings_.fov, static_cast<float>(width()) / height(), 0.1f, 1000.0f);

    QMatrix4x4 view_matrix;
    view_matrix.translate(0.0f, 0.0f, -camera_.target_distance);
    QQuaternion q_rotation = QQuaternion(camera_.rel_rotation.s,
                                         -camera_.rel_rotation.b_yz,
                                         camera_.rel_rotation.b_xz,
                                         camera_.rel_rotation.b_xy)
                                 .conjugated();
    view_matrix.rotate(q_rotation);
    view_matrix.translate(-camera_.target_pos.x, -camera_.target_pos.y, -camera_.target_pos.z);

    // Send matrices and settings to the shader
    shader_program_.setUniformValue("u_projection_matrix", projection_matrix);
    shader_program_.setUniformValue("u_view_matrix", view_matrix);
    shader_program_.setUniformValue("u_particle_size",
                                    static_cast<GLfloat>(settings_.particle_size_param / 10.0f));
    shader_program_.setUniformValue("u_camera_target_distance", (float)camera_.target_distance);

    if (settings_.coloring_method == ColoringMethod::BlackFog) {
        shader_program_.setUniformValue("u_coloring_method", 0);
        shader_program_.setUniformValue("u_fog_mu", (float)settings_.black_fog_param);
    } else {  // WHITE_FOG
        shader_program_.setUniformValue("u_coloring_method", 1);
        shader_program_.setUniformValue("u_fog_mu", (float)settings_.white_fog_param);
    }

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_particles);

    vao_.release();
    shader_program_.release();
}

// void ParticleRenderer::drawCross(float x, float y, float size) {
//     float thickness = size / 6.0;

//     float x_aspect = 1.0f;
//     float y_aspect = 1.0f;
//     if (width() < height()) {
//         y_aspect = static_cast<float>(width()) / height();
//     } else {
//         x_aspect = static_cast<float>(height()) / width();
//     }

//     float x_hor = size * x_aspect;
//     float x_ver = thickness * x_aspect;
//     float y_hor = thickness * y_aspect;
//     float y_ver = size * y_aspect;

//     // Define vertices for the rectangle using two triangles
//     // Horizontal Rectangle
//     GLfloat vertices[] = {
//         // Horizontal Rectangle
//         x - x_hor,
//         y - y_hor,
//         x + x_hor,
//         y - y_hor,
//         x + x_hor,
//         y + y_hor,
//         x - x_hor,
//         y - y_hor,
//         x + x_hor,
//         y + y_hor,
//         x - x_hor,
//         y + y_hor,

//         // Vertical Rectangle
//         x - x_ver,
//         y - y_ver,
//         x + x_ver,
//         y - y_ver,
//         x + x_ver,
//         y + y_ver,
//         x - x_ver,
//         y - y_ver,
//         x + x_ver,
//         y + y_ver,
//         x - x_ver,
//         y + y_ver,
//     };

//     // VBO goes brrrrrrrrrrrr
//     GLuint vbo;
//     glGenBuffers(1, &vbo);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

//     glDrawArrays(GL_TRIANGLES, 0, 12);

//     glDisableVertexAttribArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glDeleteBuffers(1, &vbo);
// }

enkas::math::Vector3D ParticleRenderer::getRelPos(const enkas::math::Vector3D& pos) {
    enkas::math::Vector3D rel_pos = camera_.rel_rotation.rotate(camera_.target_pos - pos) -
                                    enkas::math::Vector3D(0.0, 0.0, -camera_.target_distance);
    return rel_pos;
}

void ParticleRenderer::animation() {
    double s = 0.002 * settings_.animation_speed;  // speed parameter

    enkas::math::Rotor3D animation;

    switch (settings_.animation_style) {
        case AnimationStyle::Right:
            animation = enkas::math::Rotor3D(1.0, 0.0, -s, 0.0);
            break;
        case AnimationStyle::Left:
            animation = enkas::math::Rotor3D(1.0, 0.0, s, 0.0);
            break;
        case AnimationStyle::Up:
            animation = enkas::math::Rotor3D(1.0, 0.0, 0.0, -s);
            break;
        case AnimationStyle::Down:
            animation = enkas::math::Rotor3D(1.0, 0.0, 0.0, s);
            break;
        case AnimationStyle::Clockwise:
            animation = enkas::math::Rotor3D(1.0, s, 0.0, 0.0);
            break;
        case AnimationStyle::Counterclockwise:
            animation = enkas::math::Rotor3D(1.0, -s, 0.0, 0.0);
            break;
        case AnimationStyle::Tutti:
            animation = enkas::math::Rotor3D(1.0, s, s, s);
            break;
        case AnimationStyle::None:
            return;
        default:
            break;
    }

    camera_.rel_rotation = animation.normalize() * camera_.rel_rotation;
}

void ParticleRenderer::setBackgroundColor() {
    switch (settings_.coloring_method) {
        case ColoringMethod::BlackFog:
            glClearColor(0.0667f, 0.0667f, 0.0667f, 1.0f);
            break;
        case ColoringMethod::WhiteFog:
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        default:
            break;
    }
}

// void ParticleRenderer::setCOMColor() {
//     switch (settings_.coloring_method) {
//         case ColoringMethod::BlackFog:
//             glColor3ub(255, 255, 0);
//             break;
//         case ColoringMethod::WhiteFog:
//             glColor3ub(0, 220, 0);
//             break;
//         default:
//             break;
//     }
// }

// void ParticleRenderer::setCenterColor() {
//     switch (settings_.coloring_method) {
//         case ColoringMethod::BlackFog:
//             glColor3ub(255, 255, 255);
//             break;
//         case ColoringMethod::WhiteFog:
//             glColor3ub(0, 0, 160);
//             break;
//         default:
//             break;
//     }
// }
