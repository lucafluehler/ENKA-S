#include "simulation_manager.h"
#include "generation_factory.h"
#include "simulation_factory.h"

SimulationManager::SimulationManager( const Settings& settings
                                    , const DataSettings& data_settings
                                    , const GenerationSettings& generation_settings
                                    , const SimulationSettings& simulation_settings
                                    , QObject* parent )
    : QObject(parent)
    , data_settings(data_settings)
    , last_render_update(0.0)
    , last_diagnostics_update(0.0)
    , last_analytics_update(0.0)
    , generator(GenerationFactory::create(generation_settings))
    , simulator(SimulationFactory::create(simulation_settings))
    , duration(simulation_settings.duration)
    , simulation_window(new SimulationWindow)
    , simulation_worker(nullptr)
    , simulation_thread(nullptr)
    , simulation_timer(new QElapsedTimer)
    , data_saver(nullptr)
    , data_saver_thread(nullptr)
    , data_saver_workload(0)
    , pending_time(-42.0)
    , data_ptr(std::make_shared<DataPtr>())
    , aborted(false)
{
    data_ptr->settings = std::make_shared<Settings>(settings);

    // Initialize simulation worker-thread pair
    simulation_worker = new SimulationWorker(generator, simulator);
    simulation_thread = new QThread(this);
    simulation_worker->moveToThread(simulation_thread);
    simulation_thread->start();

    // Signals from Thread to Manager
    connect( simulation_thread, &QThread::finished
            , this, &SimulationManager::threadFinished);

    // Signals from Manager to Worker
    connect( this, &SimulationManager::requestGeneration
           , simulation_worker, &SimulationWorker::startGeneration);
    connect( this, &SimulationManager::requestInitialization
           , simulation_worker, &SimulationWorker::startInitialization);
    connect( this, &SimulationManager::requestSimulationStep
           , simulation_worker, &SimulationWorker::evolveSystem);

    // Signals from Worker to Manager
    connect( simulation_worker, &SimulationWorker::generationCompleted
           , this, &SimulationManager::receivedGenerationCompleted);
    connect( simulation_worker, &SimulationWorker::initializationCompleted
           , this, &SimulationManager::receivedInitializationCompleted);
    connect( simulation_worker, &SimulationWorker::simulationStep
           , this, &SimulationManager::receivedSimulationStep);

    // Signals from Manager to Window
    connect( this, &SimulationManager::renderDataStep
           , simulation_window, &SimulationWindow::renderDataUpdate );
    connect( this, &SimulationManager::diagnosticsDataStep
           , simulation_window, &SimulationWindow::diagnosticsDataUpdate );
    connect( this, &SimulationManager::analyticsDataStep
           , simulation_window, &SimulationWindow::analyticsDataUpdate );

    // Initialize data_saver worker-thread pair
    data_saver = new DataSaver(data_ptr);
    data_saver_thread = new QThread(this);
    data_saver->moveToThread(data_saver_thread);
    data_saver_thread->start();

    // Signals from Thread to Manager
    connect( data_saver_thread, &QThread::finished
           , this, &SimulationManager::threadFinished);

    // Signals from Worker to Manager
    connect( data_saver, &DataSaver::workFinished
           , this, &SimulationManager::dataSaverFinished);

    // Signals from Manager to Thread
    connect( this, &SimulationManager::saveSettings
           , data_saver, &DataSaver::saveSettings);
    connect( this, &SimulationManager::saveInitialSystem
           , data_saver, &DataSaver::saveInitialSystem);
    connect( this, &SimulationManager::saveRenderData
           , data_saver, &DataSaver::saveRenderData);
    connect( this, &SimulationManager::saveDiagnosticsData
           , data_saver, &DataSaver::saveDiagnosticsData);
    connect( this, &SimulationManager::saveAnalyticsData
           , data_saver, &DataSaver::saveAnalyticsData);
}

SimulationManager::~SimulationManager()
{
    if (!aborted) abortSimulation();

    if (simulation_thread->isRunning()) {
        simulation_thread->quit();
        simulation_thread->wait();
    }

    if (data_saver_thread->isRunning()) {
        data_saver_thread->quit();
        data_saver_thread->wait();
    }
}


