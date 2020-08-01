/**
 * @file Descriptor.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 */

#include "Descriptor.hpp"

namespace faces {

    std::vector<double> Descriptor::computeDescriptors(cv::Mat const &faceImg) {
        if (!_ok) {
            return {};
        }

        cv::Mat preparedImg = prepareImage(faceImg);

        return _computeDescriptors(preparedImg);
    }

    cv::Mat Descriptor::prepareImage(cv::Mat const &faceImg) {
        cv::Mat prepared;
        cv::resize(faceImg, prepared, get_faceSize());
        _prepareImage(prepared);

        return prepared;
    }

}