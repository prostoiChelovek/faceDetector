//
// Created by prostoichelovek on 29.02.2020.
//

#include "Dataset_voc.h"


namespace Faces {
    namespace Datasets {

        void Annotation_object_voc::draw(cv::Mat &img, const cv::Scalar &color) const {
            cv::rectangle(img, rect, color, 2, 4);
            cv::rectangle(img, cv::Rect(rect.x, rect.y - 20, rect.width, 25),
                          color, cv::FILLED, 4);
            cv::putText(img, label, rect.tl(),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        }

        cv::Mat Image_loader_voc::load() {
            return cv::imread(path);
        }

        bool Annotation_voc::load() {
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(source.c_str());
            if (!result) {
                log(ERROR, "Cannot load annotation from", source, ":", result.description());
                return false;
            }

            try {
                pugi::xml_node root = doc.first_child();
                images_loader.path = root.child_value("path");

                for (pugi::xml_node &object : root.children("object")) {
                    std::string label = object.child("name").first_child().value();

                    pugi::xml_node bndbox = object.child("bndbox");
                    int x_min = std::stoi(bndbox.child("xmin").first_child().value());
                    int y_min = std::stoi(bndbox.child("ymin").first_child().value());
                    int x_max = std::stoi(bndbox.child("xmax").first_child().value());
                    int y_max = std::stoi(bndbox.child("ymax").first_child().value());

                    cv::Rect rect(cv::Point(x_min, y_min), cv::Point(x_max, y_max));
                    annotations.emplace_back(rect, label);
                }
            } catch (std::exception &e) {
                log(ERROR, "Cannot load annotation from", source, ":", e.what());
                return false;
            }

            ok = true;
            return true;
        }

        void Annotation_voc::draw(cv::Mat &img, const cv::Scalar &color) const {
            for (const Annotation_object &obj : annotations) {
                obj.draw(img, color);
            }
        }

        Annotation_voc Dataset_voc::get_annotation(int num) {
            Annotation_voc a(path + "/" + std::to_string(num) + ".xml");
            if (!a.load()) {
                log(ERROR, "Cannot load annotation with num", num);
            }
            return a;
        }

        Dataset<Annotation_voc> *Dataset_voc::create(const std::string &path) {
            return new Dataset_voc(path);
        }
    }
}