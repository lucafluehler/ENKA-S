#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generators/spiral_galaxy_generator.h>
#include <enkas/physics/physics_helpers.h>

SpiralGalaxyGenerator::SpiralGalaxyGenerator(const SpiralGalaxySettings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

data::InitialSystem SpiralGalaxyGenerator::createSystem()
{
    data::InitialSystem initial_system;
    initial_system.reserve(settings.N + 1); // add one for the black hole

    const double c_STELLAR_MASS = settings.total_mass/settings.N;
    const double c_INNER_RADIUS = settings.radius/40.0;

    std::mt19937 gen(seed);
    std::normal_distribution<double> disk_thickness_dist(0.0, settings.radius/100.0);

    // Generate Disk
    const double c_N_PER_ARM = settings.N/settings.arms;

    for (size_t k = 0; k < settings.arms; k++) {
        for (size_t i = 0; i < c_N_PER_ARM; i++) {
            data::BaseParticle particle;

            // Mass
            particle.mass = c_STELLAR_MASS;

            // Position
            const double c_DISTANCE = c_INNER_RADIUS + settings.radius*i/settings.N;
            const double c_ANGLE = (settings.twist*M_PI*i/c_N_PER_ARM) + (2*M_PI*k/settings.arms);
            particle.pos = math::Vector3D(std::sin(c_ANGLE), std::cos(c_ANGLE), 0.0)
                           .set_norm(c_DISTANCE);

            // This can also be done with a thicc GA statement using Vectors, Bivectors
            // and Rotors
            // particle.pos = math::Rotor3D(c_ANGLE, math::Bivector3D::XY()).normalize()
            //                .rotate(math::Vector3D::X(c_DISTANCE));

            // Velocity
            const double eccentricity_mean = 0.4/(1 + std::exp((settings.N/50.0 - i)/4.0)) + 0.05;
            std::normal_distribution<double> eccentricity_dist(eccentricity_mean, 0.1);
            double eccentricity = eccentricity_dist(gen);

            // rejection technique to ensure an elliptic trajectory
            while (eccentricity >= 1.0 || eccentricity <= 0.0) {
                eccentricity = eccentricity_dist(gen);
            }

            const double c_MAJOR_HALF_AXIS = c_DISTANCE/(1 + eccentricity);
            const double c_FIRST_TERM = utils::G*(settings.bh_mass + settings.total_mass);
            const double c_SECOND_TERM = (2.0/c_DISTANCE - 1.0/c_MAJOR_HALF_AXIS);
            const double c_VELOCITY = std::sqrt(c_FIRST_TERM*c_SECOND_TERM);
            particle.vel = math::Vector3D(particle.pos.y, -particle.pos.x, 0.0)
                           .set_norm(c_VELOCITY)*(-1);

            // This can also be done using a GA statement by taking the Hodge Dual of the
            // Bivector spanned by wedging the position vector with the unit
            // z-axis vector... which is the same as the cross product *yuck*
            // particle.vel = math::wedge(particle.pos, math::Vector3D::Z()).getPerpendicular()
            //               .set_norm(c_VELOCITY)*(-1);

            particle.pos.z = disk_thickness_dist(gen);

            initial_system.push_back(particle);
        }
    }

    // Add black hole to center
    data::BaseParticle black_hole;
    black_hole.mass = settings.bh_mass;
    initial_system.push_back(black_hole);

    physics::centerParticles(initial_system);

    return initial_system;
}
