/**
 * @file IDetection.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains an interface for face detectors
 */

#ifndef FACES_DETECTOR_H
#define FACES_DETECTOR_H

#include <vector>

#include <opencv2/opencv.hpp>

#include "Face/Face.h"
#include "utils/utils.h"

namespace faces {

    class Detector {
    public:

        std::vector<Face> detect(cv::Mat const &img) {
            if (!_ok) {
                return {};
            }
            return _detect(img);
        }

        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        bool _ok = false;

        virtual std::vector<Face> _detect(cv::Mat const &img) = 0;

    };

}

#endif //FACES_DETECTOR_H
