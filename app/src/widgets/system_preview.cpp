#include "system_preview.h"

#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generation_method.h>
#include <enkas/generation/generator.h>

#include <fstream>

#include "../core/settings.h"
#include "enkas/generation/settings/collision_model_settings.h"

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
    auto settings = Settings();
    settings.setSetting(
        "GenerationMethod",
        Setting(Setting::Group::Generation, Setting::Type::GenerationMethod, method));
    auto config = settings.getGenerationConfig();
    auto system = enkas::generation::Factory::create(config)->createSystem();

    // Feed system to particle renderer
    updateData(std::make_shared<enkas::data::System>(system));
}

void SystemPreview::initializeFromFile(const QString& system_path) {
    setMethodSettings();

    // Load system from file
    std::ifstream stream(system_path.toStdString());
    if (!stream.is_open()) {
        throw std::runtime_error(
            std::format("Failed to open system file: {}", system_path.toStdString()));
    }
    auto system = enkas::generation::Factory::create(stream)->createSystem();

    // Feed system to particle renderer
    updateData(std::make_shared<enkas::data::System>(system));
}

void SystemPreview::initializeHomeScreen() {
    setHomeScreenSettings();

    // Generate system
    enkas::generation::CollisionModelSettings settings;
    settings.particle_count_1 = 1200;
    settings.sphere_radius_1 = 0.6;
    settings.particle_count_2 = 1200;
    settings.sphere_radius_2 = 0.6;
    auto config = enkas::generation::Config();
    config.seed = 42;
    config.specific_settings = settings;
    auto system = enkas::generation::Factory::create(config)->createSystem();

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
