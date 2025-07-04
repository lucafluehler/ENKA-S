#ifndef FILE_CHECKER_H_
#define FILE_CHECKER_H_

#include <QObject>

enum class FileType
{
    Settings,
    InitialSystem,
    RenderData,
    DiagnosticsData,
    AnalyticsData
};

class FileChecker : public QObject
{
    Q_OBJECT

public:
    FileChecker(QObject *parent = nullptr);
    ~FileChecker();

public slots:
    void checkFiles(const QVector<FileType>& files, const QString& dir_path);

    void checkSettingsFile(const QString& file_path);
    void checkInitialSystemFile(const QString& file_path);
    void checkRenderFile(const QString& file_path);
    void checkDiagnosticsFile(const QString& file_path);
    void checkAnalyticsFile(const QString& file_path);

signals:
    void filesChecked();
    void fileChecked(FileType type, const QString& path, bool result);

private:

};

#endif // FILE_CHECKER_H_
