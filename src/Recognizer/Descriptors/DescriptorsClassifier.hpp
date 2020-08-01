/**
 * @file DescriptorsClassifier.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DESCRIPTORSCLASSIFIER_HPP
#define FACES_DESCRIPTORSCLASSIFIER_HPP

#include <vector>
#include <map>

#include "utils/utils.h"

namespace faces {

    class DescriptorsClassifier {
    public:
        FACES_DECLARE_ATTRIBUTE(double, threshold)

        int classifyDescriptors(const std::vector<double> &descriptors) {
            if (!_ok) {
                return -2;
            }

            return _classifyDescriptors(descriptors);
        }

        virtual void train(std::map<int, std::vector<double>> const &samples) = 0;

        bool save(std::string const &dst) {
            if (!_ok) {
                return false;
            }

            return _save(dst);
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

        virtual int _classifyDescriptors(const std::vector<double> &descriptors) = 0;

        virtual bool _save(std::string const &dst) = 0;

    };

}

#endif //FACES_DESCRIPTORSCLASSIFIER_HPP
