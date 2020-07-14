/**
 * @file Face.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains the Face class definition
 */

#ifndef FACES_FACE_H
#define FACES_FACE_H

#include <utility>

#include <opencv2/opencv.hpp>

namespace faces {

    /**
     * This is the class representing faces.
     */
    class Face {
    public:
        /// The bounding box of the face on an image
        cv::Rect rect;
        /// The confidence of a detector about this face
        int detectionConfidence = 0;

        Face() = default;

        explicit Face(cv::Rect rect, const int &detectionConfidence = 0)
                : rect(std::move(rect)), detectionConfidence(detectionConfidence) {}

        Face(const cv::Point &pt1, const cv::Point &pt2, const int &detectionConfidence = 0)
                : rect(pt1, pt2), detectionConfidence(detectionConfidence) {}
    };

}


#endif //FACES_FACE_H
