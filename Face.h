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

        int confidence = 0;

        const static int minOffset = 5;
        cv::Point offset; // distance between current position and last

        const static int minLabelNotChanged = 5;
        long labelNotChanged = 0; // number of consecutive label recognitions

        Face *last = nullptr;

        std::map<std::string, int> executedCallbacks;

        Face() = default;

        Face(cv::Point p1, cv::Point p2, int label);

        bool checkBounds(const cv::Size &imgSize);

        void setLast(Face *f);

        void setLabel(int lbl);

        // -1 -- not recognized
        // -2 -- label changed recently
        int getLabel() const;

        bool hasMoved() const;

        bool operator==(const Face &f);

        bool operator!=(const Face &f);

    private:
        int label = -1;

    };

}

#endif //FACES_FACE_H
