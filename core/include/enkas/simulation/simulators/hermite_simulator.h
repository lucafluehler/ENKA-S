#pragma once

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/simulation/settings/hermite_settings.h>
#include <enkas/simulation/simulator.h>

#include <vector>

namespace enkas::simulation {

class HermiteSimulator : public Simulator {
public:
    explicit HermiteSimulator(const HermiteSettings& settings);

    ~HermiteSimulator() override = default;

    void setSystem(const data::System& initial_system) override;
    void step() override;

    [[nodiscard]] double getSystemTime() const override;
    [[nodiscard]] data::System getSystem() const override;
    [[nodiscard]] data::Diagnostics getDiagnostics() const override;

private:
    /**
     * @brief Calculates the accelerations of the particles.
     */
    void updateForces();

private:
    HermiteSettings settings_;

    double system_time_ = 0.0;       // current time of the system
    const double softening_sqr_;     // squared softening parameters
    double potential_energy_ = 0.0;  // potential energy of the system

    data::System system_;                        // current state of the system
    std::vector<math::Vector3D> accelerations_;  // accelerations of the particles
    std::vector<math::Vector3D> jerks_;          // jerks of the particles
};

}  // namespace enkas::simulation
