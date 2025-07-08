#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

#include <enkas/simulation/simulators/barneshut_tree.h>
#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>

namespace enkas::simulation {

struct BarnesHutNode
{
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
    : max_point(max), min_point(min), children(8)
{
    const math::Vector3D extent = max_point - min_point;
    const double max_edge = std::max({extent.x, extent.y, extent.z});
    edge_length_sqr = max_edge*max_edge;
}

namespace { // Anonymous namespace for helper functions

void updateMassRecursive(BarnesHutNode& node, const data::System& system)
{
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

math::Vector3D sumAccRecursive( const BarnesHutNode& node
                              , const math::Vector3D& target_pos
                              , size_t target_index
                              , const data::System& system
                              , double theta_sqr
                              , double softening_sqr ) {
    // If this is a leaf node with one particle...
    if (node.num_particles == 1) {
        // ...and it's not the target particle itself...
        if (node.particle_index != target_index) {
            const math::Vector3D r = system.positions[node.particle_index] - target_pos;
            const double dist_sq = r.norm2() + softening_sqr;
            return r*(system.masses[node.particle_index]/(dist_sq*std::sqrt(dist_sq)));
        }
        return {}; // Return zero vector if it's the same particle
    }

    // Check the Multipole Acceptance Criterion (MAC)
    const double d_sq = (node.center_of_mass - target_pos).norm2();
    if (node.edge_length_sqr / d_sq < theta_sqr) {
        // Node is far enough away, treat as a single mass
        const math::Vector3D r = node.center_of_mass - target_pos;
        const double dist_sq = r.norm2() + softening_sqr;
        return r*(node.total_mass/(dist_sq*std::sqrt(dist_sq)));
    }

    // Node is too close, recurse into its children
    math::Vector3D acc;
    for (const auto& child : node.children) {
        if (child) {
            acc += sumAccRecursive(*child, target_pos, target_index, system, 
                                   theta_sqr, softening_sqr);
        }
    }

    return acc;
}

int getOctantIndex(const math::Vector3D& position, const math::Vector3D& center) 
{
    int index = 0;
    if (position.x > center.x) index |= 4; // 100
    if (position.y > center.y) index |= 2; // 010
    if (position.z > center.z) index |= 1; // 001
    return index;
}

// Forward declare for mutual recursion
void insertRecursive(BarnesHutNode& node, size_t particle_index, const data::System& system);

void createAndInsertIntoChild(BarnesHutNode& parent, int octant_index, 
                              size_t particle_index, const data::System& system) {
    if (!parent.children[octant_index]) {
        const math::Vector3D center = (parent.max_point + parent.min_point)*0.5;
        const double child_edge_length = (parent.max_point.x - center.x);
        
        math::Vector3D child_min = center;
        math::Vector3D child_max = parent.max_point;
        // This logic can be simplified by calculating the new center and half-edge
        math::Vector3D offset;
        offset.x = ((octant_index & 4) ? 1 : -1) * child_edge_length/2.0;
        offset.y = ((octant_index & 2) ? 1 : -1) * child_edge_length/2.0;
        offset.z = ((octant_index & 1) ? 1 : -1) * child_edge_length/2.0;
        
        math::Vector3D child_center = center + offset;
        math::Vector3D half_child_edge = child_edge_length*0.5*math::Vector3D{1.0, 1.0, 1.0};
        child_min = child_center - half_child_edge;
        child_max = child_center + half_child_edge;

        parent.children[octant_index] = std::make_unique<BarnesHutNode>(child_max, child_min);
    }
    insertRecursive(*parent.children[octant_index], particle_index, system);
}

void insertRecursive(BarnesHutNode& node, size_t new_particle_index, const data::System& system) {
    // If the node is not empty, we need to decide where the new particle goes.
    if (node.num_particles > 0) {
        // If it's a leaf, we must move the existing particle down.
        if (node.num_particles == 1) {
            // Handle coincident particles to prevent infinite recursion.
            if (system.positions[node.particle_index] == system.positions[new_particle_index]) {
                // For simplicity, we can just "stack" the new particle here without
                // further subdivision. The mass properties will be correct.
                node.num_particles++;
                return;
            }
            const math::Vector3D center = (node.max_point + node.min_point)*0.5;
            const int old_idx = getOctantIndex(system.positions[node.particle_index], center);
            createAndInsertIntoChild(node, old_idx, node.particle_index, system);
            node.particle_index = BarnesHutNode::NO_PARTICLE; // No longer a leaf
        }

        // Now, insert the new particle into the correct child octant.
        const math::Vector3D center = (node.max_point + node.min_point)*0.5;
        const int new_idx = getOctantIndex(system.positions[new_particle_index], center);
        createAndInsertIntoChild(node, new_idx, new_particle_index, system);
    }
    // If the node is an empty leaf, place the particle here.
    else {
        node.particle_index = new_particle_index;
        node.num_particles++;
    }
}

} // End of anonymous namespace

BarnesHutTree::BarnesHutTree() 
    : root_(std::make_unique<BarnesHutNode>())
{}
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
    
    math::Vector3D center = (max_p + min_p)*0.5;
    double edge_length = (max_p - min_p).norm();
    edge_length *= 1.1; // Add a small buffer
    
    math::Vector3D half_edge = edge_length*0.5*math::Vector3D{1.0, 1.0, 1.0};

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

void BarnesHutTree::updateForces( const data::System& system
                                , double theta_mac_sqr
                                , double softening_sqr
                                , std::vector<math::Vector3D>& out_acc) const 
{
    const size_t particle_count = system.count();
    if (!root_ || particle_count == 0) return;

    // Reset accelerations to zero
    std::fill(out_acc.begin(), out_acc.end(), math::Vector3D{});

    for (size_t i = 0; i < particle_count; ++i) {
        out_acc[i] = sumAccRecursive(*root_, system.positions[i], i, system, theta_mac_sqr, softening_sqr);
    }
}

} // namespace enkas::simulation
