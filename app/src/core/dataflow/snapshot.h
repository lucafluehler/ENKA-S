#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <memory>

template <typename T>
struct Timed {
    std::shared_ptr<T> data;
    double time;

    Timed(std::shared_ptr<T> d, double t = 0.0) : data(std::move(d)), time(t) {}
    Timed(T d, double t = 0.0) : data(std::make_shared<T>(std::move(d))), time(t) {}
};

using SystemSnapshot = const Timed<enkas::data::System>;
using DiagnosticsSnapshot = const Timed<enkas::data::Diagnostics>;

using DiagnosticsSeries = std::vector<Timed<enkas::data::Diagnostics>>;

using SystemSnapshotPtr = std::shared_ptr<const Timed<enkas::data::System>>;
using DiagnosticsSnapshotPtr = std::shared_ptr<const Timed<enkas::data::Diagnostics>>;
