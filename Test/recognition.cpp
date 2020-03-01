//
// Created by prostoichelovek on 01.03.2020.
//

#include <chrono>

#include "../Faces.h"

#include "Datasets/Dataset_voc.h"
#include "Datasets/Dataset_SoF.h"

using namespace Faces::Datasets;

const std::string config_file = "../config4test.json";

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
std::vector<T_a> get_annotations_for(T *dataset, const std::string &id, int num = 1) {
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
    return res;
}

template<typename T, typename T_a>
void add_samples(T *dataset, Faces::Faces &faces, std::vector<T_a> &annotations) {
    for (auto &a : annotations) {

        auto a_obj = a.annotations.back();
        faces.recognition.addLabel(a_obj.id);

        for (auto &annotation : get_annotations_for<Annotation_SoF>(dataset, a_obj.id, 5)) {
            faces.detector.faces.clear();
            auto annotation_obj = a.annotations.back();
            cv::Mat sample = annotation.images_loader.load(Occlusion_SoF::EYE,
                                                           Image_filters_SoF::NORMAL,
                                                           Difficulty_SoF::ORIGINAL);

            Faces::Face face(annotation_obj.rect.tl(), annotation_obj.rect.br(), faces.recognition.currentLabel);
            faces.detector.faces.emplace_back(face);
            faces.detector.faces.back().img = faces.detector.normalizeFaces(sample)[0];

            faces.recognition.addSample(faces.cfg.samples_directory, faces.detector.faces.back());
        }
    }
}

int main(int argc, char **argv) {
    Faces::Faces faces(config_file);
    faces.detectFreq = 0;
    faces.recognition.minLabelNotChanged = 0;
    faces.recognizeFreq = 0;

    auto *dataset = ::Dataset_SoF::create(
            "/home/prostoichelovek/Documents/datasets/faces/sof/metadata/metadata.json",
            "/home/prostoichelovek/Documents/datasets/faces/sof/images");

    cv::Mat img, img2;

    std::vector<Annotation_SoF> annotations;
    get_ids(dataset, annotations);
    if (argc > 1) {
        add_samples(dataset, faces, annotations);

        faces.recognition.train(faces.cfg.samples_directory);
        faces.recognition.save(faces.cfg.recognition_classifier);
    }

    return EXIT_SUCCESS;
}