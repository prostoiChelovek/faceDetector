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
int num_train_samples = 4;
int max_persons = 4;
std::string postfix_str;

Occlusion_SoF occlusion = Occlusion_SoF::EYE;
Image_filters_SoF filters = Image_filters_SoF::NORMAL;
Difficulty_SoF difficulty = Difficulty_SoF::ORIGINAL;

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
    int j = 0;
    for (auto &a : annotations) {
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
    }
}

template<typename T, typename T_a>
std::tuple<int, int>
test(T *dataset, std::vector<T_a> &annotations, Faces::Faces &faces, const std::vector<int> &trained_ids) {
    int successfully = 0, failed = 0;
    for (const int &id : trained_ids) {
        std::string id_str = faces.recognition.labels[id];
        for (auto &annotation : get_annotations_for<Annotation_SoF>(dataset, id_str, num_train_samples + 3,
                                                                    num_train_samples)) {
            faces.detector.faces.clear();

            auto annotation_obj = annotation.annotations.back();
            cv::Mat sample = annotation.images_loader.load(occlusion, filters, difficulty);

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
                annotation_obj.id +=
                        ", not " + (predicted_id >= 0 ? faces.recognition.labels[predicted_id] : "unknown");
                annotation_obj.draw(sample, cv::Scalar(0, 255, 0));
                std::string path = data_dir + "/recognition_fails_3/" + std::to_string(real_label_id) + "_" +
                                   std::to_string(predicted_id) + "_" + std::to_string(failed);
                path += "_" + postfix_str + ".jpg";
                cv::imwrite(path, sample);
            }
        }
    }
    return std::make_tuple(successfully, failed);
}

template<typename T, typename T_a>
void test_numbers(T *dataset, std::vector<T_a> &annotations, Faces::Faces &faces) {
    std::ofstream log_fs("../Test/log.txt", std::ios::app);
    log_fs << "==========================================" << std::endl;

    for (; num_train_samples < 20; num_train_samples += 4) {
        for (max_persons = num_train_samples; max_persons < 61; max_persons += 20) {
            std::vector<int> trained_ids = faces.recognition.train(faces.cfg.samples_directory, max_persons,
                                                                   num_train_samples);
            faces.recognition.save(faces.cfg.recognition_classifier);

            int successfully = 0, failed = 0;
            std::tie(successfully, failed) = test(dataset, annotations, faces, trained_ids);
            int success_percent = ((float) successfully / (float) (successfully + failed)) * 100;

            log(INFO, "Number of train samples:", num_train_samples);
            log(INFO, "Number of persons:", max_persons);
            log(INFO, "Recognized successfully:", successfully);
            log(INFO, "Recognized wrong:", failed);
            log(INFO, "Success percent:", success_percent);
            log(INFO, "----------------");
            log_fs << "Number of train samples: " << num_train_samples << std::endl;
            log_fs << "Number of persons: " << max_persons << std::endl;
            log_fs << "Recognized successfully: " << successfully << std::endl;
            log_fs << "Recognized wrong: " << failed << std::endl;
            log_fs << "Success percent: " << success_percent << std::endl;
            log_fs << "-----------------" << std::endl;
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

    std::vector<Annotation_SoF> annotations;
    get_ids(dataset, annotations);
    if (argc > 1 && strcmp(argv[1], "a") == 0) {
        log(INFO, "Adding samples....");
        system("rm ../data/samples_test -r");
        system("rm ../data/labels_test.txt -r");
        add_samples(dataset, faces, annotations);
    } else {
        for (auto &a : annotations) {
            auto a_obj = a.annotations.back();
            faces.recognition.addLabel(a_obj.id);
        }
    }

    if (argc > 1 && strcmp(argv[1], "b") == 0) {
        log(INFO, "Testing different numbers of persons and training samples...");
        test_numbers(dataset, annotations, faces);
    }

    if (argc > 1 && strcmp(argv[1], "c") == 0) {
        std::vector<int> trained_ids = faces.recognition.train(faces.cfg.samples_directory, max_persons,
                                                               num_train_samples);

        log(INFO, "Testing different difficulties...");
        std::ofstream log_fs("../Test/log2.txt", std::ios::app);
        log_fs << "==========================================" << std::endl;
        std::vector<std::pair<Occlusion_SoF, std::vector<std::pair<std::vector<Image_filters_SoF>, std::vector<Difficulty_SoF>>>>> params{
                {Occlusion_SoF::EYE,           {
                                                       {{Image_filters_SoF::NORMAL}, {Difficulty_SoF::ORIGINAL}},
                                                       {{Image_filters_SoF::NOISE, Image_filters_SoF::SMOOTH, Image_filters_SoF::POSTERIZE},
                                                               {Difficulty_SoF::EASY, Difficulty_SoF::MEDIUM, Difficulty_SoF::HARD}}
                                               }},
                {Occlusion_SoF::EYE_AND_NOSE,  {
                                                       {{Image_filters_SoF::NORMAL}, {Difficulty_SoF::EASY, Difficulty_SoF::MEDIUM, Difficulty_SoF::HARD}}
                                               }},
                {Occlusion_SoF::EYE_AND_MOUTH, {
                                                       {{Image_filters_SoF::NORMAL}, {Difficulty_SoF::EASY, Difficulty_SoF::MEDIUM, Difficulty_SoF::HARD}}
                                               }}
        };
        for (int i = 0; i < params.size(); ++i) {
            const auto &occlision_params = params[i];
            occlusion = occlision_params.first;
            for (int j = 1; j < occlision_params.second.size(); j++) {
                const auto &filter_difficulty = occlision_params.second[j];
                for (int k = 0; k < filter_difficulty.first.size(); k++) {
                    filters = filter_difficulty.first[k];
                    for (int l = 1; l < filter_difficulty.second.size(); l++) {
                        difficulty = filter_difficulty.second[l];

                        nlohmann::json tmp_json;
                        postfix_str.clear();
                        to_json(tmp_json, occlusion);
                        postfix_str += std::string(tmp_json) + "_";
                        to_json(tmp_json, filters);
                        postfix_str += std::string(tmp_json) + "_";
                        to_json(tmp_json, difficulty);
                        postfix_str += std::string(tmp_json);

                        int successfully = 0, failed = 0;
                        std::tie(successfully, failed) = test(dataset, annotations, faces, trained_ids);
                        int success_percent = ((float) successfully / (float) (successfully + failed)) * 100;

                        log(INFO, "Postfix:", postfix_str);
                        log(INFO, "Recognized successfully:", successfully);
                        log(INFO, "Recognized wrong:", failed);
                        log(INFO, "Success percent:", success_percent);
                        log(INFO, "----------------");
                        log_fs << "Postfix: " << postfix_str << std::endl;
                        log_fs << "Recognized successfully: " << successfully << std::endl;
                        log_fs << "Recognized wrong: " << failed << std::endl;
                        log_fs << "Success percent: " << success_percent << std::endl;
                        log_fs << "-----------------" << std::endl;
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}