/**
 * @file Recognizer.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_RECOGNIZER_HPP
#define FACES_RECOGNIZER_HPP

#include "Face/Face.h"

namespace faces {

    class Recognizer {
    public:
        void recognize(cv::Mat const &img, Face &face) {
            if (!_ok) {
                return;
            }

            _recognize(img, face);
        }

        void recognize(cv::Mat const &img, std::vector<Face> &faces) {
            for (auto &face : faces) {
                recognize(img, face);
            }
        }

        void train(std::map<int, cv::Mat &> const &samples) {
            _train(samples);
        }

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the recognizer
        bool _ok = false;

        virtual void _recognize(cv::Mat const &img, Face &face) = 0;

        virtual void _train(std::map<int, cv::Mat &> const &samples) = 0;
    };

}

#endif //FACES_RECOGNIZER_HPP
