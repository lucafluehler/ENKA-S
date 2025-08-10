#include "rendering/particle_renderer.h"

#include <enkas/logging/logger.h>
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

#include "core/dataflow/snapshot.h"

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
    QImage screenshot = this->grabFramebuffer();

    QDir video_dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    if (video_dir.mkpath("VIDEO") && video_dir.cd("VIDEO")) {
        QString filename = QString("ENKAS_%1_%2.png")
                               .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"))
                               .arg(system_ ? system_->time : 0.0);
        QString save_path = video_dir.filePath(filename);

        if (!QFile(save_path).exists()) {
            screenshot.save(save_path);
        }
    }
}

void ParticleRenderer::initializeGL() {
    initializeOpenGLFunctions();
    initializeParticleShader();
    initializeCrossShader();
}

void ParticleRenderer::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void ParticleRenderer::paintGL() {
    setBackgroundColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    animation();

    projection_matrix_.setToIdentity();
    projection_matrix_.perspective(
        settings_.fov, static_cast<float>(width()) / height(), 0.1f, 1000.0f);

    view_matrix_.setToIdentity();
    view_matrix_.translate(0.0f, 0.0f, -camera_.target_distance);
    QQuaternion q_rotation = QQuaternion(camera_.rel_rotation.s,
                                         -camera_.rel_rotation.b_yz,
                                         camera_.rel_rotation.b_xz,
                                         camera_.rel_rotation.b_xy)
                                 .conjugated();
    view_matrix_.rotate(q_rotation);
    view_matrix_.translate(-camera_.target_pos.x, -camera_.target_pos.y, -camera_.target_pos.z);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    drawParticles();

    glDisable(GL_DEPTH_TEST);

    if (settings_.show_center_of_mass) {
        bool is_visible;
        // Use the new, correct projection function
        QPointF com_ndc = projectWorldToNdc(com_position_, &is_visible);

        if (is_visible) {
            QVector3D color;
            switch (settings_.coloring_method) {
                case ColoringMethod::BlackFog:
                    color = QVector3D(1.0f, 1.0f, 0.0f);  // Yellow
                    break;
                case ColoringMethod::WhiteFog:
                    color = QVector3D(0.0f, 220.0f / 255.0f, 0.0f);  // Green
                    break;
            }
            drawCross(com_ndc, 0.04f, color);
        }
    }

    if (settings_.show_center_of_screen) {
        QVector3D color;
        switch (settings_.coloring_method) {
            case ColoringMethod::BlackFog:
                color = QVector3D(1.0f, 1.0f, 1.0f);  // White
                break;
            case ColoringMethod::WhiteFog:
                color = QVector3D(0.1f, 0.1f, 220.0f / 255.0f);  // Blue
                break;
        }
        drawCross(QPointF(0.0f, 0.0f), 0.04f, color);
    }
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
    enkas::math::Vector3D displacement;

    const float distance = 0.02 * camera_.target_distance;

    switch (event->key()) {
        case Qt::Key_W:
            displacement.y = -distance;
            break;
        case Qt::Key_A:
            displacement.x = distance;
            break;
        case Qt::Key_S:
            displacement.y = distance;
            break;
        case Qt::Key_D:
            displacement.x = -distance;
            break;
        case Qt::Key_Q:
            displacement.z = -distance;
            break;
        case Qt::Key_E:
            displacement.z = distance;
            break;
        default:
            break;
    }

    camera_.target_pos += camera_.rel_rotation.get_reverse().rotate(displacement);
}

