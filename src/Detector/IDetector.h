/**
 * @file IDetection.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains an interface for face detectors
 */

#ifndef FACES_IDETECTOR_H
#define FACES_IDETECTOR_H

#include <vector>

#include <opencv2/opencv.hpp>

#include "Face/Face.h"
#include "utils/utils.h"

namespace faces {

    class IDetector {
    public:

        virtual std::vector<Face> detect(const cv::Mat &img) = 0;

    };

    class TestDetector : public IDetector {
    public:
        explicit TestDetector(std::string const &a) : _val(a) {}

        std::vector<Face> detect(const cv::Mat &img) override;

        FACES_REGISTER_SUBCLASS(IDetector, TestDetector, Test, std::string const &)

    private:
        std::string _val;
    };

    FACES_REGISTER_SUBCLASS_ARGUMENTS(IDetector, TestDetector, Test)

}

#endif //FACES_IDETECTOR_H
