#include "system_preview.h"

#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generation_method.h>
#include <enkas/generation/generation_settings.h>
#include <enkas/generation/generator.h>

#include <memory>

#include "core/file_parse_logic.h"
#include "enkas/generation/settings/normal_sphere_settings.h"

SystemPreview::SystemPreview(QWidget* parent) : ParticleRenderer(parent) {
    // Set default Render Settings
    RenderSettings render_settings;
    render_settings.coloring_method = ColoringMethod::WhiteFog;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;

    redraw(render_settings);
}

void SystemPreview::initializeProcedural(enkas::generation::Method method) {
    setMethodSettings();

    // Generate system
    std::unique_ptr<enkas::generation::Generator> generator;

    switch (method) {
        case enkas::generation::Method::UniformCube:
            generator = enkas::generation::Factory::create(getUniformCubeSettings());
            break;
        case enkas::generation::Method::NormalSphere:
            generator = enkas::generation::Factory::create(getNormalSphereSettings());
            break;
        case enkas::generation::Method::UniformSphere:
            generator = enkas::generation::Factory::create(getUniformSphereSettings());
            break;
        case enkas::generation::Method::PlummerSphere:
            generator = enkas::generation::Factory::create(getPlummerSphereSettings());
            break;
        case enkas::generation::Method::SpiralGalaxy:
            generator = enkas::generation::Factory::create(getSpiralGalaxySettings());
            break;
        case enkas::generation::Method::CollisionModel:
            generator = enkas::generation::Factory::create(getCollisionModelSettings());
            break;
        default:
            return;  // Unsupported method
    }

    if (!generator) return;  // Generator creation failed

    // Feed system to particle renderer
    updateData(std::make_shared<enkas::data::System>(generator->createSystem()));
}

void SystemPreview::initializeFromFile(const QString& system_path) {
    setMethodSettings();

    auto system = FileParseLogic::parseInitialSystem(system_path.toStdString());

    if (!system) return;  // File parsing failed

    // Feed system to particle renderer
    updateData(std::make_shared<enkas::data::System>(system));
}

void SystemPreview::initializeHomeScreen() {
    setHomeScreenSettings();

    // Generate home screen system
    enkas::generation::CollisionModelSettings settings;
    settings.seed = 42;
    settings.particle_count_1 = 1200;
    settings.sphere_radius_1 = 0.6;
    settings.particle_count_2 = 1200;
    settings.sphere_radius_2 = 0.6;
    auto system = enkas::generation::Factory::create(settings)->createSystem();

    // Feed system to particle renderer
    updateData(std::make_shared<enkas::data::System>(system));
}

void SystemPreview::setMethodSettings() {
    // Render Settings
    RenderSettings render_settings;
    render_settings.animation_style = AnimationStyle::Tutti;
    render_settings.animation_speed = 1.4;
    render_settings.coloring_method = ColoringMethod::WhiteFog;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    render_settings.fov = 50;
    render_settings.particle_size_param = 1.2;

    redraw(render_settings);
}

void SystemPreview::setHomeScreenSettings() {
    // Render Settings
    RenderSettings render_settings;
    render_settings.animation_style = AnimationStyle::Up;
    render_settings.animation_speed = 1;
    render_settings.coloring_method = ColoringMethod::WhiteFog;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    render_settings.fov = 60;
    render_settings.particle_size_param = 0.4;

    redraw(render_settings);
}

enkas::generation::NormalSphereSettings SystemPreview::getNormalSphereSettings() {
    enkas::generation::NormalSphereSettings settings;
    settings.seed = 42;
    settings.particle_count = 1000;
    settings.position_std_dev = 0.7;
    settings.velocity_std_dev = 0.1;
    settings.mass_mean = 50.0;
    settings.mass_std_dev = 1.0;
    return settings;
}

enkas::generation::UniformCubeSettings SystemPreview::getUniformCubeSettings() {
    enkas::generation::UniformCubeSettings settings;
    settings.seed = 42;
    settings.particle_count = 1000;
    settings.side_length = 3.0;
    settings.initial_velocity = 0.1;
    settings.total_mass = 50.0;
    return settings;
}

enkas::generation::UniformSphereSettings SystemPreview::getUniformSphereSettings() {
    enkas::generation::UniformSphereSettings settings;
    settings.seed = 42;
    settings.particle_count = 1000;
    settings.sphere_radius = 2.0;
    settings.initial_velocity = 0.1;
    settings.total_mass = 50.0;
    return settings;
}

enkas::generation::PlummerSphereSettings SystemPreview::getPlummerSphereSettings() {
    enkas::generation::PlummerSphereSettings settings;
    settings.seed = 42;
    settings.particle_count = 1000;
    settings.sphere_radius = 0.6;
    settings.total_mass = 5000.0;
    return settings;
}

enkas::generation::SpiralGalaxySettings SystemPreview::getSpiralGalaxySettings() {
    enkas::generation::SpiralGalaxySettings settings;
    settings.seed = 42;
    settings.particle_count = 1000;
    settings.num_arms = 2;
    settings.radius = 4.0;
    settings.total_mass = 10.0;
    settings.twist = 2.4;
    settings.black_hole_mass = 10000.0;
    return settings;
}

enkas::generation::CollisionModelSettings SystemPreview::getCollisionModelSettings() {
    enkas::generation::CollisionModelSettings settings;
    settings.seed = 42;
    settings.particle_count_1 = 600;
    settings.sphere_radius_1 = 0.3;
    settings.total_mass_1 = 3000.0;
    settings.particle_count_2 = 400;
    settings.sphere_radius_2 = 0.2;
    settings.total_mass_2 = 2000.0;
    return settings;
}
