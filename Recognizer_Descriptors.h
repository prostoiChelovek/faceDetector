//
// Created by prostoichelovek on 27.07.19.
//

#ifndef FACES_RECOGNIZER_DESCRIPTORS_H
#define FACES_RECOGNIZER_DESCRIPTORS_H

#ifdef USE_DLIB

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

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>

#include "MyClassifier.hpp"
#include "RecognitionNet.hpp"
#include "Recognizer.h"

namespace Faces {

    class Recognizer_Descriptors : public Recognizer {
    public:
        double threshold = 0.3;

        Recognizer_Descriptors(Callbacks *callbacks, cv::Size faceSize,
                               std::string classifierFile, std::string descriptorFile);

        // .csv file contains face descriptors and label for them
        void train(std::string samplesDir) override;

        bool save(std::string file) override;

        std::string addSample(std::string storage, Face &face) override;

        void setThreshold(double val) override;

        void load(std::string classifierFile, std::string descriptorFile);

    private:
        // DNN that estimates 128D face descriptors
        anet_type descriptor;

        std::vector<MyClassifier> classifiers;

        void recognize(Face &face) override;

        void getDescriptors(Face &face);

        std::map<std::string, int> getSamples(std::string dir);

    };

}

#endif //USE_DLIB

#endif //FACES_RECOGNIZER_DESCRIPTORS_H
