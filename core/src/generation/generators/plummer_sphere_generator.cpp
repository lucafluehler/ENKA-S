#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generators/plummer_sphere_generator.h>
#include <enkas/physics/physics_helpers.h>

namespace enkas::generation {

PlummerSphereGenerator::PlummerSphereGenerator(const PlummerSphereSettings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

data::InitialSystem PlummerSphereGenerator::createSystem()
{
    data::InitialSystem initial_system;
    initial_system.reserve(settings.N);

    const double c_GRAV_MASS = std::sqrt(2.0*physics::G*settings.total_mass);
    const double c_PLUMMER_RADIUS_SQR = settings.radius*settings.radius;

    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    double cumulative_mass_min = 0.0;
    double cumulative_mass_max = 1.0/settings.N;

    for (size_t i = 0; i < settings.N; i++) {
        data::BaseParticle particle;

        // MASS
        particle.mass = settings.total_mass/settings.N;

        std::uniform_real_distribution<double>
                    mass_dist( cumulative_mass_min, cumulative_mass_max );

        double cumulative_mass = mass_dist(gen);
        cumulative_mass_min = cumulative_mass_max;
        cumulative_mass_max += 1.0/settings.N;

        // POSITION
        const double c_MASS_TERM = std::pow(cumulative_mass, -2.0/3.0);
        const double c_RADIUS = settings.radius/std::sqrt(c_MASS_TERM - 1.0);
        particle.pos = utils::getRandSphere(gen, c_RADIUS);

        // VELOCITY
        double q = 0.0;
        double rejection_parameter = 0.1;
        while (rejection_parameter > q*q*std::pow(1.0 - q*q, 3.5)) {
            q = dist(gen);
            rejection_parameter = dist(gen)*0.1;
        }

        const double c_MU = std::pow(c_PLUMMER_RADIUS_SQR + c_RADIUS*c_RADIUS, -0.25);
        const double c_VELOCITY = q*c_GRAV_MASS*c_MU;
        particle.vel = utils::getRandSphere(gen, c_VELOCITY);

        initial_system.push_back(particle);
    }

    physics::centerParticles(initial_system);

    return initial_system;
}

} // namespace enkas::generation
