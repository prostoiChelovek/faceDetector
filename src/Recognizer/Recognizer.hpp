/**
 * @file Recognizer.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief This file contains an interface for face recognizers
 */

#ifndef FACES_RECOGNIZER_HPP
#define FACES_RECOGNIZER_HPP

#include "Face/Face.h"

namespace faces {

    /**
     * A base class for all of the face recognizers
     */
    class Recognizer {
    public:
        /**
         * Estimate a label of the given face on photo.
         * This is a wrapper around the actual recognition method, which is just checking the @ref _ok flag
         *
         * @param img - photo, face was detected on
         * @param face - the face, estimate a label for ROI of which
         */
        void recognize(cv::Mat const &img, Face &face) {
            if (!_ok) {
                return;
            }

            _recognize(img, face);
        }

        /**
         * A helper method which recognizes a bunch of faces
         *
         * @param img - photo, face was detected on
         * @param face - the face, estimate a label for ROI of which
         */
        void recognize(cv::Mat const &img, std::vector<Face> &faces) {
            for (auto &face : faces) {
                recognize(img, face);
            }
        }

        /**
         * Train a detector
         *
         * @param samples - a map in a format {label: face ROI}
         */
        virtual void train(std::map<int, cv::Mat &> const &samples) = 0;

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the recognizer
        bool _ok = false;

        /**
         * Estimate a label of the given face on photo.
         *
         * @param img - photo, face was detected on
         * @param face - the face, estimate a label for ROI of which
         */
        virtual void _recognize(cv::Mat const &img, Face &face) = 0;

    };

}

#endif //FACES_RECOGNIZER_HPP