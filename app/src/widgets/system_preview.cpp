#include "system_preview.h"

#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generation_settings.h>
#include <enkas/generation/generator.h>
#include <enkas/logging/logger.h>

#include <memory>

#include "core/dataflow/snapshot.h"
#include "core/settings/generation_method.h"
#include "rendering/particle_renderer.h"
#include "services/file_parser/file_parser.h"

namespace {
// --- Default Preview Configuration ---
constexpr unsigned int kDefaultPreviewSeed = 42;
constexpr int kDefaultPreviewParticleCount = 1000;

// --- Default Render Settings for Previews ---
constexpr double kPreviewAnimationSpeed = 1.4;
constexpr int kPreviewFov = 50;
constexpr double kPreviewParticleSize = 120.0;

// --- Home Screen Specific Configuration ---
constexpr unsigned int kHomeScreenSeed = 42;
constexpr double kHomeScreenAnimationSpeed = 1.0;
constexpr int kHomeScreenFov = 60;
constexpr double kHomeScreenParticleSize = 70.0;
constexpr double kHomeScreenImpactParam = 0.8;
constexpr double kHomeScreenRelVelocity = 0.1;
constexpr int kHomeScreenParticleCount = 2400;
constexpr double kHomeScreenSphereRadius = 0.4;
constexpr double kHomeScreenTotalMass = 10.0;

// --- Generation Method Specific Parameters ---
// Normal Sphere
constexpr double kNormalSpherePosStdDev = 0.7;
constexpr double kNormalSphereVelStdDev = 0.1;
constexpr double kNormalSphereMassMean = 50.0;
constexpr double kNormalSphereMassStdDev = 1.0;
// Uniform Cube
constexpr double kUniformCubeSideLength = 3.0;
constexpr double kUniformCubeInitialVel = 0.1;
constexpr double kUniformCubeTotalMass = 50.0;
// Uniform Sphere
constexpr double kUniformSphereRadius = 2.0;
constexpr double kUniformSphereInitialVel = 0.1;
constexpr double kUniformSphereTotalMass = 50.0;
// Plummer Sphere
constexpr double kPlummerSphereRadius = 0.6;
constexpr double kPlummerSphereTotalMass = 5000.0;
// Spiral Galaxy
constexpr int kSpiralGalaxyNumArms = 2;
constexpr double kSpiralGalaxyRadius = 4.0;
constexpr double kSpiralGalaxyTotalMass = 10.0;
constexpr double kSpiralGalaxyTwist = 2.4;
constexpr double kSpiralGalaxyBlackHoleMass = 10000.0;
// Collision Model
constexpr int kCollisionModelParticleCount1 = 600;
constexpr double kCollisionModelRadius1 = 0.3;
constexpr double kCollisionModelMass1 = 3000.0;
constexpr int kCollisionModelParticleCount2 = 400;
constexpr double kCollisionModelRadius2 = 0.2;
constexpr double kCollisionModelMass2 = 2000.0;

}  // namespace

