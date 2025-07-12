#pragma once

#include <QString>
#include <QMap>
#include <QVariant>

#include <initializer_list>

#include "data_settings.h"
#include "generation_settings.h"
#include "simulation_settings.h"

struct Setting {
    enum class Group {
        Data, Generation, Simulation
    };

    enum class Type {
        Int, Double, Bool, File, GenerationMethod, SimulationMethod
    };

    Group group;
    Type type;
    QVariant value;

    Setting(Group g = Group::Data, Type t = Type::Int, const QVariant& v = QVariant())
        : group(g), type(t), value(v)
    {}

    Setting(std::initializer_list<QVariant> initList) {
        if (initList.size() >= 3) {
            auto it = initList.begin();
            group = static_cast<Group>(it->toInt());
            ++it;
            type = static_cast<Type>(it->toInt());
            ++it;
            value = *it;
        } else {
            throw std::invalid_argument("Initializer list is incomplete.");
        }
    }
};

class Settings
{
public:
    Settings();

    bool setSetting(const QString& identifier, const Setting& setting);
    bool setGroup(const QString& identifier, const Setting::Group& group);
    bool setType(const QString& identifier, const Setting::Type& type);
    bool setValue(const QString& identifier, const QVariant& value);

    QStringList getIdentifiers() const;

    bool isSetting(const QString& identifier) const;
    Setting getSetting(const QString& identifier, bool* is_setting = nullptr) const;

    DataSettings getDataSettings() const;
    GenerationSettings getGenerationSettings() const;
    SimulationSettings getSimulationSettings() const;

    QMap<GenerationMethod, QString> getGenerationMethods() const;
    QMap<SimulationMethod, QString> getSimulationMethods() const;

private:
    bool isConvertible(const QVariant& value, const Setting::Type& type);

    QMap<QString, Setting> settings;
};