void SimulationManager::startSimulationProcedere()
{
    if (data_settings.save_folder) {
        data_saver_workload += 1;
        emit saveSettings();
    }

    simulation_timer->start();
    emit requestGeneration();
}

void SimulationManager::abortSimulation()
{
    aborted = true;
    simulator->requestAbortion();

    if (simulation_window) {
        simulation_window->close();
        simulation_window->deleteLater();
        simulation_window = nullptr;
    }

    if (simulation_thread->isRunning()) {
        simulation_thread->quit();
    } else { threadFinished(); }

    if (data_saver_thread->isRunning()) {
        data_saver_thread->quit();
    } else { threadFinished(); }
}

double SimulationManager::getTime() const { return simulator->getGlobalTime(); }

double SimulationManager::getDuration() const { return duration; }


void SimulationManager::openSimulationWindow()
{
    if (!simulation_window) return;

    if (simulation_window->getMode() == SimulationWindow::Mode::Uninitialized)
        simulation_window->initLiveMode(data_ptr, duration);

    simulation_window->show();
}


void SimulationManager::dataSaverFinished()
{
    data_saver_workload -= 1;

    if (data_saver_workload == 0 && pending_time != -42.0) {
        performSimulationStep(pending_time);
        pending_time = -42.0;
    }
}

void SimulationManager::receivedGenerationCompleted()
{
    logTime("Generation");

    if (data_settings.save_folder) {
        data_ptr->initial_system =
            std::make_shared<utils::InitialSystem>(simulation_worker->getInitialSystem());
        data_saver_workload += 1;
        emit saveInitialSystem();
    }

    emit generationCompleted();

    if (aborted) return;

    simulation_timer->start();
    emit requestInitialization();
}

void SimulationManager::receivedInitializationCompleted()
{
    logTime("Initialization");

    emit initializationCompleted();
    if (aborted) return;

    simulation_timer->start();
    emit requestSimulationStep();
}

void SimulationManager::receivedSimulationStep(double time)
{
    if (data_saver_workload == 0) {
        performSimulationStep(time);
    } else {
        pending_time = time;
    }
}

void SimulationManager::threadFinished()
{
    if (data_saver_thread->isFinished() && simulation_thread->isFinished())
        deleteLater();
}


void SimulationManager::performSimulationStep(double time)
{
    emit simulationStep(time);

    if (data_settings.render_step <= time - last_render_update) {
        data_ptr->render_data =
            std::make_shared<RenderData>(simulator->getRenderData());
        emit renderDataStep();
        last_render_update = time;

        if (data_settings.save_render_data) {
            data_saver_workload += 1;
            emit saveRenderData();
        }
    }

    if (data_settings.diagnostics_step <= time - last_diagnostics_update) {
        data_ptr->diagnostics_data =
            std::make_shared<DiagnosticsData>(simulator->getDiagnosticsData());
        emit diagnosticsDataStep();
        last_diagnostics_update = time;

        if (data_settings.save_diagnostics_data) {
            data_saver_workload += 1;
            emit saveDiagnosticsData();
        }
    }

    if (data_settings.analytics_step <= time - last_analytics_update) {
        data_ptr->analytics_data =
            std::make_shared<AnalyticsData>(simulator->getAnalyticsData());
        emit analyticsDataStep();
        last_analytics_update = time;

        if (data_settings.save_analytics_data) {
            data_saver_workload += 1;
            emit saveAnalyticsData();
        }
    }

    if (time <= duration && !aborted) {
        emit requestSimulationStep();
    } else {
        logTime("Simulation");
    }
}

void SimulationManager::logTime(const QString& time_identifier)
{
    qint64 elapsed_time = simulation_timer->elapsed();
    QString ms_txt = QString::number(elapsed_time);
    QString sec_txt = QString::number(elapsed_time/1000.0, 'f', 3);
    QString min_txt = QString::number(elapsed_time/60000.0, 'f', 3);

    qDebug() << time_identifier.toStdString() + " Time: ";
    qDebug() << QString("  %1 ms ").arg(ms_txt);
    qDebug() << QString("  %1 s  ").arg(sec_txt);
    qDebug() << QString("  %1 min").arg(min_txt);
    qDebug();
}
