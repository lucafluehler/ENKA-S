#pragma once

#include "geometric_algebra.h"

struct DiagnosticsData
{
    double time;            // time stamp
    double e_kin;           // total kinetic energy
    double e_pot;           // total potential energy
    double L_tot;           // total magnitude of angular momentum
    ga::Vector3D com_pos;   // center of mass position
    ga::Vector3D com_vel;   // center of mass velocity
    double r_vir;           // virial radius
    double ms_vel;          // mean square velocity
    double t_cr;            // crossing time
};
