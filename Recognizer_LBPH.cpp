//
// Created by prostoichelovek on 27.07.19.
//

#include "Recognizer_LBPH.h"

namespace Faces {

    Recognizer_LBPH::Recognizer_LBPH(Callbacks *callbacks, cv::Size faceSize, std::string modelFile)
            : Recognizer(callbacks, faceSize) {
        model = cv::face::LBPHFaceRecognizer::create();
        load(modelFile);
    }

    void Recognizer_LBPH::recognize(Face &face) {
        if (!ok)
            return;

        cv::Mat gray;
        cv::cvtColor(face.img, gray, cv::COLOR_BGR2GRAY);

        face.setLabel(model->predict(gray));
    }

    void Recognizer_LBPH::train(std::string samplesDir) {
        auto samples = getSamples(samplesDir);

        std::vector<cv::Mat> imgs;
        std::vector<int> labels;

        for (auto &file : samples) {
            cv::Mat img = cv::imread(samplesDir + "/" + file.first, cv::IMREAD_GRAYSCALE);
            imgs.emplace_back(img);
            labels.emplace_back(file.second);
        }

        model->train(imgs, labels);
    }

    bool Recognizer_LBPH::save(std::string file) {
        try {
            model->save(file);
            return true;
        } catch (std::exception &e) {
            log(ERROR, "Cannot save LBPH face recognition model to", file, ":", e.what());
            return false;
        }
    }

    std::map<std::string, int> Recognizer_LBPH::getSamples(std::string dir) {
        std::vector<std::string> names = list_directory(dir, "png");

        std::map<std::string, int> samples;

        for (int i = 0; i < labels.size(); i++) {
            for (std::string &name : names) {
                if (name.find(std::to_string(i) + "_") != std::string::npos) {
                    samples[name] = i;
                }
            }
        }

        return samples;
    }

    std::string Recognizer_LBPH::addSample(std::string storage, Face &face) {
        createDirNotExists(storage);
        std::string path = storage + "/" + std::to_string(currentLabel)
                           + "_" + std::to_string(imgNum[currentLabel]) + ".png";

        cv::Mat img = face.img.clone();
        cv::resize(img, img, faceSize, 0, 0);
        cv::imwrite(path, img);

        imgNum[currentLabel]++;
        return path;
    }

    void Recognizer_LBPH::setThreshold(double val) {
        model->setThreshold(val);
    }

    void Recognizer_LBPH::load(std::string modelFile) {
        try {
            model->read(modelFile);
            ok = true;
        } catch (std::exception &e) {
            log(ERROR, "Cannot read face recognition model from", modelFile, ":\n", e.what());
        }
        if (model->empty())
            log(ERROR, "Cannot read face recognition model from", modelFile);
    }

}