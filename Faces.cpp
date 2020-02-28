//
// Created by prostoichelovek on 25.07.19.
//

#include "Faces.h"

namespace Faces {

    Faces::Faces(const std::string &config_file)
            : cfg(config_file),
              detector(&callbacks, faceSize),
              recognition(&callbacks, faceSize, cfg.recognition_classifier, cfg.recognition_descriptorsNet) {

        if (!detector.readNet("../modules/MTCNN/models")) { // TODO: add this to config
            log(ERROR, "Cannot load face detection model from", cfg.detection_netConfig, "amd",
                cfg.detection_netWeights);
            ok = false;
        }

        if (!cfg.landmarks_model.empty()) {
            if (!detector.readLandmarksPredictor((cfg.landmarks_model))) {
                log(ERROR, "Cannot load landmarks predictor from", cfg.landmarks_model);
                ok = false;
            }
        }

        if (!cfg.checker_histogramChecker.empty()) {
            checker = FaceChecker(cfg.checker_histogramChecker);
            if (!checker.ok) {
                log(WARNING, "Cannot initialize face checker with classifier path", cfg.checker_histogramChecker);
            }
        }

        if (!cfg.samples_labelsFile.empty()) {
            if (!recognition.readLabels(cfg.samples_labelsFile)) {
                log(ERROR, "Cannot read labels list from", cfg.samples_labelsFile);
                ok = false;
            }
        }
    }

    void Faces::operator()(cv::Mat &img) {
        if (detectionSkipped == detectFreq) {
            detector(img);
            detectionSkipped = 0;
        } else detectionSkipped++;
        if (recognitionSkipped == recognizeFreq) {
            recognition.recognize(detector.faces);
            recognitionSkipped = 0;
        } else recognitionSkipped++;
    }

    void Faces::operator()(cv::Mat &img, cv::Mat &disp) {
        operator()(img);
        for (Face &f : detector.faces) {
            cv::Mat faceDisp = disp(f.rect);
            cv::resize(faceDisp, faceDisp, faceSize);
            bool real = checker.check(faceDisp);
            if (!real)
                f.setLabel(-4);
        }
    }

    void Faces::update() {
        detector.lastFaces = detector.faces;
        for (Face &f : detector.lastFaces) {
            f.last = nullptr;
        }
    }

    void Faces::draw(cv::Mat &img, bool displayAligned, bool draw_faces) {
        // For aligned faces ->
        int max_vert = img.rows / faceSize.height;
        int vert = 0, hor = 0;
        int cols = std::ceil((float) detector.faces.size() / (float) max_vert) * faceSize.width;
        if (cols == 0 && !detector.faces.empty())
            cols = faceSize.width;
        cv::Mat facesImg(img.rows, cols, 16, cv::Scalar(255, 255, 255));
        // <- For aligned faces

        for (const Face &f : detector.faces) {
            if (draw_faces) {
                f.draw(img, &recognition.labels);
            }

            // Aligned faces ->
            if (displayAligned) {
                if (vert < max_vert) {
                    cv::Rect r = cv::Rect(
                            hor * f.img.cols, vert * f.img.rows,
                            f.img.cols, f.img.rows
                    );
                    f.img.copyTo(facesImg(r));
                    vert++;
                } else {
                    hor++;
                    vert = 0;
                }
            }
            // <- Aligned faces
        }
        if (displayAligned)
            cv::hconcat(std::vector<cv::Mat>{img, facesImg}, img);
    }

}