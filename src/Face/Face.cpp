/**
 * @file Face.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains the Face class implementation
 */

#include "Face.h"

namespace faces {

    std::vector<cv::Point> Face::getRectLandmarks() const {
        std::vector<cv::Point> res;
        std::transform(landmarks.begin(), landmarks.end(), std::back_inserter(res),
                       [&](cv::Point const &pt) -> cv::Point { return rect.tl() + pt; });
        return res;
    }
}