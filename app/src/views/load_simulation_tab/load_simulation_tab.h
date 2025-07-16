#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVector>
#include <QWidget>

#include "i_load_simulation_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LoadSimulationTab;
}
QT_END_NAMESPACE

class LoadSimulationPresenter;

class LoadSimulationTab : public QWidget, public ILoadSimulationView {
    Q_OBJECT

public:
    explicit LoadSimulationTab(QWidget *parent = nullptr);

signals:
    void requestFilesCheck(const QVector<QString> file_paths);

private slots:
    void updatePreview() override;

    void openFolderDialog();

    void onSettingsParsed(bool success) override;
    void onInitialSystemParsed(std::optional<enkas::data::System> system) override;
    void onDiagnosticsSeriesParsed(bool success) override;

private:
    void resetSimulationFilePaths();
    void checkFiles(const QString &dir_path);

    Ui::LoadSimulationTab *ui_;

    LoadSimulationPresenter *presenter_;

    QString settings_file_path_;
    QString system_file_path_;
    QString diagnostics_file_path_;
};
