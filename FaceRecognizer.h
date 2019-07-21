//
// Created by prostoichelovek on 08.05.19.
//

#ifndef FACES_FACERECOGNIZER_H
#define FACES_FACERECOGNIZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/face.hpp>
#include <opencv2/tracking.hpp>

namespace FaceRecognizer {

    extern cv::Size faceSize;

    bool getFileContent(std::string fileName, std::vector<std::string> &vecOfStrs);

    bool
    read_csv(const std::string &filename, std::vector<cv::Mat> &images, std::vector<int> &labels, char separator = ';');

    bool normalizeImages(const std::string &filename, char separator = ';');

    double getDist(cv::Point a, cv::Point b);

    struct Face {
        cv::Rect rect;
        int label = -1;

        int confidence = 0;

        cv::Point offset; // distance between current position and last
        Face *last = nullptr;

        const static int minOffset = 5;

        Face() = default;

        Face(cv::Point p1, cv::Point p2, int label);

        bool checkBounds(const cv::Size &imgSize);
    };


    class FaceRecognizer {
    public:
        cv::Size inSize = cv::Size(300, 300);
        double inScaleFactor = 1.0;
        float confidenceThreshold = 0.7;
        cv::Scalar meanVal = cv::Scalar(104.0, 177.0, 123.0);

        cv::dnn::Net net;
        cv::Ptr<cv::face::LBPHFaceRecognizer> model;

        std::vector<Face> faces;
        std::vector<Face> lastFaces;

        int currentLabel = 0;
        std::vector<std::string> labels;
        std::map<int, int> imgNum;

        FaceRecognizer();

        ~FaceRecognizer();

        bool trainRecognizer(std::string imsList, std::string modelFile);

        bool detectFaces(cv::Mat &img);

        bool recognizeFaces(cv::Mat &img);

        bool operator()(cv::Mat &img);

        void sortFacesByScore();

        void preventOverlapping();

        Face *getLastFace(Face &now);

        bool readNet(std::string caffeConfigFile, std::string caffeWeightFile);

        bool readRecognitionModel(std::string file);

        bool readLabels(std::string file);

        bool readImageList(std::string file);

        void addLabel(std::string &label);

        std::string addTrainImage(std::string imagesDir, cv::Mat &img);

        void draw(cv::Mat &img, cv::Scalar color = cv::Scalar(0, 255, 0));

    private:
        std::ofstream labelsFs;
        std::ofstream imsListFs;
    };

}

#endif //FACES_FACERECOGNIZER_H
