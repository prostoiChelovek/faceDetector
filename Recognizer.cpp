//
// Created by prostoichelovek on 08.05.19.
//

#include "Recognizer.h"

namespace Faces {

    cv::Size faceSize = cv::Size(200, 240);

    bool normalizeImages(const std::string &filename, char separator) {
        std::ifstream file(filename.c_str(), std::ifstream::in);
        if (!file) {
            std::cerr << "No valid input file was given, please check the given filename." << std::endl;
            return false;
        }
        std::string line, path;
        while (getline(file, line)) {
            std::stringstream liness(line);
            getline(liness, path, separator);
            if (path.empty())
                continue;
            cv::Mat img = cv::imread(path);
            if (img.empty())
                continue;
            if (img.size() != faceSize)
                cv::resize(img, img, faceSize);
            cv::imwrite(path, img);
        }
        return true;
    }


    Recognizer::Recognizer() {
        model = cv::face::LBPHFaceRecognizer::create();
    }

    Recognizer::~Recognizer() {
        labelsFs.close();
        imsListFs.close();
    }

    Face *Recognizer::getLastFace(Face &now) {
        int minDist = -1;
        Face *r = nullptr;
        for (auto &lastFace : lastFaces) {
            Face *l = &lastFace;
            int dist = getDist(l->rect.tl(), now.rect.tl())
                       + getDist(l->rect.br(), now.rect.br());
            if (dist < minDist && dist <= 50 || minDist == -1) {
                minDist = dist;
                r = l;
            }
        }
        return r;
    }


    bool Recognizer::readNet(std::string caffeConfigFile, std::string caffeWeightFile) {
        net = cv::dnn::readNet(caffeConfigFile, caffeWeightFile);
        if (net.empty()) {
            fprintf(stderr, "Could not load net (%s, %s)", caffeConfigFile.c_str(), caffeWeightFile.c_str());
            return false;
        }
        return true;
    }


    bool Recognizer::readRecognitionModel(std::string file) {
        try {
            model->read(file);
        } catch (std::exception &e) {
            std::cerr << "Cannot read face recognition model: " << e.what() << std::endl;
            return false;
        }
        return !model->empty();
    }

    bool Recognizer::readLabels(std::string file) {
        if (!getFileContent(file, labels))
            return false;
        labelsFs = std::ofstream(file, std::ios::app);
        if (!labelsFs) {
            std::cerr << "Could not open labels file for write " << file << std::endl;
            return false;
        }
        return true;
    }

    bool Recognizer::readImageList(std::string file) {
        imsListFs = std::ofstream(file, std::ios::app);
        if (!imsListFs.is_open()) {
            std::cerr << "Could not open image list file " << file << std::endl;
            return false;
        }
        std::vector<cv::Mat> imgs;
        std::vector<int> lbls;
        read_csv(file, imgs, lbls);
        for (int lbl : lbls)
            imgNum[lbl]++;

        return true;
    }

    bool Recognizer::detectFaces(cv::Mat &img) {
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
            }
        }

        sortFacesByScore();
        preventOverlapping();

        return !faces.empty();
    }

    bool Recognizer::recognizeFaces(cv::Mat &img) {
        if (!model->empty()) {
            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            for (Face &f : faces) {
                cv::Mat resized = gray.clone()(f.rect);
                cv::resize(resized, resized, faceSize, 1, 1);
                f.setLabel(model->predict(resized));
            }
            return true;
        }
        return false;
    }

    bool Recognizer::operator()(cv::Mat &img) {
        if (!detectFaces(img)) return false;
        return recognizeFaces(img);
    }

    void Recognizer::sortFacesByScore() {
        // in most cases, false predictions occurrence when face is too close to camera
        auto countScore = [&](const Face &f) -> int {
            int score = f.confidence;
            if (f.last != nullptr)
                score += f.last->confidence;
            if (!model->empty()) {
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

    void Recognizer::preventOverlapping() {
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

    void Recognizer::addLabel(std::string &label) {
        labels.emplace_back(label);
        currentLabel = labels.size() - 1;
        imgNum[currentLabel] = 0;
        labelsFs << label << "\n";
        labelsFs.flush();
    }

    std::string Recognizer::addTrainImage(std::string imagesDir, cv::Mat &img) {
        std::string path = imagesDir + "/" + std::to_string(currentLabel)
                           + "_" + std::to_string(imgNum[currentLabel]) + ".png";
        cv::resize(img, img, faceSize, 1, 1);
        cv::imwrite(path, img);
        imsListFs << path << ";" << currentLabel << std::endl;
        imgNum[currentLabel]++;

        return path;
    }

    void Recognizer::draw(cv::Mat &img, cv::Scalar color) {
        cv::Scalar clr = std::move(color);
        for (const Face &f : faces) {
            // Color ->
            if (f.getLabel() == -1)
                clr = cv::Scalar(0, 0, 255);
            if (f.getLabel() == -2)
                clr = cv::Scalar(0, 125, 255);
            if (model->empty())
                clr = cv::Scalar(255, 0, 0);
            // <- Color

            cv::rectangle(img, f.rect, clr, 2, 4);

            // Label ->
            std::string text = std::to_string(f.confidence);
            if (f.getLabel() > -1) {
                std::string label = f.getLabel() < labels.size() ? labels[f.getLabel()] : "WTF";
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
        }
    }

    bool Recognizer::trainRecognizer(std::string imsList, std::string modelFile) {
        std::vector<cv::Mat> imgs;
        std::vector<int> labels;
        read_csv(imsList, imgs, labels);
        if (imgs.empty() || labels.empty() || imgs.size() != labels.size())
            return false;

        normalizeImages(imsList);
        model->train(imgs, labels);
        model->save(modelFile);
        return true;
    }

}