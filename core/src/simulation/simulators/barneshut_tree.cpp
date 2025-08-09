#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/simulation/simulators/barneshut_tree.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace enkas::simulation {

struct BarnesHutNode {
    BarnesHutNode();
    BarnesHutNode(const math::Vector3D& max, const math::Vector3D& min);

    // Geometric properties of the node
    math::Vector3D max_point;
    math::Vector3D min_point;
    double edge_length_sqr = 0.0;

    // Mass properties of the node
    math::Vector3D center_of_mass;
    double total_mass = 0.0;
    int num_particles = 0;

    std::vector<std::unique_ptr<BarnesHutNode>> children;

    static constexpr size_t NO_PARTICLE = static_cast<size_t>(-1);
    size_t particle_index = NO_PARTICLE;
};

BarnesHutNode::BarnesHutNode() : children(8) {}
BarnesHutNode::BarnesHutNode(const math::Vector3D& max, const math::Vector3D& min)
    : max_point(max), min_point(min), children(8) {
    const math::Vector3D extent = max_point - min_point;
    const double max_edge = std::max({extent.x, extent.y, extent.z});
    edge_length_sqr = max_edge * max_edge;
}

namespace {  // Anonymous namespace for helper functions

void updateMassRecursive(BarnesHutNode& node, const data::System& system) {
    if (node.num_particles == 1) {
        node.center_of_mass = system.positions[node.particle_index];
        node.total_mass = system.masses[node.particle_index];
        return;
    }

    // Reset before accumulating from children
    node.total_mass = 0.0;
    node.center_of_mass.fill(0.0);

    for (auto& child : node.children) {
        if (!child) continue;
        updateMassRecursive(*child, system);
        node.total_mass += child->total_mass;
        node.center_of_mass += child->center_of_mass * child->total_mass;
    }

    if (node.total_mass > 0) {
        node.center_of_mass /= node.total_mass;
    }
}

math::Vector3D sumForcesRecursive(const BarnesHutNode& node,
                                  const math::Vector3D& target_pos,
                                  const double target_mass,
                                  size_t target_index,
                                  double& out_potential_energy,  // Accumulator
                                  const data::System& system,
                                  double theta_sqr,
                                  double softening_sqr) {
    // If this is a leaf node with one particle...
    if (node.num_particles == 1) {
        // ...and it's not the target particle itself...
        if (node.particle_index != target_index) {
            const math::Vector3D r = system.positions[node.particle_index] - target_pos;
            const double dist_sq = r.norm2() + softening_sqr;
            const double dist_inv = 1.0 / std::sqrt(dist_sq);

            // Accumulate potential energy (U = -m1*m2/r)
            out_potential_energy -= system.masses[node.particle_index] * target_mass * dist_inv;

            // Return acceleration (a = F/m1 = m2*r_vec/r^3)
            return r * (system.masses[node.particle_index] * dist_inv / dist_sq);
        }
        return {};  // Return zero vector if it's the same particle
    }

    // Check the Multipole Acceptance Criterion (MAC)
    const double d_sq = (node.center_of_mass - target_pos).norm2();
    if (node.edge_length_sqr / d_sq < theta_sqr) {
        // Node is far enough away, treat as a single mass
        const math::Vector3D r = node.center_of_mass - target_pos;
        const double dist_sq = r.norm2() + softening_sqr;
        const double dist_inv = 1.0 / std::sqrt(dist_sq);

        // Accumulate potential energy
        out_potential_energy -= node.total_mass * target_mass * dist_inv;

        // Return acceleration
        return r * (node.total_mass * dist_inv / dist_sq);
    }

    // Node is too close, recurse into its children
    math::Vector3D acc;
    for (const auto& child : node.children) {
        if (child) {
            acc += sumForcesRecursive(*child,
                                      target_pos,
                                      target_mass,
                                      target_index,
                                      out_potential_energy,
                                      system,
                                      theta_sqr,
                                      softening_sqr);
        }
    }

    return acc;
}

int getOctantIndex(const math::Vector3D& position, const math::Vector3D& center) {
    int index = 0;
    if (position.x > center.x) index |= 4;  // 100
    if (position.y > center.y) index |= 2;  // 010
    if (position.z > center.z) index |= 1;  // 001
    return index;
}

// Forward declare for mutual recursion
void insertRecursive(BarnesHutNode& node, size_t particle_index, const data::System& system);

void createAndInsertIntoChild(BarnesHutNode& parent,
                              int octant_index,
                              size_t particle_index,
                              const data::System& system) {
    if (!parent.children[octant_index]) {
        const math::Vector3D parent_center = (parent.max_point + parent.min_point) * 0.5;
        const math::Vector3D half_parent_edge = (parent.max_point - parent_center);
        const math::Vector3D quarter_parent_edge = half_parent_edge * 0.5;

        // Calculate the center of the new child cell
        math::Vector3D child_center;
        child_center.x = (octant_index & 4) ? parent_center.x + quarter_parent_edge.x
                                            : parent_center.x - quarter_parent_edge.x;
        child_center.y = (octant_index & 2) ? parent_center.y + quarter_parent_edge.y
                                            : parent_center.y - quarter_parent_edge.y;
        child_center.z = (octant_index & 1) ? parent_center.z + quarter_parent_edge.z
                                            : parent_center.z - quarter_parent_edge.z;

        // The child's min/max are its center +/- its half-edge (which is quarter_parent_edge)
        math::Vector3D child_min = child_center - quarter_parent_edge;
        math::Vector3D child_max = child_center + quarter_parent_edge;

        parent.children[octant_index] = std::make_unique<BarnesHutNode>(child_max, child_min);
    }

    insertRecursive(*parent.children[octant_index], particle_index, system);
}

void insertRecursive(BarnesHutNode& node,
                     size_t new_particle_index,
                     const math::Vector3D& new_particle_pos,
                     const data::System& system) {
    // If the node is an empty leaf, place the particle here.
    if (node.num_particles == 0) {
        node.particle_index = new_particle_index;
        node.num_particles = 1;  // It now has one particle
        return;
    }

    // If the node is a leaf with a particle, we must subdivide.
    if (node.num_particles == 1) {
        // Check for coincident particles.
        if (system.positions[node.particle_index] == new_particle_pos) {
            constexpr double pert_scale = 1e-6;
            const double pert_val = node.edge_length_sqr > 0
                                        ? std::sqrt(node.edge_length_sqr) * pert_scale
                                        : 1e-9;  // Fallback for zero-size nodes

            // Create a unique perturbation vector based on the particle's index
            math::Vector3D perturbation;
            switch (new_particle_index % 3) {
                case 0:
                    perturbation.x = pert_val;
                    break;
                case 1:
                    perturbation.y = pert_val;
                    break;
                default:
                    perturbation.z = pert_val;
                    break;
            }

            // Recursively call this function again for the new particle, but with the
            // perturbed position. The original particle remains untouched.
            insertRecursive(node, new_particle_index, new_particle_pos + perturbation, system);
            return;
        }

        // The node is a leaf, but the particles are not coincident.
        // Move the original particle down into a child.
        const math::Vector3D center = (node.max_point + node.min_point) * 0.5;
        const int old_idx = getOctantIndex(system.positions[node.particle_index], center);
        createAndInsertIntoChild(node, old_idx, node.particle_index, system);

        node.particle_index = BarnesHutNode::NO_PARTICLE;
    }

    // Insert the new particle into the correct child octant using its geometric position.
    const math::Vector3D center = (node.max_point + node.min_point) * 0.5;
    const int new_idx = getOctantIndex(new_particle_pos, center);
    createAndInsertIntoChild(node, new_idx, new_particle_index, system);

    node.num_particles++;
}

void insertRecursive(BarnesHutNode& node, size_t new_particle_index, const data::System& system) {
    insertRecursive(node, new_particle_index, system.positions[new_particle_index], system);
}

}  // End of anonymous namespace

