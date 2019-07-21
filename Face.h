//
// Created by prostoichelovek on 21.07.19.
//

#ifndef FACES_FACE_H
#define FACES_FACE_H


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

namespace Faces {

    class Face {
    public:
        cv::Rect rect;
        int label = -1;

        int confidence = 0;

        cv::Point offset; // distance between current position and last
        Face *last = nullptr;

        const static int minOffset = 5;

        Face() = default;

        Face(cv::Point p1, cv::Point p2, int label);

        bool checkBounds(const cv::Size &imgSize);
    };

}

#endif //FACES_FACE_H
