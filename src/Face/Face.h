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

        /// A predicted by the Recognizer label of this face
        int label = -1;

        Face() = default;

        explicit Face(cv::Rect rect)
                : rect(std::move(rect)) {}

    };

}


#endif //FACES_FACE_H