SystemPreview::SystemPreview(QWidget* parent) : ParticleRenderer(parent) {
    // Set default Render Settings
    RenderSettings render_settings;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    redraw(render_settings);

    // Populate the settings factory dispatch table
    settings_factories_[GenerationMethod::NormalSphere] = []() {
        enkas::generation::NormalSphereSettings s;
        s.seed = kDefaultPreviewSeed;
        s.particle_count = kDefaultPreviewParticleCount;
        s.position_std_dev = kNormalSpherePosStdDev;
        s.velocity_std_dev = kNormalSphereVelStdDev;
        s.mass_mean = kNormalSphereMassMean;
        s.mass_std_dev = kNormalSphereMassStdDev;
        return s;
    };
    settings_factories_[GenerationMethod::UniformCube] = []() {
        enkas::generation::UniformCubeSettings s;
        s.seed = kDefaultPreviewSeed;
        s.particle_count = kDefaultPreviewParticleCount;
        s.side_length = kUniformCubeSideLength;
        s.initial_velocity = kUniformCubeInitialVel;
        s.total_mass = kUniformCubeTotalMass;
        return s;
    };
    settings_factories_[GenerationMethod::UniformSphere] = []() {
        enkas::generation::UniformSphereSettings s;
        s.seed = kDefaultPreviewSeed;
        s.particle_count = kDefaultPreviewParticleCount;
        s.sphere_radius = kUniformSphereRadius;
        s.initial_velocity = kUniformSphereInitialVel;
        s.total_mass = kUniformSphereTotalMass;
        return s;
    };
    settings_factories_[GenerationMethod::PlummerSphere] = []() {
        enkas::generation::PlummerSphereSettings s;
        s.seed = kDefaultPreviewSeed;
        s.particle_count = kDefaultPreviewParticleCount;
        s.sphere_radius = kPlummerSphereRadius;
        s.total_mass = kPlummerSphereTotalMass;
        return s;
    };
    settings_factories_[GenerationMethod::SpiralGalaxy] = []() {
        enkas::generation::SpiralGalaxySettings s;
        s.seed = kDefaultPreviewSeed;
        s.particle_count = kDefaultPreviewParticleCount;
        s.num_arms = kSpiralGalaxyNumArms;
        s.radius = kSpiralGalaxyRadius;
        s.total_mass = kSpiralGalaxyTotalMass;
        s.twist = kSpiralGalaxyTwist;
        s.black_hole_mass = kSpiralGalaxyBlackHoleMass;
        return s;
    };
    settings_factories_[GenerationMethod::CollisionModel] = []() {
        enkas::generation::CollisionModelSettings s;
        s.seed = kDefaultPreviewSeed;
        s.particle_count_1 = kCollisionModelParticleCount1;
        s.sphere_radius_1 = kCollisionModelRadius1;
        s.total_mass_1 = kCollisionModelMass1;
        s.particle_count_2 = kCollisionModelParticleCount2;
        s.sphere_radius_2 = kCollisionModelRadius2;
        s.total_mass_2 = kCollisionModelMass2;
        return s;
    };
}

void SystemPreview::initializeProcedural(GenerationMethod method) {
    setMethodSettings();

    auto it = settings_factories_.find(method);
    if (it == settings_factories_.end()) {
        ENKAS_LOG_ERROR("Unsupported generation method for procedural preview: {}",
                        generationMethodToString(method));
        return;
    }

    auto generator = enkas::generation::Factory::create(it->second());
    if (!generator) {
        ENKAS_LOG_ERROR("Failed to create generator for procedural preview");
        return;
    }

    updateData(std::make_shared<SystemSnapshot>(generator->createSystem()));
}

void SystemPreview::initializeFromFile(const QString& system_path) {
    setMethodSettings();

    auto system = FileParser().parseInitialSystem(system_path.toStdString());
    if (!system) {
        ENKAS_LOG_ERROR("Failed to parse initial system from file: {}", system_path.toStdString());
        return;
    }

    updateData(std::make_shared<SystemSnapshot>(system.value()));
}

void SystemPreview::initializeSystem(const enkas::data::System& system) {
    setMethodSettings();
    updateData(std::make_shared<SystemSnapshot>(system));
}

void SystemPreview::initializeHomeScreen() {
    setHomeScreenSettings();

    enkas::generation::CollisionModelSettings settings;
    settings.seed = kHomeScreenSeed;
    settings.impact_parameter = kHomeScreenImpactParam;
    settings.relative_velocity = kHomeScreenRelVelocity;
    settings.particle_count_1 = kHomeScreenParticleCount;
    settings.sphere_radius_1 = kHomeScreenSphereRadius;
    settings.total_mass_1 = kHomeScreenTotalMass;
    settings.particle_count_2 = kHomeScreenParticleCount;
    settings.sphere_radius_2 = kHomeScreenSphereRadius;
    settings.total_mass_2 = kHomeScreenTotalMass;

    auto system = enkas::generation::Factory::create(settings)->createSystem();

    updateData(std::make_shared<SystemSnapshot>(system));
}

void SystemPreview::clearPreview() { clearData(); }

void SystemPreview::setMethodSettings() {
    RenderSettings render_settings;
    render_settings.animation_style = AnimationStyle::Tutti;
    render_settings.animation_speed = kPreviewAnimationSpeed;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    render_settings.fov = kPreviewFov;
    render_settings.particle_size_param = kPreviewParticleSize;

    redraw(render_settings);
}

void SystemPreview::setHomeScreenSettings() {
    RenderSettings render_settings;
    render_settings.animation_style = AnimationStyle::Up;
    render_settings.animation_speed = kHomeScreenAnimationSpeed;
    render_settings.show_center_of_mass = false;
    render_settings.show_center_of_screen = false;
    render_settings.fov = kHomeScreenFov;
    render_settings.particle_size_param = kHomeScreenParticleSize;

    redraw(render_settings);
}
