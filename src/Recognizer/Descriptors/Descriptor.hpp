/**
 * @file Descriptor.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DESCRIPTOR_HPP
#define FACES_DESCRIPTOR_HPP

#include <opencv2/opencv.hpp>

#include "utils/utils.h"

namespace faces {

    class Descriptor {
    public:
        std::vector<double> computeDescriptors(cv::Mat const &faceImg) {
            if (!_ok) {
                return {};
            }

            cv::Mat preparedImg = prepareImage(faceImg);

            return _computeDescriptors(preparedImg);
        }

        cv::Mat prepareImage(cv::Mat const &faceImg) {
            cv::Mat prepared;
            cv::resize(faceImg, prepared, get_faceSize());
            _prepareImage(prepared);

            return prepared;
        }

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the detector
        bool _ok = false;

        FACES_DECLARE_ATTRIBUTE(cv::Size, faceSize)

        virtual std::vector<double> _computeDescriptors(cv::Mat const &faceImg) = 0;

        virtual void _prepareImage(cv::Mat const &faceImg) {}

    };

}

#endif //FACES_DESCRIPTOR_HPP
