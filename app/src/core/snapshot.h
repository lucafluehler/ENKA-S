#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <QMetaType>
#include <memory>

template <typename T>
struct Timed {
    T data;
    double time;

    Timed(T d, double t = 0.0) : data(std::move(d)), time(t) {}
};

using SystemSnapshot = const Timed<enkas::data::System>;
using DiagnosticsSnapshot = const Timed<enkas::data::Diagnostics>;

using SystemSnapshotPtr = std::shared_ptr<const Timed<enkas::data::System>>;
using DiagnosticsSnapshotPtr = std::shared_ptr<const Timed<enkas::data::Diagnostics>>;
