//
// Created by prostoichelovek on 25.07.19.
//

#include "Recognizer.h"

namespace Faces {

    bool normalizeImages(const std::string &filename, cv::Size faceSize, char separator) {
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
            cv::resize(img, img, faceSize);
            cv::imwrite(path, img);
        }
        return true;
    }


    Recognizer::Recognizer(Callbacks *callbacks, cv::Size faceSize)
            : callbacks(callbacks), faceSize(faceSize) {
        model = cv::face::LBPHFaceRecognizer::create();
    }

    Recognizer::~Recognizer() {
        labelsFs.close();
        imsListFs.close();
    }

    bool Recognizer::readModel(std::string file) {
        try {
            model->read(file);
        } catch (std::exception &e) {
            log(ERROR, "Cannot read face recognition model:", e.what());
            return false;
        }
        return !model->empty();
    }

    bool Recognizer::readLabels(std::string file) {
        if (!getFileContent(file, labels))
            return false;
        labelsFs = std::ofstream(file, std::ios::app);
        if (!labelsFs) {
            log(ERROR, "Could not open labels file for write", file);
            return false;
        }
        return true;
    }

    bool Recognizer::readImageList(std::string file) {
        imsListFs = std::ofstream(file, std::ios::app);
        if (!imsListFs.is_open()) {
            log(ERROR, "Could not open image list file", file);
            return false;
        }
        std::vector<cv::Mat> imgs;
        std::vector<int> lbls;
        read_csv(file, imgs, lbls);
        for (int lbl : lbls)
            imgNum[lbl]++;

        return true;
    }

    bool Recognizer::train(std::string imsList, std::string modelFile) {
        std::vector<cv::Mat> imgs;
        std::vector<int> labels;
        read_csv(imsList, imgs, labels);
        if (imgs.empty() || labels.empty() || imgs.size() != labels.size())
            return false;

        normalizeImages(imsList, faceSize);
        model->train(imgs, labels);
        model->save(modelFile);
        return true;
    }

    bool Recognizer::operator()(std::vector<Face> &faces) {
        for (auto &f : faces) {
            bool ok = operator()(f);
            if (!ok)
                return false;
        }
        return true;
    }

    bool Recognizer::operator()(Face &face) {
        if (!model->empty()) {
            cv::Mat gray;
            cv::cvtColor(face.img, gray, cv::COLOR_BGR2GRAY);
            cv::resize(gray, gray, faceSize, 1, 1);

            face.setLabel(model->predict(gray));

            if (callbacks != nullptr) {
                if (face.getLabel() > -1)
                    callbacks->call("faceRecognized", &face);
                if (face.getLabel() == -1)
                    callbacks->call("unknownFace", &face);
                if (face.getLabel() != -2 && face.last->getLabel() == -2)
                    callbacks->call("labelConfirmed", &face);
            }
            return true;
        }
        return false;
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

}