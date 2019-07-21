//
// Created by prostoichelovek on 08.05.19.
//

#include "FaceRecognizer.h"

namespace FaceRecognizer {

    cv::Size faceSize = cv::Size(200, 240);

    bool getFileContent(std::string fileName, std::vector<std::string> &vecOfStrs) {
        std::ifstream in(fileName.c_str());
        if (!in) {
            std::cerr << "Cannot open the file: " << fileName << std::endl;
            return false;
        }
        std::string str;
        while (std::getline(in, str)) {
            if (!str.empty())
                vecOfStrs.push_back(str);
        }
        in.close();
        return true;
    }

    bool read_csv(const std::string &filename, std::vector<cv::Mat> &images, std::vector<int> &labels,
                  char separator) {
        std::ifstream file(filename.c_str(), std::ifstream::in);
        if (!file) {
            std::cerr << "No valid input file was given, please check the given filename." << std::endl;
            return false;
        }
        std::string line, path, classlabel;
        while (getline(file, line)) {
            std::stringstream liness(line);
            getline(liness, path, separator);
            getline(liness, classlabel);
            if (!path.empty() && !classlabel.empty()) {
                images.push_back(cv::imread(path, 0));
                labels.push_back(atoi(classlabel.c_str()));
            }
        }
        return labels.size() == images.size();
    }

    double getDist(cv::Point a, cv::Point b) {
        return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
    }

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


    Face::Face(cv::Point p1, cv::Point p2, int label) : label(label) {
        rect = cv::Rect(p1, p2);
    }

    std::string Face::getMovingDir() const {
        std::string dir;
        if (offset.x >= minOffset)
            dir += " right";
        if (-offset.x >= minOffset)
            dir += " left";
        if (-offset.y >= minOffset)
            dir += " top";
        if (offset.y >= minOffset)
            dir += " down";
        return dir;
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


    FaceRecognizer::FaceRecognizer() {
        model = cv::face::LBPHFaceRecognizer::create();
    }

    FaceRecognizer::~FaceRecognizer() {
        labelsFs.close();
        imsListFs.close();
    }

    Face *FaceRecognizer::getLastFace(Face &now) {
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


    bool FaceRecognizer::readNet(std::string caffeConfigFile, std::string caffeWeightFile) {
        net = cv::dnn::readNet(caffeConfigFile, caffeWeightFile);
        if (net.empty()) {
            fprintf(stderr, "Could not load net (%s, %s)", caffeConfigFile.c_str(), caffeWeightFile.c_str());
            return false;
        }
        return true;
    }


    bool FaceRecognizer::readRecognitionModel(std::string file) {
        try {
            model->read(file);
        } catch (std::exception &e) {
            std::cerr << "Cannot read face recognition model: " << e.what() << std::endl;
            return false;
        }
        return !model->empty();
    }

    bool FaceRecognizer::readLabels(std::string file) {
        if (!getFileContent(file, labels))
            return false;
        labelsFs = std::ofstream(file, std::ios::app);
        if (!labelsFs) {
            std::cerr << "Could not open labels file for write " << file << std::endl;
            return false;
        }
        return true;
    }

    bool FaceRecognizer::readImageList(std::string file) {
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

    bool FaceRecognizer::detectFaces(cv::Mat &img) {
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

                f.last = getLastFace(f);
                if (f.last != nullptr) {
                    f.offset.x = f.rect.x - f.last->rect.x;
                    f.offset.y = f.rect.y - f.last->rect.y;
                }

                faces.emplace_back(f);
            }
        }

        sortFacesByScore();
        preventOverlapping();

        return !faces.empty();
    }

    bool FaceRecognizer::recognizeFaces(cv::Mat &img) {
        if (!model->empty()) {
            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            for (Face &f : faces) {
                cv::Mat resized = gray.clone()(f.rect);
                cv::resize(resized, resized, faceSize, 1, 1);
                f.label = model->predict(resized);
            }
            return true;
        }
        return false;
    }

    bool FaceRecognizer::operator()(cv::Mat &img) {
        if (!detectFaces(img)) return false;
        return recognizeFaces(img);
    }

    void FaceRecognizer::sortFacesByScore() {
        // in most cases, false predictions occurrence when face is too close to camera
        auto countScore = [&](const Face &f) -> int {
            int score = f.confidence;
            if (f.last != nullptr)
                score += f.last->confidence;
            if (!model->empty()) {
                if (f.label != -1)
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

    void FaceRecognizer::preventOverlapping() {
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

    void FaceRecognizer::addLabel(std::string &label) {
        labels.emplace_back(label);
        currentLabel = labels.size() - 1;
        imgNum[currentLabel] = 0;
        labelsFs << label << "\n";
        labelsFs.flush();
    }

    std::string FaceRecognizer::addTrainImage(std::string imagesDir, cv::Mat &img) {
        std::string path = imagesDir + "/" + std::to_string(currentLabel)
                           + "_" + std::to_string(imgNum[currentLabel]) + ".png";
        cv::resize(img, img, faceSize, 1, 1);
        cv::imwrite(path, img);
        imsListFs << path << ";" << currentLabel << std::endl;
        imgNum[currentLabel]++;

        return path;
    }

    void FaceRecognizer::draw(cv::Mat &img, cv::Scalar color) {
        cv::Scalar clr = std::move(color);
        for (const Face &f : faces) {
            if (f.label == -1)
                clr = cv::Scalar(0, 0, 255);
            if (model->empty())
                clr = cv::Scalar(255, 0, 0);

            if (f.label != -1) {
                std::string label = f.label < labels.size() ? labels[f.label] : "WTF";
                putText(img, label, cv::Point(f.rect.x, f.rect.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.9,
                        clr, 2);
            }
            cv::rectangle(img, f.rect, clr, 2, 4);

            putText(img, std::to_string(f.confidence), cv::Point(f.rect.x, f.rect.y - 50),
                    cv::FONT_HERSHEY_SIMPLEX, 0.9, clr, 2);

            if (f.last != nullptr)
                putText(img, f.getMovingDir(), cv::Point(f.rect.x, f.rect.y - 25),
                        cv::FONT_HERSHEY_SIMPLEX, 0.9, clr, 2);
        }
    }

    bool FaceRecognizer::trainRecognizer(std::string imsList, std::string modelFile) {
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