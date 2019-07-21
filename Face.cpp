//
// Created by prostoichelovek on 21.07.19.
//

#include "Face.h"

namespace Faces {

    Face::Face(cv::Point p1, cv::Point p2, int label) : label(label) {
        rect = cv::Rect(p1, p2);
    }

    bool Face::checkBounds(const cv::Size &imgSize) {
        if (rect.width <= 0 || rect.height <= 0)
            return false;
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        if (rect.x >= imgSize.width)
            rect.x = imgSize.width - rect.width;
        if (rect.y >= imgSize.height)
            rect.y = imgSize.height - rect.height;
        if (rect.x + rect.width > imgSize.width)
            rect.width = imgSize.width - rect.x;
        if (rect.y + rect.height > imgSize.height)
            rect.height = imgSize.height - rect.y;
        if (rect.width > imgSize.width)
            rect.width = imgSize.width;
        if (rect.height > imgSize.height)
            rect.height = imgSize.height;
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        return true;
    }

}