#include "bh_tree.h"
#include "utils.h"

BHNode::BHNode()
    : children(8)
{}

BHNode::BHNode( const math::Vector3D& p_max_point, const math::Vector3D& p_min_point
              , double p_edge_length )
    : max_point(p_max_point)
    , min_point(p_min_point)
    , edge_length(p_edge_length)
    , children(8)
{}

void BHNode::updateMass()
{
    if (num_particles == 1) {
        center_of_mass = particle->pos;
        total_mass = particle->mass;
        return;
    }

    for (auto& child: children) {
        if (!child) continue;
        child->updateMass();
        total_mass += child->total_mass;
        center_of_mass += child->center_of_mass*child->total_mass;
    }

    center_of_mass /= total_mass;
}

math::Vector3D BHNode::sumAcc(const BHParticle& particle_i, double theta, double softening2) const {
    if (num_particles == 1) 
        return calcAcc(particle_i, particle->pos, particle->mass, softening2);

    if (edge_length/(center_of_mass - particle_i.pos).norm() <= theta) 
        return calcAcc(particle_i, center_of_mass, total_mass, softening2);

    math::Vector3D acc;
    for (const auto& child: children) {
        if (!child) continue;
        acc += child->sumAcc(particle_i, theta, softening2);
    }
    return acc;
}

math::Vector3D BHNode::calcAcc( const BHParticle& particle_i, const math::Vector3D& particle_j_pos
              , double particle_j_mass, double softening2 ) const
{
    if (&particle_i.pos == &particle_j_pos) return 0;

    math::Vector3D distance_ij = particle_i.pos - particle_j_pos;

    double softened_dist_2 = distance_ij.norm2() + softening2;

    double distance_factor = std::sqrt(softened_dist_2)*softened_dist_2;

    return distance_ij*particle_j_mass/distance_factor*(-1);
}

double BHNode::sumEpot(const BHParticle& particle_i, double theta, double softening2) const {
    if (num_particles == 1) 
        return calcEpot(particle_i, particle->pos, particle->mass, softening2);

    if (edge_length/(center_of_mass - particle_i.pos).norm() <= theta) 
        return calcEpot(particle_i, center_of_mass, total_mass, softening2);

    double e_pot = 0.0;
    for (const auto& child: children) {
        if (!child) continue;
        e_pot += child->sumEpot(particle_i, theta, softening2);
    }
    return e_pot;
}

double BHNode::calcEpot( const BHParticle& particle_i, const math::Vector3D& particle_j_pos
               , double particle_j_mass, double softening2 ) const
{
    if (&particle_i.pos == &particle_j_pos) return 0.0;

    math::Vector3D distance_ij = particle_i.pos - particle_j_pos;

    const double c_SOFT_DIST2 = distance_ij.norm2() + softening2;

    return -(particle_i.mass*particle_j_mass/std::sqrt(c_SOFT_DIST2));
}

//------------------------------------------------------------------------------------------

BHTree::BHTree()
    : system(nullptr)
{}

BHTree::~BHTree()
{
    system = nullptr;
}


void BHTree::updateAcclerations(std::shared_ptr<System> system, double MAC, double soft_param)
{
    if (!system) return;
    build(system);

    for (auto& particle : *system) {
        particle.acc = root.sumAcc(particle, MAC, soft_param);
    }
}

double BHTree::getTotalEPot(double MAC, double soft_param) const
{
    if (!system) return 0.0;

    double e_pot = 0.0;
    for (auto& particle : *system) {
        e_pot += root.sumEpot(particle, MAC, soft_param);
    }
    return e_pot;
}


void BHTree::build(std::shared_ptr<System> p_system)
{
    if (!p_system) return;
    system = p_system;

    // Reset the whole tree by resetting the root
    resetRoot();

    // Build the tree structure
    for (auto& particle : *system) {
        insert(root, particle);
    }

    // Build the mass distribution
    root.updateMass();
}

void BHTree::resetRoot()
{
    math::Vector3D max_point = system->at(0).pos;
    math::Vector3D min_point = system->at(0).pos;

    for (const auto& particle : *system) {
        max_point.x = std::max(particle.pos.x, max_point.x);
        min_point.x = std::min(particle.pos.x, min_point.x);

        max_point.y = std::max(particle.pos.y, max_point.y);
        min_point.y = std::min(particle.pos.y, min_point.y);

        max_point.z = std::max(particle.pos.z, max_point.z);
        min_point.z = std::min(particle.pos.z, min_point.z);
    }

    root.edge_length = 1.1*std::max( std::max( max_point.x - min_point.x
                                             , max_point.y - min_point.y )
                                   , max_point.z - min_point.z);

    math::Vector3D center = (max_point + min_point)/2;

    root.max_point = center + root.edge_length/2;
    root.min_point = center - root.edge_length/2;

    root.center_of_mass = math::Vector3D();
    root.total_mass = 0.0;
    root.num_particles = 0;
    root.particle = nullptr;

    root.children.clear();
    root.children.resize(8);
}

void BHTree::insert(BHNode& node, BHParticle& particle)
{
    if (node.num_particles > 1) {
        addChildAndInsert(node, particle);
    } else if (node.num_particles == 1) {
        addChildAndInsert(node, *node.particle);
        addChildAndInsert(node, particle);
        node.particle = nullptr;
    } else {
        node.particle = &particle;
    }

    node.num_particles++;
}

void BHTree::addChildAndInsert(BHNode& node, BHParticle& particle)
{
    OctantData octant = getOctantData(node, particle);
    auto& child = node.children.at(octant.idx);

    if (!child) {
        child = std::make_unique<BHNode>(octant.max_point, octant.min_point, octant.edge_length);
    }

    insert(*child, particle);
}

OctantData BHTree::getOctantData(const BHNode& node, const BHParticle& particle) const
{
    OctantData octant_data;
    math::Vector3D node_center = (node.max_point + node.min_point)/2;

    // The translation vector defines where the center of the octant is located
    // relative to the center to the center of the node.
    // ergo node_center + translation*stretch = octant_center
    math::Vector3D translation( particle.pos.x > node_center.x ? 1 : -1
                            , particle.pos.y > node_center.y ? 1 : -1
                            , particle.pos.z > node_center.z ? 1 : -1 );

    // Bitmask defining in which octant a particle is located
    if (translation.x == 1) octant_data.idx |= 0b100;
    if (translation.y == 1) octant_data.idx |= 0b010;
    if (translation.z == 1) octant_data.idx |= 0b001;

    translation *= node.edge_length/4;

    octant_data.edge_length = node.edge_length/2;

    octant_data.max_point = node_center + translation + node.edge_length/4;
    octant_data.min_point = node_center + translation - node.edge_length/4;

    return octant_data;
}
