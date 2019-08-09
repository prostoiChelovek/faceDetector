//
// Created by prostoichelovek on 08.08.19.
//

#include "FaceChecker.h"

namespace Faces {

    FaceChecker::FaceChecker(std::string classifierPath)
            : classifierPath(std::move(classifierPath)) {
        ok = load();
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
        std::vector<std::vector<double>> trueHists = read_csv<double>(samplesDir + "/real.csv");
        if (trueHists.empty()) {
            log(ERROR, "Cannot read true samples from", samplesDir + "/real.csv");
            return;
        }

        std::vector<std::vector<double>> fakeHists = read_csv<double>(samplesDir + "/fake.csv");
        if (trueHists.empty()) {
            log(ERROR, "Cannot read fake samples from", samplesDir + "/fake.csv");
            return;
        }

        std::vector<std::vector<double>> trainingData;
        trainingData.insert(trainingData.end(), trueHists.begin(), trueHists.end());
        trainingData.insert(trainingData.end(), fakeHists.begin(), fakeHists.end());

        std::vector<double> trainingLabels;
        std::for_each(trueHists.begin(), trueHists.end(), [&](const auto &e) {
            trainingLabels.emplace_back(+1);
        });
        std::for_each(fakeHists.begin(), fakeHists.end(), [&](const auto &e) {
            trainingLabels.emplace_back(-1);
        });

        std::vector<sample_type> samples;
        std::for_each(trainingData.begin(), trainingData.end(), [&](const auto &data) {
            samples.emplace_back(dlib::mat(data));
        });

        trainer_type trainer;
        trainer.set_kernel(kernel_type());
        trainer.set_c(10);

        classifier = trainer.train(samples, trainingLabels);
        ok = true;
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
        try {
            dlib::serialize(classifierPath) << classifier;
        } catch (dlib::serialization_error &e) {
            log(ERROR, "Cannot save face histogram validator to", classifierPath, ":", e.what());
        }
    }

    bool FaceChecker::load() {
        try {
            dlib::deserialize(classifierPath) >> classifier;
        } catch (dlib::serialization_error &e) {
            log(ERROR, "Cannot load face histogram validator from", classifierPath, ":", e.what());
            return false;
        }
        return false;
    }

    bool FaceChecker::check(cv::Mat &faceDisp) {
        if (!ok)
            return true;

        cv::Mat hist = calculateHist(faceDisp);
        std::vector<double> histVec = hist;
        sample_type histMatr = dlib::mat(histVec);

        double prediction = classifier(histMatr);

        log(INFO, prediction);
        return prediction == 1;
    }


}