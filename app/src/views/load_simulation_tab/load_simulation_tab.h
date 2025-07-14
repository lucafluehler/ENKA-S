#pragma once

#include <QThread>
#include <QTimer>
#include <QWidget>

#include "../../core/file_types.h"
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
    void requestFilesCheck(const QVector<FileType> files, const QString &file_path);

private slots:
    void updatePreview() override;

    void openFolderDialog();

    void onFileChecked(const FileType &file, const QString &path, bool result) override;

    void openSettings();
    void openInitialSystem();
    void run();

private:
    void resetSimulationFilePaths();
    void loadSettings(const QString &file_path);
    void checkFiles(const QString &dir_path);

    Ui::LoadSimulationTab *ui_;

    LoadSimulationPresenter *presenter_;
};
