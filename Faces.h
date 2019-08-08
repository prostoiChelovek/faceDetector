//
// Created by prostoichelovek on 25.07.19.
//

#ifndef FACES_FACES_H
#define FACES_FACES_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/face.hpp>

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
#include "Detector.h"
#include "Face.h"
#include "Callbacks.hpp"
#include "Recognizer.h"
#include "Recognizer_LBPH.h"
#include "Recognizer_Descriptors.h"
#include "FaceChecker.h"

namespace Faces {

    class Faces {
    public:
        cv::Size faceSize = cv::Size(150, 150);

        Callbacks callbacks;
        Detector detector;
        Recognizer *recognition = nullptr;
        FaceChecker checker;

        bool ok = true;

        int detectFreq = 1;
        int recognizeFreq = 1;

        // recognitionModel used in LBPH recognizer
        // descriptorEstimator and faceClassifiers used in descriptor-based recognizer
        // faceHistVal SVM for face checker
        Faces(std::string configFile, std::string weightFile,
              std::string landmarksPredictor = "", std::string LBPH_model = "",
              std::string descriptorEstimator = "", std::string faceClassifiers = "",
              std::string faceHistVal = "", std::string labelsList = "");

        void operator()(cv::Mat &img);

        void operator()(cv::Mat &img, cv::Mat &disp);

        void update();

        void draw(cv::Mat &img, bool displayAligned = true);

    private:
        int detectionSkipped = 0;
        int recognitionSkipped = 0;
    };

}

#endif //FACES_FACES_H
