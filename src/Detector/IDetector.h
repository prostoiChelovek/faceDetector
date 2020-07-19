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

        cv::Size faceSize = cv::Size(200, 200);

        virtual std::vector<Face> detect(const cv::Mat &img) = 0;

    };

}

#endif //FACES_IDETECTOR_H
