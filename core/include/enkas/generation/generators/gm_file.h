#pragma once

#include <string>
#include <vector>

#include "generator.h"
#include "utils.h"

class GM_File : public Generator
{
public:
    struct Settings
    {
        std::string system_data_path;

        bool isValid() const
        {
            return system_data_path != "";
        }
    };

public:
    GM_File(const Settings& settings);

    utils::InitialSystem createSystem() override;

private:
    Settings settings;
};
