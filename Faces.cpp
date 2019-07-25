//
// Created by prostoichelovek on 25.07.19.
//

#include "Faces.h"

namespace Faces {

    Faces::Faces(std::string configFile, std::string weightFile, std::string landmarksPredictor,
                 std::string recognitionModel, std::string labelsList, std::string imagesList)
            : detector(&callbacks, faceSize), recognition(&callbacks, faceSize) {
        bool ok;
        if (!configFile.empty() && !weightFile.empty()) {
            ok = detector.readNet(configFile, weightFile);
            if (!ok)
                throw "Cannot load face detection model from " + configFile + " amd " + weightFile;
        }
        if (!recognitionModel.empty()) {
            ok = recognition.readModel(recognitionModel);
            if (!ok)
                throw "Cannot load face recognition model from " + recognitionModel;
        }
#ifdef USE_DLIB
        if (!landmarksPredictor.empty()) {
            ok = detector.readLandmarksPredictor((landmarksPredictor));
            if (!ok)
                throw "Cannot load landmarks predictor from " + landmarksPredictor;
        }
#endif
        if (!labelsList.empty()) {
            ok = recognition.readLabels(labelsList);
            if (!ok)
                throw "Cannot read labels list from " + labelsList;
        }
        if (!imagesList.empty()) {
            ok = recognition.readImageList(imagesList);
            if (!ok)
                throw "Cannot read images list from " + imagesList;
        }
    }

    bool Faces::operator()(cv::Mat &img) {
        bool ok = detector(img);
        if (!ok) return false;
        return recognition(detector.faces);
    }

    void Faces::update() {
        detector.lastFaces = detector.faces;
    }

    void Faces::draw(cv::Mat &img, bool displayAligned) {
        cv::Scalar clr = cv::Scalar(0, 255, 0);

#ifdef USE_DLIB
        // For aligned faces ->
        int max_vert = img.rows / faceSize.height;
        int vert = 0, hor = 0;
        int cols = (detector.faces.size() / max_vert) * faceSize.width;
        if (cols == 0 && !detector.faces.empty())
            cols = faceSize.width;
        cv::Mat facesImg(img.rows, cols, 16, cv::Scalar(255, 255, 255));
        // <- For aligned faces
#endif

        for (const Face &f : detector.faces) {
            // Color ->
            if (f.getLabel() == -1)
                clr = cv::Scalar(0, 0, 255);
            if (f.getLabel() == -2)
                clr = cv::Scalar(0, 125, 255);
            if (f.getLabel() == -3)
                clr = cv::Scalar(255, 0, 0);
            // <- Color

            cv::rectangle(img, f.rect, clr, 2, 4);

            // Label ->
            std::string text = std::to_string(f.confidence);
            if (f.getLabel() > -1) {
                std::string label = f.getLabel() < recognition.labels.size()
                                    ? recognition.labels[f.getLabel()] : "WTF";
                text += " - " + label;
            }
            std::for_each(text.begin(), text.end(),
                          [](char &ch) { ch = ::toupper(static_cast<unsigned char>(ch)); });

            cv::rectangle(img, cv::Rect(f.rect.x, f.rect.y - 20, f.rect.width, 25),
                          clr, cv::FILLED, 4);
            cv::putText(img, text, f.rect.tl(),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            // <- Label

            // Moving direction ->
            auto drawCrn = [&](int x, int y, int width, int height) {
                cv::Point start(x, y);
                cv::Point end = start;
                if (width != 0)
                    end.x += width;
                if (height != 0)
                    end.y += height;
                cv::line(img, start, end, cv::Scalar(255, 255, 255), 3);
            };

            // Handle transverse directions(like right-up)
            // ie when moving in one direction - draw it on center
            // but when moving in two direction - draw them on corner
            int h = f.rect.height / 2;
            int startY = f.rect.y + (f.rect.height / 4);
            if (-f.offset.y >= f.minOffset) // up
                startY = f.rect.y - 20;
            if (f.offset.y >= f.minOffset) { // down
                h = -h;
                startY = f.rect.br().y;
            }
            int w = f.rect.width / 2;
            int startX = f.rect.x + (f.rect.width / 4);
            if (f.offset.x >= f.minOffset) { // right
                w = -w;
                startX = f.rect.br().x;
            }
            if (-f.offset.x >= f.minOffset) // left
                startX = f.rect.x;

            if (f.offset.x >= f.minOffset) // right
                drawCrn(f.rect.br().x, startY, 0, h);
            if (-f.offset.x >= f.minOffset) // left
                drawCrn(f.rect.x, startY, 0, h);
            if (-f.offset.y >= f.minOffset) // up
                drawCrn(startX, f.rect.y - 20, w, 0);
            if (f.offset.y >= f.minOffset) // down
                drawCrn(startX, f.rect.br().y, w, 0);
            // <- Moving direction

            // Landmarks ->
            for (const cv::Point &lm : f.landmarks) {
                cv::circle(img, lm, 1, clr);
            }
            // <- Landmarks

#ifdef USE_DLIB
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
#endif
        }
#ifdef USE_DLIB
        if (displayAligned)
            cv::hconcat(std::vector<cv::Mat>{img, facesImg}, img);
#endif
    }

}