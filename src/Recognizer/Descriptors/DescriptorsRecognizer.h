/**
 * @file DescriptorsRecognizer.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DESCRIPTORSRECOGNIZER_H
#define FACES_DESCRIPTORSRECOGNIZER_H

#include <type_traits>

#include "../Recognizer.hpp"

#include "Descriptor.hpp"
#include "DescriptorsClassifier.hpp"

namespace faces {

    class DescriptorsRecognizer : public Recognizer {
    public:
        DescriptorsClassifier *classifier;
        Descriptor *descriptor;

        DescriptorsRecognizer();

        bool save(std::string const &dst);

    protected:
        bool _checkOk();

        void _recognize(cv::Mat const &img, Face &face) override;

        void _train(std::map<int, cv::Mat &> const &samples) override;

    };

}

#endif //FACES_DESCRIPTORSRECOGNIZER_H
