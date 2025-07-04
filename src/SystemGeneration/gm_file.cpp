#include <QFile>
#include <QTextStream>

#include <vector>

#include "gm_file.h"
#include "utils.h"

GM_File::GM_File(const Settings& settings)
    : settings(settings)
{}

utils::InitialSystem GM_File::createSystem()
{
    utils::InitialSystem initial_system;

    QFile file(QString::fromStdString(settings.system_data_path));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return initial_system;

    QTextStream in(&file);

    // Skip header
    in.readLine();

    // Load particles one by one :)
    while (!in.atEnd()) {
        QStringList fields = in.readLine().split(',');

        utils::BaseParticle particle;

        particle.pos.x = fields.at(0).toDouble();
        particle.pos.y = fields.at(1).toDouble();
        particle.pos.z = fields.at(2).toDouble();

        particle.vel.x = fields.at(3).toDouble();
        particle.vel.y = fields.at(4).toDouble();
        particle.vel.z = fields.at(5).toDouble();

        particle.mass = fields.at(6).toDouble();

        initial_system.push_back(particle);
    }

    initial_system.shrink_to_fit();

    return initial_system;
}
