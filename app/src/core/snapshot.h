#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <memory>

template <typename T>
struct Timed {
    double time;
    T data;

    Timed(double t, T d) : time(t), data(std::move(d)) {}
};

using DiagnosticsSnapshotPtr = std::shared_ptr<const Timed<enkas::data::Diagnostics>>;
using SystemSnapshotPtr = std::shared_ptr<const Timed<enkas::data::System>>;