void ParticleRenderer::drawParticles() {
    if (!system_ || system_->data->positions.empty()) {
        return;
    }

    const auto& positions_d = system_->data->positions;  // 'd' for double
    const auto num_particles = system_->data->count();

    // Convert to float for GPU processing
    particle_positions_f_.clear();
    particle_positions_f_.reserve(num_particles * 3);

    for (const auto& p : positions_d) {
        particle_positions_f_.push_back(static_cast<float>(p.x));
        particle_positions_f_.push_back(static_cast<float>(p.y));
        particle_positions_f_.push_back(static_cast<float>(p.z));
    }

    shader_program_.bind();

    // Send matrices and settings to the shader
    shader_program_.setUniformValue("u_projection_matrix", projection_matrix_);
    shader_program_.setUniformValue("u_view_matrix", view_matrix_);
    shader_program_.setUniformValue("u_particle_size",
                                    static_cast<GLfloat>(settings_.particle_size_param / 1000.0f));
    shader_program_.setUniformValue("u_camera_target_distance", (float)camera_.target_distance);

    if (settings_.coloring_method == ColoringMethod::BlackFog) {
        shader_program_.setUniformValue("u_coloring_method", 0);
        shader_program_.setUniformValue("u_fog_mu", (float)settings_.black_fog_param);
    } else {  // WHITE_FOG
        shader_program_.setUniformValue("u_coloring_method", 1);
        shader_program_.setUniformValue("u_fog_mu", (float)settings_.white_fog_param);
    }

    vao_.bind();
    particle_position_vbo_.bind();

    // Determine if we need to reallocate the VBO
    const size_t required_bytes = particle_positions_f_.size() * sizeof(float);
    if (required_bytes > particle_vbo_capacity_bytes_) {
        // The buffer is too small, we must re-allocate.
        particle_position_vbo_.allocate(particle_positions_f_.data(),
                                        static_cast<int>(required_bytes));
        particle_vbo_capacity_bytes_ = required_bytes;
    } else {
        // The buffer is large enough, just update its content (fast path).
        particle_position_vbo_.write(
            0, particle_positions_f_.data(), static_cast<int>(required_bytes));
    }

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_particles);

    vao_.release();
    shader_program_.release();
}

void ParticleRenderer::initializeParticleShader() {
    // Load shaders
    shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/particle.vert");
    shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/particle.frag");
    if (!shader_program_.link()) {
        ENKAS_LOG_CRITICAL("Shader link error: {}", shader_program_.log().toStdString());
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
    particle_vbo_capacity_bytes_ = 0;

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

void ParticleRenderer::initializeCrossShader() {
    cross_shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cross.vert");
    cross_shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/cross.frag");
    if (!cross_shader_program_.link()) {
        ENKAS_LOG_CRITICAL("Cross shader link error: {}",
                           cross_shader_program_.log().toStdString());
        return;
    }

    // Geometry for a cross made of two rectangles
    const float thickness = 1.0f / 12.0f;
    const GLfloat cross_vertices[] = {// Horizontal bar
                                      -0.5f,
                                      -thickness,
                                      0.5f,
                                      -thickness,
                                      0.5f,
                                      thickness,
                                      -0.5f,
                                      -thickness,
                                      0.5f,
                                      thickness,
                                      -0.5f,
                                      thickness,
                                      // Vertical bar
                                      -thickness,
                                      -0.5f,
                                      thickness,
                                      -0.5f,
                                      thickness,
                                      0.5f,
                                      -thickness,
                                      -0.5f,
                                      thickness,
                                      0.5f,
                                      -thickness,
                                      0.5f};

    cross_vao_.create();
    cross_vao_.bind();
    cross_vbo_.create();
    cross_vbo_.bind();
    cross_vbo_.allocate(cross_vertices, sizeof(cross_vertices));

    cross_shader_program_.enableAttributeArray(0);
    cross_shader_program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    cross_vao_.release();
    cross_vbo_.release();
}

void ParticleRenderer::drawCross(const QPointF& center, float size, const QVector3D& color) {
    cross_shader_program_.bind();
    cross_vao_.bind();

    // Adjust size based on aspect ratio to keep the cross square
    float size_x = size;
    float size_y = size;
    if (width() > height()) {
        size_x *= static_cast<float>(height()) / width();
    } else {
        size_y *= static_cast<float>(width()) / height();
    }

    cross_shader_program_.setUniformValue("u_center", QVector2D(center));
    cross_shader_program_.setUniformValue("u_size", QVector2D(size_x, size_y));
    cross_shader_program_.setUniformValue("u_color", color);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    cross_vao_.release();
    cross_shader_program_.release();
}

QPointF ParticleRenderer::projectWorldToNdc(const enkas::math::Vector3D& world_pos,
                                            bool* is_visible) {
    QVector4D world_vec4(world_pos.x, world_pos.y, world_pos.z, 1.0);
    QVector4D clip_pos = projection_matrix_ * view_matrix_ * world_vec4;

    // If w is zero or negative, the point is behind or on the camera plane.
    if (clip_pos.w() <= 0.0f) {
        *is_visible = false;
        return QPointF();
    }

    QVector3D ndc_pos = clip_pos.toVector3D() / clip_pos.w();

    *is_visible = (std::abs(ndc_pos.x()) <= 1.0f && std::abs(ndc_pos.y()) <= 1.0f &&
                   std::abs(ndc_pos.z()) <= 1.0f);

    return QPointF(ndc_pos.x(), ndc_pos.y());
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
