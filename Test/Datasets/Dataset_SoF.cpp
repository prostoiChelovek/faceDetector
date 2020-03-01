//
// Created by prostoichelovek on 29.02.2020.
//

#include "Dataset_SoF.h"

namespace Faces {
    namespace Datasets {

        void Annotation_object_SoF::draw(cv::Mat &img, const cv::Scalar &color) const {
            cv::rectangle(img, rect, color, 2, 4);
            cv::rectangle(img, cv::Rect(rect.x, rect.y - 20, rect.width, 25),
                          color, cv::FILLED, 4);
            cv::putText(img, id, rect.tl(),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            cv::rectangle(img, glasses_roi, color, 1, 4);

            for (const cv::Point &lm : landmarks) {
                cv::circle(img, lm, 2, color, cv::FILLED);
            }
        }

        cv::Mat Image_loader_SoF::load(Occlusion_SoF occlusion, Image_filters_SoF filters, Difficulty_SoF difficulty) {
            std::string img_path = path + "_";
            nlohmann::json tmp_json;
            to_json(tmp_json, occlusion);
            img_path += std::string(tmp_json);
            img_path += "_";
            to_json(tmp_json, filters);
            img_path += std::string(tmp_json);
            img_path += "_";
            to_json(tmp_json, difficulty);
            img_path += std::string(tmp_json);
            img_path += ".jpg";
            return cv::imread(img_path);
        }

        bool Annotation_SoF::load() {
            try {
                annotations.emplace_back(Serialization::fromJson<Annotation_object_SoF>(source));
                images_loader.path = images_directory + "/" + get_image_path();
                ok = true;
                return true;
            } catch (std::exception &e) {
//                log(ERROR, "Cannot load annotation: ", e.what());
                return false;
            }
        }

        void Annotation_SoF::draw(cv::Mat &img, const cv::Scalar &color) const {
            for (const Annotation_object_SoF &obj : annotations) {
                obj.draw(img, color);
            }
        }

        std::string Annotation_SoF::get_image_path() {
            if (annotations.empty()) {
                return "";
            }
            Annotation_object_SoF &obj = annotations.back();
            obj.filename.pop_back();
            std::stringstream filename;
            filename << obj.filename;

            nlohmann::json tmp_json; // i am too lazy to do conversion somehow else and more efficiently
            to_json(tmp_json, obj.lightning);
            filename << std::string(tmp_json) << "_";
            to_json(tmp_json, obj.view);
            filename << std::string(tmp_json) << "_";
            to_json(tmp_json, obj.cropped);
            filename << std::string(tmp_json) << "_";
            to_json(tmp_json, obj.emotion);
            filename << std::string(tmp_json) << "_";
            filename << obj.year << "_" << obj.part;

            return filename.str();
        }

        Annotation_SoF::Annotation_SoF(const nlohmann::json &source, const std::string &images_directory)
                : Annotation(source), images_directory(images_directory) {
        }

        Dataset_SoF::Dataset_SoF(const std::string &metadata_path, const std::string &images_directory)
                : Dataset(metadata_path), images_directory(images_directory) {
            std::ifstream ifs(metadata_path);
            ifs >> metadata;
            ifs.close();
        }

        Annotation_SoF Dataset_SoF::get_annotation(int num) {
            Annotation_SoF res(metadata[num], images_directory);
            res.load();
            return res;
        }

        Dataset<Annotation_SoF> *Dataset_SoF::create(const std::string &path, const std::string &images_directory) {
            return new Dataset_SoF(path, images_directory);
        }

    }
}

namespace cv {
    void to_json(nlohmann::json &j, const Point &p) {
        j = nlohmann::json{{"x", p.x},
                           {"y", p.y}};
    }

    void from_json(const nlohmann::json &j, Point &p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }

    void to_json(nlohmann::json &j, const Rect &p) {
        j = nlohmann::json{{"x",      p.x},
                           {"y",      p.y},
                           {"width",  p.width},
                           {"height", p.height}};
    }

    void from_json(const nlohmann::json &j, Rect &p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("width").get_to(p.width);
        j.at("height").get_to(p.height);
    }
}

namespace std {
    void to_json(nlohmann::json &j, const vector<cv::Point> &p) {
        for (const cv::Point &pt : p) {
            j.emplace_back(pt);
        }
    }

    void from_json(const nlohmann::json &j, vector<cv::Point> &p) {
        for (const cv::Point &pt : j) {
            p.emplace_back(pt);
        }
    }
}