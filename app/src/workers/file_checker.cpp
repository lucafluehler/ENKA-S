#include <QDir>
#include "file_checker.h"
#include "settings.h"

FileChecker::FileChecker(QObject *parent)
    : QObject(parent)
{
    
}

FileChecker::~FileChecker()
{
    
}

void FileChecker::checkFiles(const QVector<FileType>& files, const QString& dir_path)
{
    for (const auto& file : files) {
        QString file_path;

        switch (file) {
        case FileType::Settings:
            file_path = QDir(dir_path).filePath("settings.csv");
            checkSettingsFile(file_path);
            break;
        case FileType::InitialSystem:
            file_path = QDir(dir_path).filePath("initial_system.csv");
            checkInitialSystemFile(file_path);
            break;
        case FileType::RenderData:
            file_path = QDir(dir_path).filePath("render_data.csv");
            checkRenderFile(file_path);
            break;
        case FileType::DiagnosticsData:
            file_path = QDir(dir_path).filePath("diagnostics_data.csv");
            checkDiagnosticsFile(file_path);
            break;
        case FileType::AnalyticsData:
            file_path = QDir(dir_path).filePath("analytics_data.csv");
            checkAnalyticsFile(file_path);
            break;
        default: break;
        }
    }

    emit filesChecked();
}


void FileChecker::checkSettingsFile(const QString& file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileChecked(FileType::Settings, file_path, false);
        return;
    }

    QTextStream in(&file);

    Settings settings_template;

    // Remove illegal duplicates in header
    QStringList csv_identifiers = in.readLine().split(",");
    csv_identifiers.removeDuplicates();

    // Check if header and values match in non-zero size
    QStringList csv_values = in.readLine().split(",");
    if (   csv_identifiers.isEmpty() || csv_values.isEmpty()
        || csv_identifiers.size() != csv_values.size() ) {
        emit fileChecked(FileType::Settings, file_path, false);
        return;
    }

    // Check corresponding values
    for (size_t i = 0; i < csv_identifiers.size(); ++i) {
        if (!settings_template.setValue(csv_identifiers.at(i), csv_values.at(i))) {
            emit fileChecked(FileType::Settings, file_path, false);
            return;
        }
    }

    emit fileChecked(FileType::Settings, file_path, true);
}

void FileChecker::checkInitialSystemFile(const QString& file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileChecked(FileType::InitialSystem, file_path, false);
        return;
    };

    QTextStream in(&file);

    // Check header
    const QString c_EXPECTED_HEADER = "pos_x,pos_y,pos_z,vel_x,vel_y,vel_z,mass";

    QString header = in.readLine();
    if (header != c_EXPECTED_HEADER) {
        emit fileChecked(FileType::InitialSystem, file_path, false);
        return;
    }

    // Check body
    while (!in.atEnd()) {
        QStringList fields = in.readLine().split(',');

        if (fields.size() != c_EXPECTED_HEADER.split(",").size()) {
            emit fileChecked(FileType::InitialSystem, file_path, false);
            return;
        }

        for (const QString& field : fields) {
            bool conversion_flag;
            field.toDouble(&conversion_flag);
            if (!conversion_flag) {
                emit fileChecked(FileType::InitialSystem, file_path, false);
                return;
            }
        }
    }

    emit fileChecked(FileType::InitialSystem, file_path, true);
}

void FileChecker::checkRenderFile(const QString& file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileChecked(FileType::RenderData, file_path, false);
        return;
    }

    QTextStream in(&file);

    // Check header
    const QString c_EXPECTED_HEADER = "time,pos_x,pos_y,pos_z";

    QString header = in.readLine();
    if (header != c_EXPECTED_HEADER) {
        emit fileChecked(FileType::RenderData, file_path, false);
        return;
    }

    // Check body
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() != c_EXPECTED_HEADER.split(",").size()) {
            emit fileChecked(FileType::RenderData, file_path, false);
            return;
        }

        for (const QString& field : fields) {
            bool conversion_flag;
            field.toDouble(&conversion_flag);
            if (!conversion_flag) {
                emit fileChecked(FileType::RenderData, file_path, false);
                return;
            }
        }
    }

    emit fileChecked(FileType::RenderData, file_path, true);
}

void FileChecker::checkDiagnosticsFile(const QString& file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileChecked(FileType::DiagnosticsData, file_path, false);
        return;
    };

    QTextStream in(&file);

    // Check header
    const QString c_EXPECTED_HEADER = "time,e_kin,e_pot,L_tot,"
                                      "com_pos_x,com_pos_y,com_pos_z,"
                                      "com_vel_x,com_vel_y,com_vel_z,"
                                      "r_vir,ms_vel,t_cr";

    QString header = in.readLine();
    if (header != c_EXPECTED_HEADER) {
        emit fileChecked(FileType::DiagnosticsData, file_path, false);
        return;
    }

    // Check body
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() != c_EXPECTED_HEADER.split(",").size()) {
            emit fileChecked(FileType::DiagnosticsData, file_path, false);
            return;
        }

        for (const QString& field : fields) {
            bool conversion_flag;
            field.toDouble(&conversion_flag);
            if (!conversion_flag) {
                emit fileChecked(FileType::DiagnosticsData, file_path, false);
                return;
            }
        }
    }

    emit fileChecked(FileType::DiagnosticsData, file_path, true);
}

void FileChecker::checkAnalyticsFile(const QString& file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileChecked(FileType::AnalyticsData, file_path, false);
        return;
    };

    QTextStream in(&file);

    // Check header
    const QString c_EXPECTED_HEADER = "time,10_percent_mass_radius,25_percent_mass_radius,"
                                      "half_mass_radius,75_percent_mass_radius,"
                                      "90_percent_mass_radius,t_rh,t_dh";

    QString header = in.readLine();
    if (header != c_EXPECTED_HEADER) {
        emit fileChecked(FileType::AnalyticsData, file_path, false);
        return;
    }

    // Check body
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() != c_EXPECTED_HEADER.split(",").size()) {
            emit fileChecked(FileType::AnalyticsData, file_path, false);
            return;
        }

        for (const QString& field : fields) {
            bool conversion_flag;
            field.toDouble(&conversion_flag);
            if (!conversion_flag) {
                emit fileChecked(FileType::AnalyticsData, file_path, false);
                return;
            }
        }
    }

    emit fileChecked(FileType::AnalyticsData, file_path, true);
}
