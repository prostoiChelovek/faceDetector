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

#include <dlib/svm.h>

#include "Face/Face.h"
#include "utils/utils.hpp"

namespace Faces {

    class FaceChecker {
    public:
        typedef dlib::matrix<double, 256, 1> sample_type;
        typedef dlib::histogram_intersection_kernel<sample_type> kernel_type;
        typedef dlib::svm_c_trainer<kernel_type> trainer_type;
        typedef dlib::decision_function<kernel_type> classifier_type;

        classifier_type classifier;
        std::string classifierPath;

        double threshold = 0.1;

        bool ok = false;

        explicit FaceChecker(std::string classifierPath);

        FaceChecker() = default;

        bool check(cv::Mat &faceDisp);

        void addTrainSample(cv::Mat &faceDisp, std::string dir, bool isReal);

        void train(std::string samplesDir);

        void save();

        bool load();

    private:
        cv::Mat calculateHist(cv::Mat faceDisp);

    };

}

#endif //FACES_FACECHECKER_H
