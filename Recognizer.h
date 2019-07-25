//
// Created by prostoichelovek on 08.05.19.
//

#ifndef FACES_FACERECOGNIZER_H
#define FACES_FACERECOGNIZER_H

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

#ifdef USE_DLIB

#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>

#endif

#include "utils.hpp"
#include "Face.h"
#include "Callbacks.hpp"

namespace Faces {

    extern cv::Size faceSize;

    bool normalizeImages(const std::string &filename, char separator = ';');

    class Recognizer {
    public:
        cv::Size inSize = cv::Size(300, 300);
        double inScaleFactor = 1.0;
        float confidenceThreshold = 0.7;
        cv::Scalar meanVal = cv::Scalar(104.0, 177.0, 123.0);

        cv::dnn::Net net;
        cv::Ptr<cv::face::LBPHFaceRecognizer> model;

        std::vector<Face> faces;
        std::vector<Face> lastFaces;

        int currentLabel = 0;
        std::vector<std::string> labels;
        std::map<int, int> imgNum;

        Callbacks callbacks;

#ifdef USE_DLIB
        dlib::shape_predictor landmarksPredictor;
#endif

        Recognizer();

        ~Recognizer();

        bool trainRecognizer(std::string imsList, std::string modelFile);

        bool detectFaces(cv::Mat &img);

        bool recognizeFaces(cv::Mat &img);

        bool operator()(cv::Mat &img);

        void sortFacesByScore();

        void preventOverlapping();

        Face *getLastFace(Face &now);

#ifdef USE_DLIB

        bool readLandmarksPredictor(std::string path);

        std::vector<cv::Mat> normalizeFaces(const cv::Mat &img);

#endif

        bool readNet(std::string configFile, std::string weightFile);

        bool readRecognitionModel(std::string file);

        bool readLabels(std::string file);

        bool readImageList(std::string file);

        void addLabel(std::string &label);

        std::string addTrainImage(std::string imagesDir, cv::Mat &img);

        void draw(cv::Mat &img, bool displayAligned = true);

    private:
        std::ofstream labelsFs;
        std::ofstream imsListFs;
    };

}

#endif //FACES_FACERECOGNIZER_H
