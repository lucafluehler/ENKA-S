#include <QDateTime>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

#include "data_saver.h"
#include "data_ptr.h"

DataSaver::DataSaver( const std::shared_ptr<DataPtr>& data_ptr
                    , QObject* parent)
    : QObject(parent)
    , data_ptr(data_ptr)
    , output_dir(nullptr)
{}


void DataSaver::saveSettings()
{
    if (!output_dir) createFolder();

    // Create file
    QString file_path = output_dir->filePath("settings.csv");

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    // Write file
    QTextStream out(&file);

    const Settings settings = *data_ptr->settings;

    // Header
    const QStringList identifiers = settings.getIdentifiers();
    QString header = identifiers.join(",");
    out << header << Qt::endl;

    // Body
    QStringList values;
    for (const auto& identifier : identifiers) {
        const Setting& setting = settings.getSetting(identifier);

        QString v = settings.getSetting(identifier).value.toString();
        if (setting.type == Setting::Type::File) v = QFileInfo(v).dir().dirName();
        values << v;
    }
    QString value_line = values.join(",");
    out << value_line << Qt::endl;

    file.close();

    emit workFinished();
}

void DataSaver::saveInitialSystem()
{
    if (!output_dir) createFolder();

    // Create file
    QString file_path = output_dir->filePath("initial_system.csv");

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    // Write file
    QTextStream out(&file);

    const auto initial_system = *data_ptr->initial_system;

    // Header
    out << "pos_x,pos_y,pos_z,vel_x,vel_y,vel_z,mass" << Qt::endl;

    // Body
    for (const auto& particle : initial_system) {
        QStringList values;
        values << getString(particle.pos.x)
               << getString(particle.pos.y)
               << getString(particle.pos.z)
               << getString(particle.vel.x)
               << getString(particle.vel.y)
               << getString(particle.vel.z)
               << getString(particle.mass);
        QString line = values.join(",");

        out << line << Qt::endl;
    }

    file.close();

    emit workFinished();
}

void DataSaver::saveRenderData()
{
    if (!output_dir) createFolder();

    // Create file
    QString file_path = output_dir->filePath("render_data.csv");

    QFile file(file_path);

    // If file is new add a header first
    bool new_file = !QFile::exists(file_path);

    if (!file.open(QIODevice::Append | QIODevice::Text)) return;

    // Write file
    QTextStream out(&file);

    const auto render_data = *data_ptr->render_data;

    // Header
    if (new_file) {
        out << "time,pos_x,pos_y,pos_z" << Qt::endl;
    }

    // Body
    // Center Of Mass
    QStringList center_of_mass;
    center_of_mass << getString(render_data.time)
                   << getString(render_data.com_position.x, 4)
                   << getString(render_data.com_position.y, 4)
                   << getString(render_data.com_position.z, 4);
    QString line = center_of_mass.join(",");
    out << line << Qt::endl;

    // Particles
    for (const auto& particle_pos : render_data.positions) {
        QStringList values;
        values << getString(render_data.time, 4)
               << getString(particle_pos.x, 4)
               << getString(particle_pos.y, 4)
               << getString(particle_pos.z, 4);
        QString line = values.join(",");

        out << line << Qt::endl;
    }

    file.close();

    emit workFinished();
}

void DataSaver::saveDiagnosticsData()
{
    if (!output_dir) createFolder();

    // Create file
    QString file_path = output_dir->filePath("diagnostics_data.csv");

    QFile file(file_path);

    // If file is new add a header first
    bool new_file = !QFile::exists(file_path);

    if (!file.open(QIODevice::Append | QIODevice::Text)) return;

    // Write file
    QTextStream out(&file);

    const auto diagnostics_data = *data_ptr->diagnostics_data;

    // Header
    if (new_file) {
        out << "time,e_kin,e_pot,L_tot,"
               "com_pos_x,com_pos_y,com_pos_z,"
               "com_vel_x,com_vel_y,com_vel_z,"
               "r_vir,ms_vel,t_cr" << Qt::endl;
    }

    // Body
    // Center Of Mass
    QStringList values;
    values << getString(diagnostics_data.time)
           << getString(diagnostics_data.e_kin)
           << getString(diagnostics_data.e_pot)
           << getString(diagnostics_data.L_tot)
           << getString(diagnostics_data.com_pos.x)
           << getString(diagnostics_data.com_pos.y)
           << getString(diagnostics_data.com_pos.z)
           << getString(diagnostics_data.com_vel.x)
           << getString(diagnostics_data.com_vel.y)
           << getString(diagnostics_data.com_vel.z)
           << getString(diagnostics_data.r_vir)
           << getString(diagnostics_data.ms_vel)
           << getString(diagnostics_data.t_cr);
    QString line = values.join(",");

    out << line << Qt::endl;

    file.close();

    emit workFinished();
}

void DataSaver::saveAnalyticsData()
{

    if (!output_dir) createFolder();

    // Create file
    QString file_path = output_dir->filePath("analytics_data.csv");

    QFile file(file_path);

    // If file is new add a header first
    bool new_file = !QFile::exists(file_path);

    if (!file.open(QIODevice::Append | QIODevice::Text)) return;

    // Write file
    QTextStream out(&file);

    const auto data = *data_ptr->analytics_data;

    // Header
    if (new_file) {
        out << "time,10_percent_mass_radius,25_percent_mass_radius,half_mass_radius,"
               "75_percent_mass_radius,90_percent_mass_radius,t_rh,t_dh" << Qt::endl;
    }

    // Body
    QStringList values;
    values << getString(data.time)
           << getString(data.mass_radius_10)
           << getString(data.mass_radius_25)
           << getString(data.half_mass_radius)
           << getString(data.mass_radius_75)
           << getString(data.mass_radius_90)
           << getString(data.t_rh)
           << getString(data.t_dh);
    QString line = values.join(",");

    out << line << Qt::endl;

    file.close();

    emit workFinished();
}


void DataSaver::createFolder()
{
    QString time_stamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString root = QCoreApplication::applicationDirPath();
    QString folder_name = QString("%1/output/ENKAS_%2").arg(root, time_stamp);
    output_dir = std::make_unique<QDir>(folder_name);

    if (!output_dir->exists()) output_dir->mkpath(".");
}

QString DataSaver::getString(double v, int p) const
{
    return QString::number(v, 'f', p);
}
