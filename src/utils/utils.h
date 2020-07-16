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
#include <type_traits>


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
         * A class which contains the register of classes, and methods to create them.
         *
         * @tparam BaseT - a common base type of stored classes
         */
        template<typename BaseT>
        class Factory {
        public:
            /// A type of the map, where subclasses will be registered
            typedef std::map<std::string, std::shared_ptr<FunctionBase>> MapType;

            template<typename ...Args>
            using InstanceInitializerT = typename FunctionWrapper<BaseT *, Args...>::FuncType;

            /**
             * Returns an initializer function of a registered subclass by its name.
             *
             * @param s     - name of the subclass
             *
             * @tparam Args - type of subclass` constructor arguments;
             *                they should perfectly match types passed during the registration
             *
             * @throws std::runtime_error - when ...Args types don`t match
             *                              types specified during the registration
             *
             * @return an initializer function of the requested subclass OR a `nullptr` if it was not found
             */
            template<typename ...Args>
            static InstanceInitializerT<Args...> getInstanceInitializer(std::string const &name) {
                auto it = getMap().find(name);
                if (it == getMap().end())
                    return nullptr;

                auto *wrappedFn = dynamic_cast<FunctionWrapper<BaseT *, Args...> *>(it->second.get());
                if (!wrappedFn) {
                    throw std::runtime_error(
                            "Invalid argument types passed to the constructor of '" + name + "'");
                }
                return wrappedFn->f;
            }


            /**
             * A wrapper around getInstanceInitializer,
             * which deduces constructor argument types from the passed function
             *
             * @see getInstanceInitializer for more details
             *
             * @param dummyFn - a function used to deduce the constructor arguments
             */
            template<typename ...Args>
            static auto getInstanceInitializer(std::string const &name, void(*dummyFn)(Args...)) {
                return getInstanceInitializer<Args...>(name);
            }

            /**
             * A wrapper around getInstanceInitializer,
             * which deduces constructor argument types from the passed arguments
             * and calls that function with the given arguments
             *
             * @see getInstanceInitializer for more details
             *
             * @note you have to specify constructor argument types in the template manually,
             *       in case if passed argument types do not match constructor`s
             *
             * @param args - arguments to pass to the constructor
             */
            template<typename ...Args>
            static BaseT *createInstance(std::string const &name, Args ...args) {
                auto initializer = getInstanceInitializer<Args...>(name);
                if (!initializer) {
                    return nullptr;
                }
                return initializer(args...);
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
         * @tparam BaseT    - a base class of the registered class
         * @tparam DerivedT - a type of the registered class itself
         * @tparam Args     - type of DerivedT`s constructor arguments (can be empty)
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
                static_assert(std::is_constructible<DerivedT, Args &&...>::value,
                              "Incorrect DerivedT`s constructor argument types passed");

                std::function<BaseT *(Args...)> fn = createT<BaseT, DerivedT, Args...>;
                std::shared_ptr<FunctionBase> fnBase = std::shared_ptr<FunctionBase>(
                        new FunctionWrapper<BaseT *, Args...>(fn));

                this->getMap()[name] = fnBase;
            }
        };

    }

}

/**
 * Constructs a name for an arguments-dummy function
 *
 * @param base - parent class name; it should not contain any extra parts (like namespace name)
 * @param name - display name of the subclass; it should not contain any spaces or extra characters
 */
#define FACES_GET_DUMMY_FUNCTION_NAME(base, name) __factory_arguments_dummy_ ## base ## _ ## name

/**
 * Defines a helper function used to deduce constructor arguments easily
 */
#define FACES_CREATE_DUMMY_FUNCTION(base, name, ...) \
        static void FACES_GET_DUMMY_FUNCTION_NAME(base, name) (__VA_ARGS__) {}

/**
 * Registers a given subclass with the given name.
 * It creates an argument-dummy method and calls a DerivedRegistrar`s constructor
 *
 * @param base      - name of the parent class; it should not contain any extra parts (eg. namespace name)
 * @param subclass  - name of the registered subclass
 * @param name      - display-name of the subclass, refer to it with;
 *                    it should not contain any spaces or extra characters
 * @param ...       - [optional] types of the constructor arguments (should match exactly)
 *
 * @code
 *      FACES_REGISTER_SUBCLASS(IDetector, TestDetector, Test, std::string const&)
 * @endcode
 */
#define FACES_REGISTER_SUBCLASS(base, subclass, name, ...) \
        FACES_CREATE_DUMMY_FUNCTION(base, name, ##__VA_ARGS__) \
        inline static faces::factory::DerivedRegistrar<base, subclass, ##__VA_ARGS__> __registrar_instance{#name};

/**
 * Registers the constructor argument types of the given subclass.
 * It creates a pointer to the dummy function in the subclass, in the global scope.
 * It is necessary to use that function without knowing the name of the subclass.
 *
 * @code
 *      FACES_REGISTER_SUBCLASS_ARGUMENTS(IDetector, TestDetector, Test)
 * @endcode
 */
#define FACES_REGISTER_SUBCLASS_ARGUMENTS(base, subclass, name) \
    inline constexpr auto *FACES_GET_DUMMY_FUNCTION_NAME(base, name) \
            = subclass::FACES_GET_DUMMY_FUNCTION_NAME(base, name);

/**
 * A helper macro, which expands to the getInstanceInitializer method of the factory
 *
 * @param base - base class for the factory
 */
#define FACES_GET_INITIALIZER_FN(base) faces::factory::Factory<faces::base>::getInstanceInitializer

/**
 * A helper macro, which expands to the createInstance method of the factory.
 * You may use it to easily pass template arguments to the method
 *
 * @code
 *      faces::IDetector *detector = FACES_CREATE_INSTANCE_FN(IDetector)<std::string const&>("Test", "hi");
 * @endcode
 */
#define FACES_CREATE_INSTANCE_FN(base) faces::factory::Factory<faces::base>::createInstance

/**
 * Creates an instance of a subclass of the given base by the given static name
 *
 * @param base - a parent class, create a subclass of
 * @param name - a display name of the subclass;
 *               it should not be any kind of string, but a valid literal to include in the function name
 * @param ...  - [optional] arguments to pass to the constructor of the subclass
 *
 * @code
 *      faces::IDetector *detector = FACES_CREATE_INSTANCE(IDetector, Test, "hi");
 * @endcode
 */
#define FACES_CREATE_INSTANCE(base, name, ...) \
        FACES_GET_INITIALIZER_FN(base)(#name, faces::FACES_GET_DUMMY_FUNCTION_NAME(base, name))(__VA_ARGS__)

/**
 * Creates an instance of a subclass of the given base by the given string name
 *
 * @see FACES_CREATE_INSTANCE for more details
 * @see FACES_CREATE_INSTANCE_FN in case if you had some problems with constructor argument types
 *
 * @param name - a display name of the subclass; it can be any kind of string
 * @param ...  - [optional] arguments to pass to the constructor of the subclass;
 *                          their types should perfectly match the types of the constructor arguments
 *
 * @code
 *      faces::IDetector *detectorD = FACES_CREATE_INSTANCE_DYNAMIC(IDetector, "Test", 42);
 * @endcode
 */
#define FACES_CREATE_INSTANCE_DYNAMIC(base, name, ...) \
        FACES_CREATE_INSTANCE_FN(base)(name, ##__VA_ARGS__)


#endif //FACES_UTILS_H
