/**
 * @file Config.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 09 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_CONFIG_H
#define FACES_CONFIG_H


#include <spdlog/spdlog.h>

#include <miniconf.h>

#include <utils/utils.h>

// TODO: add documentation

namespace faces {

#define FACES_ADD_CONFIG_OPTION(name, shortFlag, defaultVal, valueRequired, valueDescription) \
        Config::getInstance().config.option(name).shortflag(shortFlag).defaultValue(defaultVal).required(valueRequired).description(valueDescription);

#define FACES_AUGMENT_CONFIG(name, ...) \
        inline static int __config_augment_dummy_fn_ ## name() { \
            __VA_ARGS__ \
            return 0; \
        } \
        inline static int __config_augment_dummy_var_ ## name = __config_augment_dummy_fn_ ## name();


    class Config {
    public:
        miniconf::Config config = miniconf::Config();

        static Config &getInstance() {
            static Config instance;
            return instance;
        }

        /* Accesses the configuration value
         *
         * If the configuration value does not exist, an empty Value object is returned.
         */
        miniconf::Value &operator[](const std::string &flag) {
            return config[flag];
        }

        /* Accesses the configuration value
         *
         * If the configuration value does not exist, std::out_of_range exception is thrown
         */
        miniconf::Value const &operator[](const std::string &flag) const {
            return config[flag];
        }

        [[nodiscard]] std::string getDataPath(std::string const &name,
                                              std::string const &dataSeparator = "") const {
            try {
                return config["dataDirectory"].getString() + dataSeparator + "/" + config[name].getString();
            } catch (std::out_of_range &e) {
                spdlog::error("Cannot get a data entry with the name '{}' from '{}' in the config!",
                              name, dataSeparator);
                return "";
            }
        }

        [[nodiscard]] std::string getModelPath(std::string const &name,
                                               std::string const &dataSeparator = "") const {
            return getDataPath(name, "/models" + dataSeparator);
        }


        Config(Config const &) = delete;

        void operator=(Config const &) = delete;

    protected:
        Config() = default;

    };

    FACES_AUGMENT_CONFIG(base,
                         FACES_ADD_CONFIG_OPTION("dataDirectory", "data", FACES_ROOT_DIRECTORY "/data",
                                 false, "A path to a directory containing all the data")
    )

}

#endif //FACES_CONFIG_H
