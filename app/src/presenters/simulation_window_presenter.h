#pragma once

#include <QObject>

class ISimulationWindowView;

class SimulationWindowPresenter : public QObject {
    Q_OBJECT
public:
    explicit SimulationWindowPresenter(ISimulationWindowView* view, QObject* parent = nullptr);
    ~SimulationWindowPresenter();

private:
    ISimulationWindowView* view_;
};
