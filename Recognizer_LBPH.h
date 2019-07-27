//
// Created by prostoichelovek on 27.07.19.
//

#ifndef FACES_RECOGNIZER_LBPH_H
#define FACES_RECOGNIZER_LBPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/face.hpp>
#include <opencv2/tracking.hpp>

#include "Recognizer.h"

namespace Faces {

    class Recognizer_LBPH : public Recognizer {
    public:
        Recognizer_LBPH(Callbacks *callbacks, cv::Size faceSize, std::string modelFile);

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

#endif //FACES_RECOGNIZER_LBPH_H
