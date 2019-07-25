//
// Created by prostoichelovek on 25.07.19.
//

#ifndef FACES_RECOGNITION_H
#define FACES_RECOGNITION_H


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
#include "Detector.h"

namespace Faces {

    bool normalizeImages(const std::string &filename, cv::Size faceSize, char separator = ';');

    class Recognizer {
    public:
        cv::Ptr<cv::face::LBPHFaceRecognizer> model;

        int currentLabel = 0;
        std::vector<std::string> labels;
        std::map<int, int> imgNum;

        cv::Size faceSize = cv::Size(200, 200);

        Callbacks *callbacks;

        explicit Recognizer(Callbacks *callbacks = nullptr, cv::Size faceSize = cv::Size(200, 200));

        ~Recognizer();

        bool train(std::string imsList, std::string modelFile);

        bool operator()(std::vector<Face> &faces);

        bool operator()(Face &face);

        bool readModel(std::string file);

        bool readLabels(std::string file);

        bool readImageList(std::string file);

        void addLabel(std::string &label);

        std::string addTrainImage(std::string imagesDir, cv::Mat &img);

    private:
        std::ofstream labelsFs;
        std::ofstream imsListFs;

    };

}

#endif //FACES_RECOGNITION_H
