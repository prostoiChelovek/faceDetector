//
// Created by prostoichelovek on 16.02.2020.
//

#ifndef FACES_CONFIG_H
#define FACES_CONFIG_H


#include <opencv2/opencv.hpp>

#include "Serialization.hpp"

namespace Faces {

    class Config {
    public:
        std::string test;
        int test2;
        int test3, test4;

        constexpr static auto properties = std::make_tuple(
                Serialization::property(&Config::test, "/str"),
                Serialization::property(&Config::test2, "/int"),
                Serialization::property(&Config::test3, "/test/int3"),
                Serialization::property(&Config::test4, "/test/int4")

        );

    };

}

#endif //FACES_CONFIG_H
