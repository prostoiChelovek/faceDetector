/**
 * @file OcvDnnDetector.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 19 Jul 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_OCVDNNDETECTOR_H
#define FACES_OCVDNNDETECTOR_H

#include "IDetector.h"

namespace faces {

    class OcvDnnDetector : public IDetector {
    protected:
        FACES_DECLARE_ATTRIBUTE(cv::Size, inSize)

        FACES_DECLARE_ATTRIBUTE(double, inScaleFactor)

        FACES_DECLARE_ATTRIBUTE(cv::Scalar, meanVal)


    public:
        FACES_DECLARE_ATTRIBUTE(float, confidenceThreshold)

        std::vector<Face> detect(const cv::Mat &img) override {
            std::cout << get_inSize() << std::endl;
            return std::vector<Face>();
        }

    };

}


#endif //FACES_OCVDNNDETECTOR_H
