/**
 * @file example.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains some example/test code used during the development
 */

#include <Face/Face.h>

#include <Detector/OcvDefaultDnnDetector.h>

int main(int argc, char **argv) {
    auto console = spdlog::stdout_color_mt("console", spdlog::color_mode::always);
    spdlog::set_default_logger(console);

    faces::IDetector *detector = FACES_CREATE_INSTANCE(IDetector, OcvDefaultDnn,
                                                       "../../data/models/deploy.prototxt",
                                                       "../../data/models/res10_300x300_ssd_iter_140000_fp16.caffemodel");

    // does not work with std::string somehow
    // faces::IDetector *detector = FACES_CREATE_INSTANCE_DYNAMIC(IDetector, "Test", "hi");

    // faces::IDetector *detector = FACES_CREATE_INSTANCE_FN(IDetector)<std::string const&>("Test", "hi");

    if (detector == nullptr) {
        spdlog::error("Cannot initialize a face detector");
        return 1;
    }
    if (!detector->isOk()) {
        return 1;
    }

    cv::Mat test = cv::imread("../../data/test.jpg");
    std::vector<faces::Face> detected = detector->detect(test);
    for (const auto &f : detected) {
        cv::rectangle(test, f.rect, {0, 255, 0});
        std::cout << f.rect << " " << f.detectionConfidence << std::endl;
    }
    cv::imshow("test", test);
    cv::waitKey(0);

    return 0;
}