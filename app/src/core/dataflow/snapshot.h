#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <memory>
#include <ostream>

template <typename T>
struct Snapshot {
    std::shared_ptr<T> data;
    double time;

    Snapshot() : data(nullptr), time(0.0) {}
    Snapshot(std::shared_ptr<T> d, double t = 0.0) : data(std::move(d)), time(t) {}
    Snapshot(T d, double t = 0.0) : data(std::make_shared<T>(std::move(d))), time(t) {}

    void reset() {
        data.reset();
        time = 0.0;
    }
};

using SystemSnapshot = const Snapshot<enkas::data::System>;
using DiagnosticsSnapshot = const Snapshot<enkas::data::Diagnostics>;

using DiagnosticsSeries = std::vector<Snapshot<enkas::data::Diagnostics>>;

using SystemSnapshotPtr = std::shared_ptr<const Snapshot<enkas::data::System>>;
using DiagnosticsSnapshotPtr = std::shared_ptr<const Snapshot<enkas::data::Diagnostics>>;

// Writer for SystemDataSnapshot
inline std::ostream& operator<<(std::ostream& os, const SystemSnapshot& snapshot) {
    const auto& system = snapshot.data;
    for (size_t i = 0; i < system->count(); ++i) {
        os << snapshot.time << ',' << system->positions[i].x << ',' << system->positions[i].y << ','
           << system->positions[i].z << ',' << system->velocities[i].x << ','
           << system->velocities[i].y << ',' << system->velocities[i].z << ',' << system->masses[i]
           << '\n';
    }
    return os;
}

// Writer for DiagnosticsDataSnapshot
inline std::ostream& operator<<(std::ostream& os, const DiagnosticsSnapshot& snapshot) {
    const auto& diag = snapshot.data;
    os << snapshot.time << ',' << diag->e_kin << ',' << diag->e_pot << ',' << diag->L_tot << ','
       << diag->com_pos.x << ',' << diag->com_pos.y << ',' << diag->com_pos.z << ','
       << diag->com_vel.x << ',' << diag->com_vel.y << ',' << diag->com_vel.z << ',' << diag->r_vir
       << ',' << diag->ms_vel << ',' << diag->t_cr << '\n';
    return os;
}