BarnesHutTree::BarnesHutTree() : root_(std::make_unique<BarnesHutNode>()) {}
BarnesHutTree::~BarnesHutTree() = default;
BarnesHutTree::BarnesHutTree(BarnesHutTree&&) noexcept = default;
BarnesHutTree& BarnesHutTree::operator=(BarnesHutTree&&) noexcept = default;

void BarnesHutTree::build(const data::System& system) {
    if (system.count() == 0) {
        root_ = nullptr;
        return;
    }

    math::Vector3D max_p = system.positions[0];
    math::Vector3D min_p = system.positions[0];
    for (size_t i = 1; i < system.count(); ++i) {
        const auto& pos = system.positions[i];
        max_p.x = std::max(pos.x, max_p.x);
        max_p.y = std::max(pos.y, max_p.y);
        max_p.z = std::max(pos.z, max_p.z);
        min_p.x = std::min(pos.x, min_p.x);
        min_p.y = std::min(pos.y, min_p.y);
        min_p.z = std::min(pos.z, min_p.z);
    }

    const math::Vector3D extent = max_p - min_p;
    double max_edge = std::max({extent.x, extent.y, extent.z});
    max_edge *= 1.1;  // Add a small buffer to prevent particles on the boundary

    math::Vector3D center = (max_p + min_p) * 0.5;
    math::Vector3D half_edge = {max_edge * 0.5, max_edge * 0.5, max_edge * 0.5};

    math::Vector3D root_max = center + half_edge;
    math::Vector3D root_min = center - half_edge;
    root_ = std::make_unique<BarnesHutNode>(root_max, root_min);

    // Insert all particles into the tree.
    for (size_t i = 0; i < system.count(); ++i) {
        insertRecursive(*root_, i, system);
    }

    // Calculate the mass distribution of the tree.
    updateMassRecursive(*root_, system);
}

double BarnesHutTree::updateForces(const data::System& system,
                                   double theta_mac_sqr,
                                   double softening_sqr,
                                   std::vector<math::Vector3D>& out_acc) const {
    const size_t particle_count = system.count();
    if (!root_ || particle_count == 0) return 0.0;

    // Reset accelerations to zero
    std::fill(out_acc.begin(), out_acc.end(), math::Vector3D{});

    double total_potential_energy = 0.0;

    for (size_t i = 0; i < particle_count; ++i) {
        double particle_potential_energy = 0.0;

        out_acc[i] = sumForcesRecursive(*root_,
                                        system.positions[i],
                                        system.masses[i],
                                        i,
                                        particle_potential_energy,
                                        system,
                                        theta_mac_sqr,
                                        softening_sqr);

        total_potential_energy += particle_potential_energy;
    }

    // Each pair (i,j) was counted twice, so we must divide by 2.
    return total_potential_energy * 0.5;
}

}  // namespace enkas::simulation
