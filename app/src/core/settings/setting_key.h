#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

enum class SettingKey {
    // --- General settings ---
    GenerationMethod,
    SimulationMethod,
    Duration,
    SystemDataStep,
    DiagnosticsDataStep,
    SaveSystemData,
    SaveDiagnosticsData,
    SaveSettings,

    // --- Generation method specific settings ---
    // File
    FilePath,
    // Normal Sphere
    NormalSphereSeed,
    NormalSphereParticleCount,
    NormalSpherePositionStdDev,
    NormalSphereVelocityStdDev,
    NormalSphereMassMean,
    NormalSphereMassStdDev,
    // Uniform Cube
    UniformCubeSeed,
    UniformCubeParticleCount,
    UniformCubeSideLength,
    UniformCubeInitialVelocity,
    UniformCubeTotalMass,
    // Uniform Sphere
    UniformSphereSeed,
    UniformSphereParticleCount,
    UniformSphereRadius,
    UniformSphereInitialVelocity,
    UniformSphereTotalMass,
    // Plummer Sphere
    PlummerSphereSeed,
    PlummerSphereParticleCount,
    PlummerSphereRadius,
    PlummerSphereInitialVelocity,
    PlummerSphereTotalMass,
    // Spiral Galaxy
    SpiralGalaxySeed,
    SpiralGalaxyParticleCount,
    SpiralGalaxyNumArms,
    SpiralGalaxyRadius,
    SpiralGalaxyTotalMass,
    SpiralGalaxyTwist,
    SpiralGalaxyBlackHoleMass,
    // Collision Model
    CollisionModelSeed,
    CollisionModelImpactParameter,
    CollisionModelRelativeVelocity,
    CollisionModelParticleCount1,
    CollisionModelSphereRadius1,
    CollisionModelTotalMass1,
    CollisionModelParticleCount2,
    CollisionModelSphereRadius2,
    CollisionModelTotalMass2,

    // --- Simulation method specific settings ---
    // Euler
    EulerTimeStep,
    EulerSoftening,
    // Leapfrog
    LeapfrogTimeStep,
    LeapfrogSoftening,
    // Hermite
    HermiteTimeStep,
    HermiteSoftening,
    // Hits
    HitsTimeStepParam,
    HitsSoftening,
    // Barnes-Hut Leapfrog
    BarnesHutLeapfrogTimeStep,
    BarnesHutLeapfrogThetaMac,
    BarnesHutLeapfrogSoftening,
};

