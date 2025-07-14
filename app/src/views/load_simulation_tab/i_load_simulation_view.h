#pragma once

#include <QString>

#include "../../core/file_types.h"

class ILoadSimulationView {
public:
    virtual ~ILoadSimulationView() = default;

    virtual void updatePreview() = 0;
    virtual void onFileChecked(const FileType& file, const QString& path, bool result) = 0;
};
