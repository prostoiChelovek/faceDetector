//
// Created by prostoichelovek on 01.03.2020.
//

#include <chrono>

#include "../Faces.h"

#include "Datasets/Dataset_voc.h"
#include "Datasets/Dataset_SoF.h"

using namespace Faces::Datasets;

const std::string data_dir = "../Test/";
const std::string config_file = "../config4test.json";
const int num_train_samples = 5;
const int train_samples_limit = 15;

template<typename T, typename T_a>
void get_ids(T *dataset, std::vector<T_a> &annotations) {
    for (int i = 1;; i++) {
        auto annotation = dataset->get_annotation(i);
        if (!annotation.ok) {
            break;
        }
        const std::string &id = annotation.annotations.back().id;
        if (std::find_if(annotations.begin(), annotations.end(),
                         [&id](const Annotation_SoF &a) -> bool { return a.annotations.back().id == id; }) ==
            annotations.end()) {
            annotations.emplace_back(annotation);
        }
    }
}

template<typename T_a, typename T>
std::vector<T_a> get_annotations_for(T *dataset, const std::string &id, int num = 1, int offset = 0) {
    std::vector<T_a> res;
    for (int i = 1; res.size() < num; i++) {
        auto annotation = dataset->get_annotation(i);
        if (!annotation.ok) {
            break;
        }
        if (annotation.annotations.back().id == id) {
            res.emplace_back(annotation);
        }
    }
    if (offset != 0) {
        if (offset < res.size()) {
            res.erase(res.begin(), res.begin() + offset);
        }
    }
    return res;
}

template<typename T, typename T_a>
void add_samples(T *dataset, Faces::Faces &faces, std::vector<T_a> &annotations) {
    int i = 0;
    int j = 0;
    for (auto &a : annotations) {
        if (i > train_samples_limit) {
            break;
        }
        auto a_obj = a.annotations.back();
        faces.recognition.addLabel(a_obj.id);

        for (auto &annotation : get_annotations_for<Annotation_SoF>(dataset, a_obj.id, num_train_samples)) {
            faces.detector.faces.clear();
            auto annotation_obj = annotation.annotations.back();
            cv::Mat sample = annotation.images_loader.load(Occlusion_SoF::EYE,
                                                           Image_filters_SoF::NORMAL,
                                                           Difficulty_SoF::ORIGINAL);

            Faces::Face face(annotation_obj.rect.tl(), annotation_obj.rect.br(), faces.recognition.currentLabel);
            faces.detector.faces.emplace_back(face);
            faces.detector.faces.back().img = faces.detector.normalizeFaces(sample)[0];

            faces.recognition.getDescriptors(faces.detector.faces.back());
            faces.recognition.addSample(faces.cfg.samples_directory, faces.detector.faces.back());

            /*annotation_obj.draw(sample, cv::Scalar(0, 255, 0));
            cv::imwrite("../Test/samples/" + std::to_string(j) + ".jpg", sample);
            j++;*/
        }
        i++;
    }
}

int main(int argc, char **argv) {
    Faces::Faces faces(config_file);
    faces.detectFreq = 0;
    faces.recognition.minLabelNotChanged = 0;
    faces.recognizeFreq = 0;

    int successfully = 0;
    int failed = 0;

    auto *dataset = ::Dataset_SoF::create(
            "/home/prostoichelovek/Documents/datasets/faces/sof/metadata/metadata.json",
            "/home/prostoichelovek/Documents/datasets/faces/sof/images");

    std::vector<Annotation_SoF> annotations;
    get_ids(dataset, annotations);
    if (argc > 1) {
        system("rm ../data/samples_test -r");
        system("rm ../data/labels_test.txt -r");
        add_samples(dataset, faces, annotations);

        faces.recognition.train(faces.cfg.samples_directory);
        faces.recognition.save(faces.cfg.recognition_classifier);
    }

    int i = 0;
    for (auto &a : annotations) {
        if (i > train_samples_limit) {
            break;
        }
        auto a_obj = a.annotations.back();
        for (auto &annotation : get_annotations_for<Annotation_SoF>(dataset, a_obj.id, num_train_samples * 2,
                                                                    num_train_samples)) {
            faces.detector.faces.clear();

            auto annotation_obj = annotation.annotations.back();
            cv::Mat sample = annotation.images_loader.load(Occlusion_SoF::EYE,
                                                           Image_filters_SoF::NORMAL,
                                                           Difficulty_SoF::ORIGINAL);

            Faces::Face face(annotation_obj.rect.tl(), annotation_obj.rect.br(), faces.recognition.currentLabel);
            faces.detector.faces.emplace_back(face);
            faces.detector.faces.back().img = faces.detector.normalizeFaces(sample)[0];

            faces.recognition.recognize(faces.detector.faces.back());
            auto real_label_itr = find(faces.recognition.labels.begin(), faces.recognition.labels.end(),
                                       annotation_obj.id);
            int real_label_id = std::distance(faces.recognition.labels.begin(), real_label_itr);

            int predicted_id = faces.detector.faces.back().label;
            if (real_label_id == predicted_id) {
                successfully++;
            } else {
                failed++;
                annotation_obj.id += ", not " + faces.recognition.labels[predicted_id];
                annotation_obj.draw(sample, cv::Scalar(0, 255, 0));
                std::string path = data_dir + "/recognition_fails/" + std::to_string(real_label_id) + "_" +
                                   std::to_string(predicted_id) + "_" + std::to_string(failed) + ".jpg";
                cv::imwrite(path, sample);
            }
        }
        i++;
    }

    log(INFO, "Recognized successfully:", successfully);
    log(INFO, "Recognized wrong:", failed);

    return EXIT_SUCCESS;
}