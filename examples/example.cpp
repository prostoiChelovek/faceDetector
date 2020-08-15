/**
 * @file example.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains some example/test code used during the development
 */

#include <spdlog/sinks/stdout_color_sinks.h>

#include <miniconf.h>

#include <Face/Face.h>

#include <Config/Config.h>
#include <Detector/Implementations/OcvDefaultDnnDetector.h>
#include <Landmarker/Implementations/DlibLandmarker.h>
#include <Aligner/Implementations/DlibChipAligner.h>
#include <Recognizer/Implementations/Descriptors/DlibResnetSvmRecognizer.h>

int main(int argc, char **argv) {
    auto console = spdlog::stdout_color_mt("console", spdlog::color_mode::always);
    spdlog::set_default_logger(console);

    faces::Config &configInstance = faces::Config::getInstance();
    miniconf::Config &config = configInstance.config;
    std::string configFile = FACES_ROOT_DIRECTORY "/config.json";
    bool configOk = config.config(configFile);
    if (!configOk) {
        spdlog::warn("Cannot load a config from the file '{}', saving a default configuration", configFile);
        config.serialize(configFile);
    }

    config.print();

    faces::Detector *detector = FACES_CREATE_INSTANCE(Detector, OcvDefaultDnn, configInstance);
    faces::Landmarker *landmarker = FACES_CREATE_INSTANCE(Landmarker, Dlib, configInstance);
    faces::Aligner *aligner = FACES_CREATE_INSTANCE(Aligner, DlibChip, configInstance);
    faces::Recognizer *recognizer = FACES_CREATE_INSTANCE(Recognizer, DlibResnetSvm, configInstance);

    if (detector == nullptr || recognizer == nullptr || landmarker == nullptr || aligner == nullptr) {
        spdlog::error("Cannot initialize face detector, recognizer, landmarker or aligner");
        return 1;
    }
    if (!detector->isOk() || !recognizer->isOk() || !landmarker->isOk() || !aligner->isOk()) {
        spdlog::error("Cannot load face detector, recognizer, landmarker or aligner");
        return 1;
    }

    cv::Mat test = cv::imread(configInstance.getDataPath("testImage"));

    std::vector<faces::Face> detected = detector->detect(test);
    landmarker->detect(detected);
    aligner->align(detected, test);
    recognizer->recognize(detected);

    int i = 0;
    for (auto &f : detected) {
        cv::rectangle(test, f.rect, {0, 255, 0});
        cv::putText(test, std::to_string(f.label), f.rect.tl(),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, {255, 255, 255}, 2);

        for (cv::Point const &pt : f.landmarks) {
            cv::Point realPt = f.rect.tl() + pt;
            cv::circle(test, realPt, 2, {0, 0, 255}, cv::FILLED, cv::LINE_AA);
        }

        std::string faceWinName = std::to_string(i) + " " + std::to_string(f.label);
        cv::namedWindow(faceWinName, cv::WINDOW_GUI_NORMAL | cv::WINDOW_AUTOSIZE);
        cv::imshow(faceWinName, f.img);

        std::cout << f.rect << " " << f.label << std::endl;
        ++i;
    }

    cv::imshow("test", test);
    cv::waitKey(0);

    return 0;
}