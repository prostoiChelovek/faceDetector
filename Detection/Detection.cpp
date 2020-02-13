//
// Created by prostoichelovek on 08.05.19.
//

#include "Detection.h"

namespace Faces {

    Detection::Detection(Callbacks *callbacks, cv::Size faceSize)
            : callbacks(callbacks), faceSize(faceSize) {

    }

    Face *Detection::getLastFace(Face &now) {
        int minDist = -1;
        Face *r = nullptr;
        for (int i = 0; i < lastFaces.size(); i++) {
            Face *l = &lastFaces[i];
            int dist = getDist(l->rect.tl(), now.rect.tl())
                       + getDist(l->rect.br(), now.rect.br());
            if (dist < minDist && dist <= 50 || minDist == -1) {
                minDist = dist;
                r = l;
            }
        }
        return r;
    }


    bool Detection::readLandmarksPredictor(std::string path) {
        try {
            dlib::deserialize(path) >> landmarksPredictor;
            return true;
        } catch (std::exception &e) {
            log(ERROR, "Cannot read landmarks predictor from", path, ":", e.what());
            return false;
        }
    }

    std::vector<cv::Mat> Detection::normalizeFaces(const cv::Mat &img) {
        dlib::cv_image<dlib::bgr_pixel> dImg = img;
        std::vector<cv::Mat> faceChips;

        std::vector<dlib::rectangle> dFaces;
        for (const Face &f : faces) {
            dFaces.emplace_back(openCVRectToDlib(f.rect));
        }

        // Run the face detector on the image, and for each face extract a
        // copy that has been normalized to 150x150 pixels in size and appropriately rotated
        // and centered.
        int i = 0;
        for (dlib::rectangle face : dFaces) {
            dlib::full_object_detection shape = landmarksPredictor(dImg, face);

            dlib::matrix<dlib::rgb_pixel> face_chip;
            dlib::chip_details face_chip_det = get_face_chip_details(shape, faceSize.width, 0.25);
            dlib::extract_image_chip(dImg, face_chip_det, face_chip);

            cv::Rect rectNormalized = dlibRect2cv(face_chip_det.rect);
            cv::Point rectNormCenter(rectNormalized.x + (rectNormalized.width / 2),
                                     rectNormalized.y + (rectNormalized.height / 2));
            double angleDegree = face_chip_det.angle * 180 / CV_PI;
            faces[i].rect_normalized = cv::RotatedRect(rectNormCenter,
                                                       rectNormalized.size(),
                                                       angleDegree);

            for (int j = 0; j < shape.num_parts(); j++) {
                dlib::point pt = shape.part(j);
                faces[i].landmarks.emplace_back(pt.x(), pt.y());
            }

            faceChips.emplace_back(dlibMatrix2cvMat(face_chip));
            i++;
        }

        return faceChips;
    }

    bool Detection::readNet(std::string configFile, std::string weightFile) {
        net = cv::dnn::readNet(configFile, weightFile);
        if (net.empty()) {
            log(ERROR, "Could not load net (", configFile, ", ", weightFile, ")");
            return false;
        }
        return true;
    }

    bool Detection::detectFaces(cv::Mat &img) {
        cv::Mat inputBlob = cv::dnn::blobFromImage(img, inScaleFactor, inSize, meanVal,
                                                   false, false);
        net.setInput(inputBlob, "data");
        cv::Mat detection = net.forward("detection_out");

        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        faces.clear();

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > confidenceThreshold) {
                auto x1 = static_cast<int>(detectionMat.at<float>(i, 3) * img.cols);
                auto y1 = static_cast<int>(detectionMat.at<float>(i, 4) * img.rows);
                auto x2 = static_cast<int>(detectionMat.at<float>(i, 5) * img.cols);
                auto y2 = static_cast<int>(detectionMat.at<float>(i, 6) * img.rows);

                Face f;
                f.rect = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
                bool ok = f.checkBounds(cv::Size(img.cols, img.rows));
                if (!ok)
                    continue;

                f.confidence = confidence * 100;

                f.setLast(getLastFace(f));

                faces.emplace_back(f);

                if (callbacks != nullptr) {
                    callbacks->call("faceDetected", &faces[faces.size() - 1]);
                    if (f.hasMoved())
                        callbacks->call("faceMoved", &faces[faces.size() - 1]);
                }
            }
        }

        sortFacesByScore();
        preventOverlapping();

        std::vector<cv::Mat> faceImgs = normalizeFaces(img);
        for (int i = 0; i < faces.size(); i++) {
            faces[i].img = faceImgs[i];
        }

        return !faces.empty();
    }

    bool Detection::operator()(cv::Mat &img) {
        return detectFaces(img);
    }

    void Detection::sortFacesByScore() {
        // in most cases, false predictions occurrence when face is too close to camera
        auto countScore = [&](const Face &f) -> int {
            int score = f.confidence;
            if (f.last != nullptr)
                score += f.last->confidence;
            if (f.getLabel() > -3) { // if face recognition was performed
                score += f.labelNotChanged / 1.5;
                if (f.getLabel() > -1)
                    score *= 2;
            }
            // when face is close to camera, the most probable prediction,
            // that is nearer to left-top corner
            score -= (f.rect.x + f.rect.y) * 1;
            return score;
        };
        std::sort(faces.begin(), faces.end(), [&](const Face &a, const Face &b) -> bool {
            return countScore(a) > countScore(b);
        });
    }

    void Detection::preventOverlapping() {
        for (int i = 0; i < faces.size(); i++) {
            Face &f = faces[i];
            cv::Rect &rect = f.rect;
            for (int j = i + 1; j < faces.size(); j++) {
                Face &f2 = faces[j];
                cv::Rect &rect2 = f2.rect;
                if (((rect & rect2).area() > 0)) {
                    faces.erase(faces.begin() + j);
                    j--;
                }
            }
        }
    }

}