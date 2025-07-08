#pragma once

#include <memory>

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>

// Forward declaration of BarnesHutNode
namespace enkas::simulation { struct BarnesHutNode; }

namespace enkas::simulation {

class BarnesHutTree
{
public:
    BarnesHutTree();
    ~BarnesHutTree();
    BarnesHutTree(BarnesHutTree&&) noexcept;
    BarnesHutTree& operator=(BarnesHutTree&&) noexcept;

    /**
     * @brief Builds the Barnes-Hut tree from the given system.
     */
    void build(const data::System& system);

    /**
     * @brief Updates the accelerations of particles in the system using the Barnes-Hut tree.
     * 
     * @param system The system containing particle data.
     * @param theta_mac_sqr The squared multipole acceptance criterion.
     * @param softening_sqr The squared softening parameter.
     * @param out_acc Output vector to store calculated accelerations.
     */
    void updateForces( const data::System& system
                     , double theta_mac_sqr
                     , double softening_sqr
                     , std::vector<math::Vector3D>& out_acc) const;

private:
    std::unique_ptr<BarnesHutNode> root_;
};

} // namespace enkas::simulation
