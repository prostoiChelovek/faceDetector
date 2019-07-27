//
// Created by prostoichelovek on 25.07.19.
//

#ifndef FACES_FACES_H
#define FACES_FACES_H

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
#include "Detector.h"
#include "Face.h"
#include "Callbacks.hpp"
#include "Recognizer.h"
#include "Recognizer_LBPH.h"
#include "Recognizer_Descriptors.h"

namespace Faces {

    class Faces {
    public:
        cv::Size faceSize = cv::Size(150, 150);

        Callbacks callbacks;
        Detector detector;
        Recognizer *recognition;

        bool ok = false;

        // recognitionModel used in LBPH recognizer
        // descriptorEstimator and faceClassifiers used in descriptor-based recognizer
        Faces(std::string configFile, std::string weightFile,
              std::string landmarksPredictor = "", std::string LBPH_model = "",
              std::string descriptorEstimator = "", std::string faceClassifiers = "",
                       std::string labelsList = "", std::string imagesList = "");

        void operator()(cv::Mat &img);

        void update();

        void draw(cv::Mat &img, bool displayAligned = true);

    };

}

#endif //FACES_FACES_H
