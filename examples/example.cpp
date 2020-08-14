/**
 * @file example.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains some example/test code used during the development
 */

#include <spdlog/sinks/stdout_color_sinks.h>

#include <Face/Face.h>

#include <Config/Config.h>
#include <Detector/Implementations/OcvDefaultDnnDetector.h>
#include <Recognizer/Implementations/Descriptors/DlibResnetSvmRecognizer.h>
#include "miniconf.h"

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

    faces::Recognizer *recognizer = FACES_CREATE_INSTANCE(Recognizer, DlibResnetSvm, configInstance);

    if (detector == nullptr || recognizer == nullptr) {
        spdlog::error("Cannot initialize face detector or/and recognizer");
        return 1;
    }
    if (!detector->isOk() || !recognizer->isOk()) {
        spdlog::error("Cannot load face detector or/and recognizer");
        return 1;
    }

    cv::Mat test = cv::imread("../../data/test.jpg");

    std::vector<faces::Face> detected = detector->detect(test);
    recognizer->recognize(detected);
    for (auto &f : detected) {
        cv::rectangle(test, f.rect, {0, 255, 0});
        cv::putText(test, std::to_string(f.label), f.rect.tl(),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        std::cout << f.rect << " " << f.label << std::endl;
    }

    cv::imshow("test", test);
    cv::waitKey(0);

    return 0;
}