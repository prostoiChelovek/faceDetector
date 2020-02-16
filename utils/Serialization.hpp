//
// Created by prostoichelovek on 16.02.2020.
//

#ifndef FACES_SERIALIZATION_HPP
#define FACES_SERIALIZATION_HPP


// https://stackoverflow.com/a/34165367/9577873

#include <tuple>
#include <map>
#include <iostream>

#include <nlohmann/json.hpp>

#include "utils.hpp"

namespace Faces {
    namespace Serialization {

        // sequence for
        template<typename T, T... S, typename F>
        constexpr void for_sequence(std::integer_sequence<T, S...>, F &&f) {
            using unpack_t = int[];
            (void) unpack_t{(static_cast<void>(f(std::integral_constant<T, S>{})), 0)..., 0};
        }

        template<typename Class, typename T>
        struct PropertyImpl {
            constexpr PropertyImpl(T Class::*aMember, const char *aName) : member{aMember}, path{aName} {}

            using Type = T;

            T Class::*member;
            const char *path;
        };

        // One could overload this function to accept both a getter and a setter instead of a member.
        template<typename Class, typename T>
        constexpr auto property(T Class::*member, const char *path) {
            return PropertyImpl<Class, T>{member, path};
        }


        // unserialize function
        template<typename T>
        T fromJson(const nlohmann::json &data) {
            T object;

            // We first get the number of properties
            constexpr auto nbProperties = std::tuple_size<decltype(T::properties)>::value;

            // We iterate on the index sequence of size `nbProperties`
            for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i) {
                // get the property
                constexpr auto property = std::get<i>(T::properties);

                // get the type of the property
                using Type = typename decltype(property)::Type;

                // set the value to the member
                object.*(property.member) = data[nlohmann::json::json_pointer(property.path)];
            });

            return object;
        }

        template<typename T>
        nlohmann::json toJson(const T &object) {
            nlohmann::json data;

            // We first get the number of properties
            constexpr auto nbProperties = std::tuple_size<decltype(T::properties)>::value;

            // We iterate on the index sequence of size `nbProperties`
            for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i) {
                // get the property
                constexpr auto property = std::get<i>(T::properties);

                // set the value to the member
                data[nlohmann::json::json_pointer(property.path)] = object.*(property.member);
            });

            return data;
        }

    }
}


#endif //FACES_SERIALIZATION_HPP
