//
// Created by prostoichelovek on 08.05.19.
//

#include "FaceRecognizer.h"

namespace FaceRecognizer {

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

    FaceRecognizer::FaceRecognizer() {
        model = cv::face::LBPHFaceRecognizer::create();
    }

    FaceRecognizer::FaceRecognizer(cv::Size capSize) : capSize(std::move(capSize)) {
        model = cv::face::LBPHFaceRecognizer::create();
    }

    FaceRecognizer::~FaceRecognizer() {
        labelsFs.close();
        imsListFs.close();
    }

    Face *FaceRecognizer::getLastFace(Face &now) {
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


    bool FaceRecognizer::readNet(std::string caffeConfigFile, std::string caffeWeightFile) {
        net = cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile);
        if (net.empty()) {
            fprintf(stderr, "Could not load net (%s, %s)", caffeConfigFile.c_str(), caffeWeightFile.c_str());
            return false;
        }
        return true;
    }


    bool FaceRecognizer::readRecognitionModel(std::string file) {
        model->read(file);
        if (model->empty()) {
            std::cerr << "Could not load face recognition model " << file << std::endl;
            return false;
        }
        return true;
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
        for (int i = 0; i < lbls.size(); i++)
            imgNum[lbls[i]]++;

        return true;
    }

    bool FaceRecognizer::detectFaces(cv::Mat &img) {
        cv::Mat inputBlob = cv::dnn::blobFromImage(img, inScaleFactor, capSize, meanVal,
                                                   false, false);
        net.setInput(inputBlob, "data");
        cv::Mat detection = net.forward("detection_out");

        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        faces.clear();

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > confidenceThreshold) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * capSize.width);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * capSize.height);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * capSize.width);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * capSize.height);

                cv::Rect capRect = cv::Rect(0, 0, capSize.width, capSize.height);
                cv::Point pt1(x1, y1), pt2(x2, y2);
                if (!capRect.contains(pt1) || !capRect.contains(pt2))
                    continue;

                cv::Mat gray;
                cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

                Face f;
                f.rect = cv::Rect(pt1, pt2);

                if (!model->empty())
                    f.label = model->predict(gray(f.rect));

                f.last = getLastFace(f);
                if (f.last != nullptr) {
                    f.offset.x = f.rect.x - f.last->rect.x;
                    f.offset.y = f.rect.y - f.last->rect.y;
                }

                faces.emplace_back(f);
            }
        }

        return !faces.empty();
    }


    std::string FaceRecognizer::addLabel() {
        std::cout << "Label: ";
        std::string lbl;
        std::getline(std::cin, lbl);
        labels.emplace_back(lbl);
        currentLabel = labels.size() - 1;
        imgNum[currentLabel] = 0;
        labelsFs << lbl << "\n";
        labelsFs.flush();

        return lbl;
    }

    std::string FaceRecognizer::addTrainImage(std::string imagesDir, cv::Mat &img) {
        std::string path = imagesDir + "/" + std::to_string(currentLabel)
                           + "_" + std::to_string(imgNum[currentLabel]) + ".png";
        cv::imwrite(path, img);
        imsListFs << path << ";" << currentLabel << std::endl;
        imgNum[currentLabel]++;

        return path;
    }

    void FaceRecognizer::draw(cv::Mat &img, cv::Scalar color) {
        for (const Face &f : faces) {
            cv::rectangle(img, f.rect, color, 2, 4);

            if (f.label != -1) {
                std::string label = f.label < labels.size() ? labels[f.label] : "";
                putText(img, label, cv::Point(f.rect.x, f.rect.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.9,
                        color, 2);
            }

            if (f.last != nullptr)
                putText(img, f.getMovingDir(), cv::Point(f.rect.x, f.rect.y - 25), cv::FONT_HERSHEY_SIMPLEX, 0.9,
                        color, 2);
        }
    }

    bool FaceRecognizer::trainRecognizer(std::string imsList, std::string modelFile) {
        std::vector<cv::Mat> imgs;
        std::vector<int> labels;
        read_csv(imsList, imgs, labels);
        if (imgs.empty() || labels.empty() || imgs.size() != labels.size())
            return false;

        model->train(imgs, labels);
        model->save(modelFile);
        return true;
    }

}