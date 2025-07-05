#pragma once

#include "utils.h"

#include <vector>
#include <memory>

struct BHParticle : public utils::BaseParticle
{
    ga::Vector3D acc;
};

struct OctantData
{
    int idx = 0;
    ga::Vector3D max_point;
    ga::Vector3D min_point;
    double edge_length = 0.0;
};

struct BHNode
{
    BHNode();
    BHNode( const ga::Vector3D& p_max_point, const ga::Vector3D& p_min_point
          , double p_edge_length );

    void updateMass();
    ga::Vector3D sumAcc(const BHParticle& particle_i, double theta, double softening2 ) const;
    ga::Vector3D calcAcc( const BHParticle& particle_i, const ga::Vector3D& particle_j_pos
                        , double particle_j_mass, double softening2 ) const;
    double sumEpot(const BHParticle& particle_i, double theta, double softening2 ) const;
    double calcEpot( const BHParticle& particle_i, const ga::Vector3D& particle_j_pos
                   , double particle_j_mass, double softening2 ) const;

    ga::Vector3D max_point;
    ga::Vector3D min_point;
    double edge_length = 0.0;

    ga::Vector3D center_of_mass;
    double total_mass = 0.0;
    int num_particles = 0;

    std::vector<std::unique_ptr<BHNode>> children;

    BHParticle* particle = nullptr;
};

class BHTree
{
public:
    BHTree();
    ~BHTree();

    void updateAcclerations( std::shared_ptr<std::vector<BHParticle>> system
                           , double MAC, double soft_param );
    void build(std::shared_ptr<std::vector<BHParticle>> system);
    double getTotalEPot(double MAC, double soft_param) const;

private:
    using System = std::vector<BHParticle>;

private:
    void resetRoot();
    void insert(BHNode& node, BHParticle& particle);
    void addChildAndInsert(BHNode& node, BHParticle& particle);
    OctantData getOctantData(const BHNode& node, const BHParticle& particle) const;

    std::shared_ptr<System> system;
    BHNode root;
};
