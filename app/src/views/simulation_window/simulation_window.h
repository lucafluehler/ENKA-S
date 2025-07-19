#pragma once

#include <QElapsedTimer>
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QVector>

#include "core/snapshot.h"
#include "i_simulation_window_view.h"
#include "rendering/render_settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulationWindow;
}
QT_END_NAMESPACE

class SimulationWindow : public QMainWindow, public ISimulationWindowView {
    Q_OBJECT

public:
    explicit SimulationWindow(QWidget *parent = nullptr);

    void initLiveMode() override;

    void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                               double simulation_duration) override;
    void updateFPS(int fps) override;

public slots:
    void onDiagnosticsDataUpdate();

private slots:
    void saveSettings();
    void toggleSidebar();
    void toggleSettings();
    void toggleMovie(bool checked);

private:
    RenderSettings settings_;

    QTimer *movie_timer;

    Ui::SimulationWindow *ui_;
};
