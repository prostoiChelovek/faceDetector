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

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>

#include "../utils/utils.hpp"
#include "../Face/Face.h"
#include "../Callbacks.hpp"
#include "../Detection/Detection.h"
#include "RecognitionNet.hpp"

namespace Faces {
    namespace Recognition {

        class recognition {
        public:
            int currentLabel = 0;
            std::vector<std::string> labels;
            std::map<int, int> imgNum;

            double threshold = 0.3;

            cv::Size faceSize = cv::Size(150, 150);

            int minLabelNotChanged = 5;

            Callbacks *callbacks;

            bool ok = false;

            explicit recognition(Callbacks *callbacks, cv::Size faceSize,
                                 std::string classifierFile, std::string descriptorFile);

            ~recognition();

            void train(std::string samplesDir);

            bool save(std::string file);

            void load(std::string classifierFile, std::string descriptorFile);

            std::string addSample(std::string storage, Face &face);

            std::map<std::string, int> getSamples(std::string dir);

            bool readLabels(std::string file);

            void addLabel(std::string &label);

            void getDescriptors(Face &face);

            void recognize(Face &face);

            void recognize(std::vector<Face> &faces);

        private:
            std::ofstream labelsFs;

            // DNN that estimates 128D face descriptors
            anet_type descriptor;

            std::vector<MyClassifier> classifiers;

        };

    }
}


#endif //FACES_RECOGNITION_H
