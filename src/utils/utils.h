/**
 * @file utils.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains definitions of the common utility functions
 */

#ifndef FACES_UTILS_H
#define FACES_UTILS_H

#include <map>
#include <iostream>
#include <functional>


namespace faces {

    /**
     * This namespace contains classes and functions needed to create factories of different classes.
     * It allows us to create subclasses by their names
     *
     * @see https://stackoverflow.com/a/582456/9577873
     */
    namespace factory {
        /**
         * This is a function which is used to create an instance of the given type.
         * It is necessary, because functions can be stored in containers unlike type names.
         *
         * @tparam BaseT - a type of a base [abstract] class of DerivedT
         * @tparam DerivedT - type to return an instance of
         * @return a pointer to an instance of DerivedT
         */
        template<typename BaseT, typename DerivedT>
        BaseT *createT() {
            return new DerivedT;
        }

        /**
         * A class which contains the register of classes and methods to create them.
         *
         * @tparam BaseT - a common base type of stored classes
         */
        template<typename BaseT>
        class Factory {
        public:
            /// A type of the map, where subclasses will be registered
            typedef std::map<std::string, std::function<BaseT *()>> MapType;

            /**
             * Creates an instance of a registered subclass by its name.
             *
             * @param s - name of the subclass
             * @return a pointer to an instance of a requested subclass OR a `nullptr` if it was not found
             */
            static BaseT *createInstance(std::string const &s) {
                typename MapType::iterator it = getMap().find(s);
                if (it == getMap().end())
                    return nullptr;
                return it->second();
            }

        protected:
            /**
             * @return a static instance of the subclass register
             */
            static MapType &getMap() {
                static MapType map;
                return map;
            }
        };

        /**
         * A helper class used to easily register subclasses
         *
         * @tparam BaseT - a base class of the registered class
         * @tparam DerivedT - a type of the registered class itself
         */
        template<typename BaseT, typename DerivedT>
        class DerivedRegistrar : public Factory<BaseT> {
        public:
            explicit DerivedRegistrar(std::string const &s) {
                this->getMap().insert(std::make_pair(s, &createT<BaseT, DerivedT>));
            }
        };

    }

}

/**
 * @see faces::factory::DerivedRegistrar
 */
#define FACES_REGISTER_SUBCLASS(base, subclass, name) \
        inline static faces::factory::DerivedRegistrar<base, subclass> __registrar_instance{name};

/**
 * @see faces::factory::Factory::createInstance
 */
#define FACES_CREATE_INSTANCE_NAME(base, name) \
        faces::factory::Factory<base>::createInstance(name)


#endif //FACES_UTILS_H
