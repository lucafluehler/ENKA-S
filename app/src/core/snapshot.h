#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <QMetaType>
#include <memory>

template <typename T>
struct Timed {
    double time;
    T data;

    Timed(double t, T d) : time(t), data(std::move(d)) {}
};

using SystemSnapshot = const Timed<enkas::data::System>;
using DiagnosticsSnapshot = const Timed<enkas::data::Diagnostics>;

using SystemSnapshotPtr = std::shared_ptr<const Timed<enkas::data::System>>;
using DiagnosticsSnapshotPtr = std::shared_ptr<const Timed<enkas::data::Diagnostics>>;

// Q_DECLARE_METATYPE(SystemSnapshotPtr)
// Q_DECLARE_METATYPE(DiagnosticsSnapshotPtr)
