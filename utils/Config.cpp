//
// Created by prostoichelovek on 16.02.2020.
//

#include "Config.h"

Faces::Config::Config(const std::string &config_file)
        : config_file(config_file) {
    config_fs.open(config_file, std::ios::in | std::ios::out);
    if (!config_fs) {
        log(WARNING, "Cannot open configuration file", config_file, ", creating it");
        config_fs.open(config_file, std::ios::out);
        if (!config_fs) {
            log(ERROR, "Cannot create config file ", config_file);
        } else {
            config_fs.close();
            config_fs.open(config_file, std::ios::in | std::ios::out);
        }
    }
    load();
}

nlohmann::json Faces::Config::serialize() {
    return Serialization::toJson(*this);
}

void Faces::Config::deserialize(const nlohmann::json &data) {
    Serialization::fromJson<Config>(this, data);
}

bool Faces::Config::save() {
    if (config_fs) {
        nlohmann::json json = serialize();
        config_fs << std::setw(4) << json << std::endl;
        return true;
    }
    log(ERROR, "Cannot save config because file is not open");
    return false;
}

bool Faces::Config::load() {
    if (config_fs) {
        nlohmann::json json;
        config_fs >> json;
        deserialize(json);

        prefix_paths();

        return true;
    }
    log(ERROR, "Cannot load config because file is not open");
    return false;
}

void Faces::Config::prefix_paths() {
    constexpr auto nbProperties = std::tuple_size<decltype(properties)>::value;
    Serialization::for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i) {
        constexpr auto property = std::get<i>(properties);

        std::vector<std::string> splitted = split(property.path, "/");
        if (std::find(splitted.begin(), splitted.end(), "paths") != splitted.end()
            && std::find(splitted.begin(), splitted.end(), "data_directory") == splitted.end()) {
            this->*(property.member) = data_directory + "/" + this->*(property.member);
        }
    });
}
