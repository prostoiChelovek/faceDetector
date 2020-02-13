//
// Created by prostoichelovek on 27.07.19.
//

#ifndef FACES_DESCRIPTORS_H
#define FACES_DESCRIPTORS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>

#include "MyClassifier.hpp"
#include "RecognitionNet.hpp"
#include "recognition.h"

namespace Faces {
    namespace Recognition {

        class Descriptors : public recognition {
        public:
            double threshold = 0.3;

            Descriptors(Callbacks *callbacks, cv::Size faceSize,
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
}


#endif //FACES_DESCRIPTORS_H
