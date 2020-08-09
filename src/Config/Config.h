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

#include <miniconf.h>

#include <utils/utils.h>

// TODO: add documentation

namespace faces {

#define FACES_ADD_CONFIG_OPTION(name, shortFlag, defaultVal, valueRequired, valueDescription) \
        Config::getConfig().option(name).shortflag(shortFlag).defaultValue(defaultVal).required(valueRequired).description(valueDescription);

#define FACES_AUGMENT_CONFIG(name, ...) \
        inline static int __config_augment_dummy_fn_ ## name() { \
            __VA_ARGS__ \
            return 0; \
        } \
        inline static int __config_augment_dummy_var_ ## name = __config_augment_dummy_fn_ ## name();


    class Config {
    public:
        static Config &getInstance() {
            static Config instance;
            return instance;
        }

        static miniconf::Config &getConfig() {
            return _config;
        }

        Config(Config const &) = delete;

        void operator=(Config const &) = delete;

    protected:
        Config() = default;

        inline static miniconf::Config _config = miniconf::Config();

    };

    FACES_AUGMENT_CONFIG(base,
                         FACES_ADD_CONFIG_OPTION("dataDirectory", "data", FACES_ROOT_DIRECTORY "/data",
                                 false, "A path to a directory containing all the data")
    )

}

#endif //FACES_CONFIG_H
