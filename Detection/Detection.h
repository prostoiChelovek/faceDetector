//
// Created by prostoichelovek on 08.05.19.
//

#ifndef FACES_FACERECOGNIZER_H
#define FACES_FACERECOGNIZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/face.hpp>

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>

#include "../utils.hpp"
#include "../Face/Face.h"
#include "../Callbacks.hpp"

namespace Faces {

    class Detection {
    public:
        cv::Size inSize = cv::Size(300, 300);
        double inScaleFactor = 1.0;
        float confidenceThreshold = 0.7;
        cv::Scalar meanVal = cv::Scalar(104.0, 177.0, 123.0);

        cv::Size faceSize = cv::Size(200, 200);

        cv::dnn::Net net;

        std::vector<Face> faces;
        std::vector<Face> lastFaces;

        Callbacks *callbacks;

        dlib::shape_predictor landmarksPredictor;

        explicit Detection(Callbacks *callbacks = nullptr, cv::Size faceSize = cv::Size(200, 200));

        bool detectFaces(cv::Mat &img);

        bool operator()(cv::Mat &img);

        void sortFacesByScore();

        void preventOverlapping();

        Face *getLastFace(Face &now);

        bool readLandmarksPredictor(std::string path);

        std::vector<cv::Mat> normalizeFaces(const cv::Mat &img);

        bool readNet(std::string configFile, std::string weightFile);

    };

}

#endif //FACES_FACERECOGNIZER_H
