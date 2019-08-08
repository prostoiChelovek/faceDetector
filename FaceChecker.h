//
// Created by prostoichelovek on 08.08.19.
//

#ifndef FACES_FACECHECKER_H
#define FACES_FACECHECKER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/face.hpp>
#include <opencv2/ml.hpp>

#include "Face.h"
#include "utils.hpp"

namespace Faces {

    class FaceChecker {
    public:
        cv::Ptr<cv::ml::SVM> classifier;
        std::string classifierPath;

        bool ok = false;

        explicit FaceChecker(std::string classifierPath);

        bool check(cv::Mat &faceDisp);

        void create();

        void addTrainSample(cv::Mat &faceDisp, std::string dir, bool isReal);

        void train(std::string samplesDir);

        void save();

        bool load();

    private:
        cv::Mat calculateHist(cv::Mat faceDisp);

    };

}

#endif //FACES_FACECHECKER_H