constexpr auto SettingKeyStrings = std::to_array<std::pair<SettingKey, std::string_view>>(
    {// --- General settings ---
     {SettingKey::GenerationMethod, "GenerationMethod"},
     {SettingKey::SimulationMethod, "SimulationMethod"},
     {SettingKey::Duration, "Duration"},
     {SettingKey::SystemDataStep, "SystemDataStep"},
     {SettingKey::DiagnosticsDataStep, "DiagnosticsDataStep"},
     {SettingKey::SaveSystemData, "SaveSystemData"},
     {SettingKey::SaveDiagnosticsData, "SaveDiagnosticsData"},
     {SettingKey::SaveSettings, "SaveSettings"},
     // Generation method specific settings
     // File
     {SettingKey::FilePath, "FilePath"},
     // Normal Sphere
     {SettingKey::NormalSphereSeed, "NormalSphereSeed"},
     {SettingKey::NormalSphereParticleCount, "NormalSphereParticleCount"},
     {SettingKey::NormalSpherePositionStdDev, "NormalSpherePositionStdDev"},
     {SettingKey::NormalSphereVelocityStdDev, "NormalSphereVelocityStdDev"},
     {SettingKey::NormalSphereMassMean, "NormalSphereMassMean"},
     {SettingKey::NormalSphereMassStdDev, "NormalSphereMassStdDev"},
     // Uniform Cube
     {SettingKey::UniformCubeSeed, "UniformCubeSeed"},
     {SettingKey::UniformCubeParticleCount, "UniformCubeParticleCount"},
     {SettingKey::UniformCubeSideLength, "UniformCubeSideLength"},
     {SettingKey::UniformCubeInitialVelocity, "UniformCubeInitialVelocity"},

     {SettingKey::UniformCubeTotalMass, "UniformCubeTotalMass"},
     // Uniform Sphere
     {SettingKey::UniformSphereSeed, "UniformSphereSeed"},
     {SettingKey::UniformSphereParticleCount, "UniformSphereParticleCount"},
     {SettingKey::UniformSphereRadius, "UniformSphereRadius"},
     {SettingKey::UniformSphereInitialVelocity, "UniformSphereInitialVelocity"},
     {SettingKey::UniformSphereTotalMass, "UniformSphereTotalMass"},
     // Plummer Sphere
     {SettingKey::PlummerSphereSeed, "PlummerSphereSeed"},
     {SettingKey::PlummerSphereParticleCount, "PlummerSphereParticleCount"},
     {SettingKey::PlummerSphereRadius, "PlummerSphereRadius"},
     {SettingKey::PlummerSphereInitialVelocity, "PlummerSphereInitialVelocity"},
     {SettingKey::PlummerSphereTotalMass, "PlummerSphereTotalMass"},
     // Spiral Galaxy
     {SettingKey::SpiralGalaxySeed, "SpiralGalaxySeed"},
     {SettingKey::SpiralGalaxyParticleCount, "SpiralGalaxyParticleCount"},
     {SettingKey::SpiralGalaxyNumArms, "SpiralGalaxyNumArms"},
     {SettingKey::SpiralGalaxyRadius, "SpiralGalaxyRadius"},
     {SettingKey::SpiralGalaxyTotalMass, "SpiralGalaxyTotalMass"},
     {SettingKey::SpiralGalaxyTwist, "SpiralGalaxyTwist"},
     {SettingKey::SpiralGalaxyBlackHoleMass, "SpiralGalaxyBlackHoleMass"},
     // Collision Model
     {SettingKey::CollisionModelSeed, "CollisionModelSeed"},
     {SettingKey::CollisionModelImpactParameter, "CollisionModelImpactParameter"},
     {SettingKey::CollisionModelRelativeVelocity, "CollisionModelRelativeVelocity"},
     {SettingKey::CollisionModelParticleCount1, "CollisionModelParticleCount1"},
     {SettingKey::CollisionModelSphereRadius1, "CollisionModelSphereRadius1"},
     {SettingKey::CollisionModelTotalMass1, "CollisionModelTotalMass1"},
     {SettingKey::CollisionModelParticleCount2, "CollisionModelParticleCount2"},
     {SettingKey::CollisionModelSphereRadius2, "CollisionModelSphereRadius2"},
     {SettingKey::CollisionModelTotalMass2, "CollisionModelTotalMass2"},
     // --- Simulation method specific settings ---
     // Euler
     {SettingKey::EulerTimeStep, "EulerTimeStep"},
     {SettingKey::EulerSoftening, "EulerSoftening"},
     // Leapfrog
     {SettingKey::LeapfrogTimeStep, "LeapfrogTimeStep"},
     {SettingKey::LeapfrogSoftening, "LeapfrogSoftening"},
     // Hermite
     {SettingKey::HermiteTimeStep, "HermiteTimeStep"},
     {SettingKey::HermiteSoftening, "HermiteSoftening"},
     // Hits
     {SettingKey::HitsTimeStepParam, "HitsTimeStepParam"},
     {SettingKey::HitsSoftening, "HitsSoftening"},
     // Barnes-Hut Leapfrog
     {SettingKey::BarnesHutLeapfrogTimeStep, "BarnesHutLeapfrogTimeStep"},
     {SettingKey::BarnesHutLeapfrogThetaMac, "BarnesHutLeapfrogThetaMac"},
     {SettingKey::BarnesHutLeapfrogSoftening, "BarnesHutLeapfrogSoftening"}});

[[nodiscard]] constexpr SettingKey stringToSettingKey(std::string_view s) {
    for (auto&& [key, val] : SettingKeyStrings) {
        if (val == s) return key;
    }
    throw std::out_of_range("Unknown SettingKey: \"" + std::string(s) + "\"");
}

[[nodiscard]] constexpr std::string_view settingKeyToString(SettingKey key) {
    for (auto&& [k, v] : SettingKeyStrings) {
        if (k == key) return v;
    }
    throw std::out_of_range("Unhandled SettingKey enum value");
}
