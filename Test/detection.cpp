//
// Created by prostoichelovek on 19.02.2020.
//

#include <chrono>

#include "../Faces.h"

#include "Datasets/Dataset_voc.h"
#include "Datasets/Dataset_SoF.h"

const std::string config_file = "../config.json";

template<typename T_a, typename T_b>
int find_similar(T_a &a, std::vector<T_b> &bs) {
    int minDist = -1;
    int r = -1;
    for (int i = 0; i < bs.size(); i++) {
        T_b &b = bs[i];

        cv::Point a_center(a.rect.x + a.rect.width / 2, a.rect.y + a.rect.height / 2);
        cv::Point b_center(b.rect.x + b.rect.width / 2, b.rect.y + b.rect.height / 2);

        int dist = getDist(a_center, b_center);
        if ((dist < minDist || minDist == -1) && dist <= 80) {
            minDist = dist;
            r = i;
        }
    }
    return r;
}

int main() {
    Faces::Faces faces(config_file);
    faces.detectFreq = 0;
    faces.recognition.minLabelNotChanged = 0;
    faces.recognizeFreq = 0;

    auto *dataset = Faces::Datasets::Dataset_voc::create(
            "/home/prostoichelovek/Documents/datasets/faces/dataset_from_dataturks/annotations_voc");
    int not_detected = 0;
    int false_detected = 0;
    int detected = 0;
    int total = 0;

    float time = 0;

    cv::Mat img, img2;

    for (int i = 1; i < 1000; i++) {
        auto annotation = dataset->get_annotation(i);
        img = annotation.images_loader.load();
        if (!annotation.ok || img.empty()) {
            break;
        }

        bool ok = true;

        img.copyTo(img2);

        int image_width = img.cols;

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        faces(img);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        time += (float) std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 100.0;

        faces.draw(img2, true, false);

        std::vector<std::pair<int, int>> pairs;

        for (int i = 0; i < faces.detector.faces.size(); i++) {
            Faces::Face &f = faces.detector.faces[i];
            int similar_id = find_similar(f, annotation.annotations);
            if (similar_id != -1) {
                auto &similar = annotation.annotations[similar_id];
                pairs.emplace_back(std::make_pair(i, similar_id));

                f.draw(img2, &faces.recognition.labels, cv::Scalar(0, 255, 0));

                detected++;
            } else {
                f.draw(img2, &faces.recognition.labels, cv::Scalar(0, 0, 255));

                false_detected++;
                ok = false;
            }
        }

        for (auto &a : annotation.annotations) {
            int similar_id = find_similar(a, faces.detector.faces);
            if (similar_id != -1) {
                a.draw(img, cv::Scalar(0, 255, 0));
            } else {
                a.draw(img, cv::Scalar(0, 0, 255));

                not_detected++;
                ok = false;
            }
            total++;
        }

        cv::hconcat(img, img2, img);
        cv::line(img, cv::Point(image_width, 0), cv::Point(image_width, img.rows), cv::Scalar(0, 0, 255));

        for (const auto &p : pairs) {
            Faces::Face &f = faces.detector.faces[p.first];
            auto &similar = annotation.annotations[p.second];
            cv::Point a_pt(similar.rect.x + similar.rect.width, similar.rect.y + similar.rect.height / 2);
            cv::Point b_pt(image_width + f.rect.x, f.rect.y + f.rect.height / 2);
            cv::line(img, a_pt, b_pt, cv::Scalar(0, 255, 255), 2);
        }

        if(img.cols > 900 || img.rows > 1300) {
            cv::resize(img, img, cv::Size(img.cols / 1.5, img.rows / 1.5));
        }

        if (!ok) {
            cv::imwrite("../Test/fails/" + std::to_string(total) + ".jpg", img);
        }

        // cv::imshow("result", img);
        // cv::waitKey(0);
    }

    log(INFO, "Not detected:", not_detected, "/", total);
    log(INFO, "False detected:", false_detected, "/", total);
    log(INFO, "Successfully detected:", detected, "/", total);
    log(INFO, "Time elapsed: ", time, "s / ", total, "faces");
    log(INFO, "Average time: ", time / (float) total, "s / face");


    return EXIT_SUCCESS;
}