//
// Created by prostoichelovek on 25.07.19.
//

#ifndef FACES_RECOGNITION_H
#define FACES_RECOGNITION_H


#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "utils.hpp"
#include "Face.h"
#include "Callbacks.hpp"
#include "Detector.h"

namespace Faces {

    class Recognizer {
    public:
        int currentLabel = 0;
        std::vector<std::string> labels;
        std::map<int, int> imgNum;

        cv::Size faceSize = cv::Size(150, 150);

        Callbacks *callbacks;

        bool ok = false;

        explicit Recognizer(Callbacks *callbacks = nullptr, cv::Size faceSize = cv::Size(150, 150));

        ~Recognizer();

        void operator()(Face &face);

        void operator()(std::vector<Face> &faces);

        virtual void train(std::string samplesDir) = 0;

        virtual bool save(std::string file) = 0;

        virtual std::string addSample(std::string storage, Face &face) = 0;

        virtual void setThreshold(double val) = 0;

        bool readLabels(std::string file);

        void addLabel(std::string &label);

    private:
        std::ofstream labelsFs;

        virtual void recognize(Face &face) = 0;

    };

}

#endif //FACES_RECOGNITION_H
