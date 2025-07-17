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

class LoadSimulationTab : public QWidget, public ILoadSimulationView {
    Q_OBJECT

public:
    explicit LoadSimulationTab(QWidget *parent = nullptr);

    void updateInitialSystemPreview() override;
    void onSettingsParsed(bool success) override;
    void onInitialSystemParsed(std::optional<enkas::data::System> system) override;
    void onDiagnosticsSeriesParsed(bool success) override;
    QVector<QString> getFilesToCheck() const override;

signals:
    void requestFilesCheck();

private slots:
    void openFolderDialog();

private:
    void resetSimulationFilePaths();
    void checkFiles(const QString &dir_path);

    Ui::LoadSimulationTab *ui_;

    QString settings_file_path_;
    QString system_file_path_;
    QString diagnostics_file_path_;
};
