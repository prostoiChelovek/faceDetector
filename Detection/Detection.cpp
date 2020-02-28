//
// Created by prostoichelovek on 08.05.19.
//

#include "Detection.h"

namespace Faces {

    Detection::Detection(Callbacks *callbacks, cv::Size faceSize)
            : callbacks(callbacks), faceSize(faceSize),
              p_mtcnn(mtcnn_factory::create_detector("tensorflow")) {
        if (p_mtcnn == nullptr) {
            std::cerr << "supported types: ";
            std::vector<std::string> type_list = mtcnn_factory::list();
            for (const auto &i : type_list)
                std::cerr << " " << i;
            std::cerr << std::endl;
        }
        p_mtcnn->set_threshold(0.6, 0.75, 0.8);
    }

    Face *Detection::getLastFace(Face &now) {
        int minDist = -1;
        Face *r = nullptr;
        for (int i = 0; i < lastFaces.size(); i++) {
            Face *l = &lastFaces[i];

            cv::Point a_center(now.rect.x + now.rect.width / 2, now.rect.y + now.rect.height / 2);
            cv::Point b_center(l->rect.x + l->rect.width / 2, l->rect.y + l->rect.height / 2);

            int dist = getDist(a_center, b_center);
            if ((dist < minDist || minDist == -1) && dist <= 150) {
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

    bool Detection::readNet(const std::string &models_dir) {
        if (p_mtcnn->load_model(models_dir) == -1) {
            log(ERROR, "Could not load mtcnn from", models_dir);
            return false;
        }
        return true;
    }

    bool Detection::detectFaces(cv::Mat &img) {
        cv::Mat inputBlob = cv::dnn::blobFromImage(img, inScaleFactor, inSize, meanVal,
                                                   false, false);
        faces.clear();

        std::vector<face_box> face_info;
        p_mtcnn->detect(img, face_info);

        for (const face_box &box : face_info) {
            Face f;
            f.rect = cv::Rect(cv::Point(box.x0, box.y0),
                              cv::Point(box.x1, box.y1));

            f.confidence = box.score * 100;

            f.setLast(getLastFace(f));

            faces.emplace_back(f);
            if (callbacks != nullptr) {
                callbacks->call("faceDetected", &faces[faces.size() - 1]);
                if (f.hasMoved())
                    callbacks->call("faceMoved", &faces[faces.size() - 1]);
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