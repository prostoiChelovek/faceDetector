//
// Created by prostoichelovek on 27.07.19.
//

#ifndef FACES_LBPH_H
#define FACES_LBPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "recognition.h"

namespace Faces {
    namespace Recognition {

        class LBPH : public recognition {
        public:
            LBPH(Callbacks *callbacks, cv::Size faceSize, std::string modelFile);

            void train(std::string samplesDir) override;

            bool save(std::string file) override;

            std::string addSample(std::string storage, Face &face) override;

            void setThreshold(double val) override;

            void load(std::string modelFile);

        private:
            cv::Ptr<cv::face::LBPHFaceRecognizer> model;

            void recognize(Face &face) override;

            std::map<std::string, int> getSamples(std::string dir);

        };

    }
}


#endif //FACES_LBPH_H
