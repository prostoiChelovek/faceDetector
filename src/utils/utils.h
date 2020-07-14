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
#include <memory>


namespace faces {

    // TODO: move it to a separate file
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
         *
         * @return a pointer to an instance of DerivedT
         */
        template<typename BaseT, typename DerivedT, typename ...Args>
        BaseT *createT(Args... args) {
            return new DerivedT(args...);
        }

        /**
         * This is a helper struct used to store different FunctionWrapper(s) in a map
         */
        struct FunctionBase {
            virtual ~FunctionBase() = default;
        };

        /**
         * A helper struct used to store a function
         *
         * @tparam ReturnT  - return type of the function
         * @tparam Args     - types of the function`s arguments
         */
        template<typename ReturnT, typename ...Args>
        struct FunctionWrapper : FunctionBase {
            typedef std::function<ReturnT(Args...)> FuncType;

            FuncType f;

            explicit FunctionWrapper(FuncType func) : f(func) {};
        };

        /**
         * A class which contains the register of classes and methods to create them.
         *
         * @tparam BaseT - a common base type of stored classes
         */
        template<typename BaseT>
        class Factory {
        public:
            /// A type of the map, where subclasses will be registered
            typedef std::map<std::string, std::shared_ptr<FunctionBase>> MapType;

            /**
             * Creates an instance of a registered subclass by its name.
             *
             * @param s     - name of the subclass
             * @param args  - arguments to pass to constructor
             *
             * @tparam Args - type of subclass` constructor arguments;
             *                you only have to specify them when types of passed arguments
             *                don`t perfectly match types of constructor`s arguments
             *
             * @throws std::runtime_error - when ...Args types don`t match
             *                              types specified during the registration
             *
             * @return a pointer to an instance of a requested subclass OR a `nullptr` if it was not found
             */
            template<typename ...Args>
            static BaseT *createInstance(std::string const &s, Args ...args) {
                auto it = getMap().find(s);
                if (it == getMap().end())
                    return nullptr;

                auto *wrappedFn = dynamic_cast<FunctionWrapper<BaseT *, Args...> *>(it->second.get());
                if (!wrappedFn) {
                    throw std::runtime_error("Invalid arguments passed to the constructor of '" + s + "'");
                }
                return wrappedFn->f(args...);
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
         * @tparam Args - type of DerivedT`s constructor arguments (can be empty)
         */
        template<typename BaseT, typename DerivedT, typename ...Args>
        class DerivedRegistrar : public Factory<BaseT> {
        public:
            /**
             * This constructor registers a subclass
             * It is necessary to perform the registration in the class body
             *
             * @param name - name of the registered subclass
             */
            explicit DerivedRegistrar(std::string const &name) {
                std::function<BaseT *(Args...)> fn = createT<BaseT, DerivedT, Args...>;
                std::shared_ptr<FunctionBase> fnBase = std::shared_ptr<FunctionBase>(
                        new FunctionWrapper<BaseT *, Args...>(fn));

                this->getMap()[name] = fnBase;
            }
        };

    }

}

// TODO: add documentation

#define FACES_REGISTER_SUBCLASS_NO_ARGS(base, subclass, name) \
        inline static faces::factory::DerivedRegistrar<base, subclass> __registrar_instance{name};

#define FACES_REGISTER_SUBCLASS_WITH_ARGS(base, subclass, name, ...) \
        inline static faces::factory::DerivedRegistrar<base, subclass, __VA_ARGS__> __registrar_instance{name};

#define FACES_GET_5_TH_ARG(arg1, arg2, arg3, arg4, arg5, ...) arg5

#define FACES_REGISTER_SUBCLASS_MACRO_CHOOSER(...) \
        FACES_GET_5_TH_ARG(__VA_ARGS__, FACES_REGISTER_SUBCLASS_WITH_ARGS, FACES_REGISTER_SUBCLASS_NO_ARGS)

#define FACES_REGISTER_SUBCLASS(...) \
        FACES_REGISTER_SUBCLASS_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)


#define FACES_CREATE_INSTANCE_FN(base) faces::factory::Factory<base>::createInstance

#define FACES_CREATE_INSTANCE(base, name) \
        FACES_CREATE_INSTANCE_FN(base)(name)

#define FACES_CREATE_INSTANCE_ARGS(base, name, ...) \
        FACES_CREATE_INSTANCE_FN(base)(name, __VA_ARGS__)

#endif //FACES_UTILS_H
