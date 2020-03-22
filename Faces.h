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

#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>

#include "utils/utils.hpp"
#include "Detection/Detection.h"
#include "Face/Face.h"
#include "Callbacks.hpp"
#include "Recognition/recognition.h"
#include "FaceChecker.h"
#include "utils/Config.h"

namespace Faces {

    class Faces {
    public:
        Config cfg;

        cv::Size faceSize = cv::Size(160, 160);

        Callbacks callbacks;
        Detection detector;
        Recognition::recognition recognition;
        FaceChecker checker;

        bool ok = true;

        int detectFreq = 1;
        int recognizeFreq = 1;

        // faceHistVal SVM for face checker
        explicit Faces(const std::string &config_file);

        void operator()(cv::Mat &img);

        void operator()(cv::Mat &img, cv::Mat &disp);

        void update();

        void draw(cv::Mat &img, bool displayAligned = true, bool draw_faces = true);

    private:
        int detectionSkipped = 0;
        int recognitionSkipped = 0;
    };

}

#endif //FACES_FACES_H
