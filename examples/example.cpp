/**
 * @file example.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains some example/test code used during the development
 */

#include <Face/Face.h>

#include <Detector/IDetector.h>

int main(int argc, char **argv) {
    faces::IDetector *detector = FACES_CREATE_INSTANCE(IDetector, Test, "hi");
    // it does not work: Invalid arguments passed to the constructor of 'Test'
    // faces::IDetector *detector =  faces::factory::Factory<faces::IDetector>::createInstance("Test", "hi");
    if (detector == nullptr) {
        return 1;
    }

    cv::Mat test;
    detector->detect(test);

    return 0;
}