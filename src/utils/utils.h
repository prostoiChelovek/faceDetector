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

#include "factory.hpp"

/**
 * A signature of the getter for the class attribute
 */
#define _FACES_ATTRIBUTE_GETTER_SIGNATURE(type, name) [[nodiscard]] virtual type &get ## _ ## name()

/**
 * A name of the class attribute.
 * It is necessary because we do not want someone to use them directly
 */
#define _FACES_GET_ATTRIBUTE_NAME(name) __faces_attrubute_ ## name

/**
 * Declare a class property and a getter for it.
 * It should be used in base classes to allow their subclasses to override the attribute value. \n
 * In case, if subclass has not overridden the attribute with @ref FACES_OVERRIDE_ATTRIBUTE, the getter will assert.
 * Generated getter with name `get_[name]` should be used to access the attribute.
 *
 * @param type - type of the attribute
 * @param name - name of the attribute
 */
#define FACES_DECLARE_ATTRIBUTE(type, name) \
        type _FACES_GET_ATTRIBUTE_NAME(name); \
        _FACES_ATTRIBUTE_GETTER_SIGNATURE(type, name) \
                { assert(false && "the property " #name " was not overridden in the derived class!"); }

/**
 * Override the attribute value of the base class. \n
 * It should be used in children of classes which using @ref FACES_DECLARE_ATTRIBUTE.
 * This macro adds an attribute with the name and type of the base class`s attribute,
 * and overrides a getter for it, so it won`t assert
 *
 * @param name - name of the attribute (the same as given in FACES_DECLARE_ATTRIBUTE)
 * @param ...  - arguments to pass to the constructor of the attribute
 */
#define FACES_OVERRIDE_ATTRIBUTE(name, ...) \
        using __faces_prop_type_ ## name = decltype(_FACES_GET_ATTRIBUTE_NAME(name)); \
        __faces_prop_type_ ## name _FACES_GET_ATTRIBUTE_NAME(name) = __faces_prop_type_ ## name(__VA_ARGS__); \
        _FACES_ATTRIBUTE_GETTER_SIGNATURE(__faces_prop_type_ ## name, name) override \
                { return _FACES_GET_ATTRIBUTE_NAME(name); }


namespace faces {

}

#endif //FACES_UTILS_H
