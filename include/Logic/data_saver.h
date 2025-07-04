#ifndef DATA_SAVER_H_
#define DATA_SAVER_H_

#include <QObject>
#include <QDir>

#include "data_ptr.h"

class DataSaver : public QObject
{
    Q_OBJECT

public:
    DataSaver( const std::shared_ptr<DataPtr>& data_ptr
             , QObject* parent = nullptr );

signals:
    void workFinished();

public slots:
    void saveSettings();
    void saveInitialSystem();
    void saveRenderData();
    void saveDiagnosticsData();
    void saveAnalyticsData();

private:
    void createFolder();
    QString getString(double value, int precision = 15) const;

    std::shared_ptr<DataPtr> data_ptr;
    std::unique_ptr<QDir> output_dir;
};

#endif // DATA_SAVER_H_
