/**
 * @file IDetection.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief
 */

#include "IDetector.h"

namespace faces {

    std::vector<Face> TestDetector::detect(const cv::Mat &img) {
        std::cout << "It works!" << std::endl;
        return std::vector<Face>();
    }

}
