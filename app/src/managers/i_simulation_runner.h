#pragma once

#include <QObject>

class ISimulationRunner : public QObject {
    Q_OBJECT
public:
    explicit ISimulationRunner(QObject* parent = nullptr) : QObject(parent) {}
    ~ISimulationRunner() override = default;

    /**
     * @brief Starts a new simulation.
     */
    virtual void startSimulationProcedure() = 0;

    /**
     * @brief Returns the current simulation time.
     * @return The current time in time step units.
     */
    virtual double getTime() const = 0;

    /**
     * @brief Returns the total duration of the simulation.
     * @return The duration in time step units.
     */
    virtual double getDuration() const = 0;

    /**
     * @brief Opens the simulation window.
     */
    virtual void openSimulationWindow() = 0;

signals:
    /** @signal
     * @brief Emitted when the generation of a new initial system is completed.
     */
    void generationCompleted();

    /** @signal
     * @brief Emitted when the initialization of the simulation is completed.
     */
    void initializationCompleted();
};
