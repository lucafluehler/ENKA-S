#pragma once

#include <QObject>
#include <optional>

#include "../core/file_types.h"
#include "core/file_parse_logic.h"
#include "enkas/data/system.h"
#include "workers/file_parse_worker.h"

class QTimer;
class QThread;
class ILoadSimulationView;
class FileCheckWorker;

class LoadSimulationPresenter : public QObject {
    Q_OBJECT
public:
    explicit LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent = nullptr);
    ~LoadSimulationPresenter();

    void isSelected(bool selected);

public slots:
    void checkFiles(const QVector<FileType>& files, const QString& file_path);

private slots:
    void onTimerTimeout();
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& frame);
    void onDiagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);

private:
    ILoadSimulationView* view_ = nullptr;
    QTimer* preview_timer_ = nullptr;
    FileParseWorker* file_parse_worker_ = nullptr;
    QThread* file_parse_thread_ = nullptr;
};
