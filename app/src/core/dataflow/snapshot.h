#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <memory>

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
