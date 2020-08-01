/**
 * @file DescriptorsRecognizer.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 */

#include "DescriptorsRecognizer.h"

namespace faces {

    bool DescriptorsRecognizer::save(std::string const &dst) {
        return classifier->save(dst);
    }

    bool DescriptorsRecognizer::_checkOk() {
        _ok = classifier->isOk() && descriptor->isOk();
        return _ok;
    }

    void DescriptorsRecognizer::_recognize(cv::Mat const &img, Face &face) {
        if (!_checkOk()) return;

        const cv::Mat faceRoi = img(face.rect);

        std::vector<double> descriptors = descriptor->computeDescriptors(faceRoi);
        int label = classifier->classifyDescriptors(descriptors);

        face.label = label;
    }

    void DescriptorsRecognizer::train(std::map<int, cv::Mat &> const &samples) {
        if (!descriptor->isOk()) return;

        std::map<int, std::vector<double>> descriptorSamples;
        for (auto const &sample : samples) {
            descriptorSamples[sample.first] = descriptor->computeDescriptors(sample.second);
        }

        classifier->train(descriptorSamples);
        _checkOk();
    }

}