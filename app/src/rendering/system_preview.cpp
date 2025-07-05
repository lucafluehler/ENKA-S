#include "system_preview.h"

#include "settings.h"
#include "generation_factory.h"
#include "utils.h"

SystemPreview::SystemPreview(QWidget* parent)
    : ParticleRenderer(parent)
{
    // Default Render Settings
    auto render_settings = RenderSettings();
    render_settings.coloring_method = ColoringMethod::WHITE_FOG;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;

    redraw(render_settings);
}

void SystemPreview::initializeMethod( GenerationMethod method
                                    , const QString& initial_system_path )
{
    if (method == GenerationMethod::File && initial_system_path.isEmpty()) return;
    setMethodSettings();

    // Generate system
    GenerationSettings settings = Settings().getGenerationSettings();
    settings.method = method;
    settings.file_settings.system_data_path = initial_system_path.toStdString();
    auto system = GenerationFactory::create(settings)->createSystem();

    // Get Render Data from System
    RenderData render_data;
    render_data.positions.reserve(system.size());
    for (const auto& particle : system) {
        render_data.positions.push_back(particle.pos);
    }
    render_data.time = 0.0;
    render_data.com_position = math::Vector3D();

    updateData(std::make_shared<RenderData>(render_data));
}

void SystemPreview::initializeHomeScreen()
{
    setHomeScreenSettings();

    // Generate system
    GenerationSettings settings = Settings().getGenerationSettings();
    settings.method = GenerationMethod::CollisionModel;
    settings.collision_model_settings.N_1 = 1200;
    settings.collision_model_settings.radius_1 = 0.6;
    settings.collision_model_settings.N_2 = 1200;
    settings.collision_model_settings.radius_2 = 0.6;
    auto system = GenerationFactory::create(settings)->createSystem();

    // Get Render Data from System
    RenderData render_data;
    render_data.positions.reserve(system.size());
    for (const auto& particle : system) {
        render_data.positions.push_back(particle.pos);
    }
    render_data.time = 0.0;
    render_data.com_position = math::Vector3D();

    updateData(std::make_shared<RenderData>(render_data));
}


void SystemPreview::setMethodSettings()
{
    // Render Settings
    auto render_settings = RenderSettings();
    render_settings.animation_style = AnimationStyle::TUTTI;
    render_settings.animation_speed = 1.4;
    render_settings.coloring_method = ColoringMethod::WHITE_FOG;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    render_settings.fov = 50;
    render_settings.particle_size_param = 1.2;

    redraw(render_settings);
}

void SystemPreview::setHomeScreenSettings()
{
    // Render Settings
    auto render_settings = RenderSettings();
    render_settings.animation_style = AnimationStyle::UP;
    render_settings.animation_speed = 1;
    render_settings.coloring_method = ColoringMethod::WHITE_FOG;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    render_settings.fov = 60;
    render_settings.particle_size_param = 0.4;

    redraw(render_settings);
}
