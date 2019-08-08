//
// Created by prostoichelovek on 08.08.19.
//

#include "FaceChecker.h"

namespace Faces {

    FaceChecker::FaceChecker(std::string classifierPath)
            : classifierPath(std::move(classifierPath)) {
        ok = load();
    }

    void FaceChecker::create() {
        classifier = cv::ml::SVM::create();
        classifier->setType(cv::ml::SVM::C_SVC);
        classifier->setKernel(cv::ml::SVM::LINEAR);
        classifier->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
    }

    void FaceChecker::addTrainSample(cv::Mat &faceDisp, std::string dir, bool isReal) {
        createDirNotExists(dir);

        std::string filename = dir + "/";
        if (isReal)
            filename += "real";
        else
            filename += "fake";
        filename += ".csv";
        std::ofstream histFs(filename, std::ios::app);
        if (!histFs) {
            log(ERROR, "Cannot open file stream for face histogram sample", filename);
            return;
        }

        cv::Mat hist = calculateHist(faceDisp);
        histFs << cv::format(hist.t(), cv::Formatter::FMT_CSV) << std::endl;

        histFs.close();
    }

    void FaceChecker::train(std::string samplesDir) {
        std::vector<std::vector<int>> trueHists = read_csv<int>(samplesDir + "/real.csv");
        if (trueHists.empty()) {
            log(ERROR, "Cannot read true samples from", samplesDir + "/real.csv");
            return;
        }

        std::vector<std::vector<int>> fakeHists = read_csv<int>(samplesDir + "/fake.csv");
        if (trueHists.empty()) {
            log(ERROR, "Cannot read fake samples from", samplesDir + "/fake.csv");
            return;
        }

        std::vector<std::vector<int>> trainingData;
        trainingData.insert(trainingData.end(), trueHists.begin(), trueHists.end());
        trainingData.insert(trainingData.end(), fakeHists.begin(), fakeHists.end());

        std::vector<int> trainingLabels;
        std::for_each(trueHists.begin(), trueHists.end(), [&](const auto &e) {
            trainingLabels.emplace_back(1);
        });
        std::for_each(fakeHists.begin(), fakeHists.end(), [&](const auto &e) {
            trainingLabels.emplace_back(-1);
        });

        cv::Mat trainingDataMat = toMat(trainingData);
        trainingDataMat.convertTo(trainingDataMat, CV_32F);
        cv::Mat trainingLabelsMat(trainingLabels.size(), 1, CV_32SC1, trainingLabels.data());

        if (!ok)
            create();
        ok = classifier->train(trainingDataMat, cv::ml::ROW_SAMPLE, trainingLabelsMat);
    }

    cv::Mat FaceChecker::calculateHist(cv::Mat faceDisp) {
        int histSize = 256;    // bin size
        float range[] = {0, 255};
        const float *ranges[] = {range};

        cv::Mat hist;
        cv::calcHist(&faceDisp, 1, nullptr, cv::Mat(), hist, 1, &histSize, ranges, true, false);

        return hist;
    }

    void FaceChecker::save() {
        if (!ok)
            return;
        classifier->save(classifierPath);
    }

    bool FaceChecker::load() {
        try {
            classifier = cv::ml::SVM::load(classifierPath);
            if (classifier->empty()) {
                log(ERROR, "Cannot load face histogram validator from", classifierPath);
            } else {
                return true;
            }
        } catch (std::exception &e) {
            log(ERROR, "Cannot load face histogram validator from", classifierPath, ":", e.what());
            return false;
        }
        return false;
    }

    bool FaceChecker::check(cv::Mat &faceDisp) {
        if (!ok)
            return true;

        cv::Mat hist = calculateHist(faceDisp);
        hist.convertTo(hist, CV_32F);
        hist = hist.t();
        float prediction = classifier->predict(hist);

        log(INFO, prediction);
        return prediction == 1;
    }


}