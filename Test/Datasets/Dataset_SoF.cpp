//
// Created by prostoichelovek on 28.02.2020.
//
/*
#include "Dataset_SoF.h"

namespace Faces {

    Annotation_object_SoF::Annotation_object_SoF(const nlohmann::json &obj) {
        Serialization::fromJson<Annotation_object_SoF>(this, obj);
    }

    void Annotation_object_SoF::draw(cv::Mat &img, const cv::Scalar &color) const {
        Annotation_object::draw(img, color);

        cv::rectangle(img, glasses_roi, color, 1, 4);
        for (const cv::Point &lm : landmarks) {
            cv::circle(img, lm, 2, color, cv::FILLED);
        }
    }

    bool Annotation_SoF::load(const nlohmann::json &obj) {
        try {
            Annotation_object_SoF o(obj);
            objects.emplace_back(o);
            get_image_path();
            return true;
        } catch (std::exception &e) {
            log(ERROR, "Cannot load annotation from", annotation_path, ":", e.what());
            return false;
        }

    }

    void Annotation_SoF::get_image_path() {
        if(objects.empty()) {
            return;
        }
        Annotation_object_SoF &obj = objects.back();
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

        obj.filename = filename.str();
    }

    Dataset_SoF::Dataset_SoF(const std::string &metadata_path) : Dataset(metadata_path) {
        std::ifstream ifs(metadata_path);
        ifs >> metadata_j;
    }


    Dataset_SoF::Annotations_t Dataset_SoF::get_annotation(int num) {
        Annotations_t res;

        Annotation_SoF a;
        try {
            a.load(metadata_j[num]);
        } catch (std::exception &e) {
            log(ERROR, "Cannot load annotation with num", num);
        }
        return res;
    }

    bool Dataset_SoF::get_sample(int num, Annotations_t &annotations, std::vector<cv::Mat> &imgs) {
        annotations = get_annotation(num);
        if (annotations.empty()) {
            return false;
        }

        try {
            //img = cv::imread(annotation.image_path);
        } catch (std::exception &e) {
            log(ERROR, "Cannot load image from", annotation.image_path, ":", e.what());
            return false;
        }

        return true;
    }

    bool Dataset_SoF::get_next(Annotations_t &annotations, std::vector<cv::Mat> &imgs) {
        if (!get_sample(current_file, annotations, imgs)) {
            return false;
        }
        current_file++;
        return true;
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
*/