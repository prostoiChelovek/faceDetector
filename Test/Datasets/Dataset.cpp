//
// Created by prostoichelovek on 19.02.2020.
//
/*
#include "Dataset.h"
#include "Dataset_SoF.h"


void Faces::Annotation_object::draw(cv::Mat &img, const cv::Scalar &color) const {
    cv::rectangle(img, rect, color, 2, 4);
    cv::rectangle(img, cv::Rect(rect.x, rect.y - 20, rect.width, 25),
                  color, cv::FILLED, 4);
    cv::putText(img, label, rect.tl(),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
}

std::ostream &Faces::operator<<(std::ostream &os, const Annotation_object &a) {
    os << "Label: " << a.label << "; Rect: " << a.rect << std::endl;
    return os;
}

bool Faces::Annotation::load() {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(annotation_path.c_str());
    if (!result) {
        log(ERROR, "Cannot load annotation from", annotation_path, ":", result.description());
        return false;
    }

    try {
        pugi::xml_node root = doc.first_child();
        image_path = root.child_value("path");

        for (pugi::xml_node &object : root.children("object")) {
            std::string label = object.child("name").first_child().value();

            pugi::xml_node bndbox = object.child("bndbox");
            int x_min = std::stoi(bndbox.child("xmin").first_child().value());
            int y_min = std::stoi(bndbox.child("ymin").first_child().value());
            int x_max = std::stoi(bndbox.child("xmax").first_child().value());
            int y_max = std::stoi(bndbox.child("ymax").first_child().value());

            cv::Rect rect(cv::Point(x_min, y_min), cv::Point(x_max, y_max));
            objects.emplace_back(rect, label);
        }
    } catch (std::exception &e) {
        log(ERROR, "Cannot load annotation from", annotation_path, ":", e.what());
        return false;
    }

    ok = true;
    return true;
}

void Faces::Annotation::draw(cv::Mat &img) {
    for (const Annotation_object &obj : objects) {
        obj.draw(img);
    }
}

Faces::Annotation::operator bool() const {
    return ok;
}


std::ostream &Faces::operator<<(std::ostream &os, const Faces::Annotation &a) {
    os << "Annotation: {" << std::endl;
    if (a) {
        os << "Path: " << a.annotation_path << std::endl;
        os << "Image path: " << a.image_path << std::endl;
        os << "Objects: [" << std::endl;
        for (const Annotation_object &object : a.objects) {
            os << object;
        }
        os << "]";
    } else {
        os << "Annotation is invalid" << std::endl;
    }
    os << "}";
    return os;
}


Faces::Annotation Faces::Dataset::get_annotation(int num) {
    Annotation a(annotations_directory + "/" + std::to_string(num) + ".xml");
    if (!a.load()) {
        log(ERROR, "Cannot load annotation with num", num);
    }
    return a;
}

bool Faces::Dataset::get_sample(int num, Annotation &annotation, cv::Mat &img) {
    annotation = get_annotation(num);
    if (!annotation) {
        return false;
    }

    try {
        img = cv::imread(annotation.image_path);
    } catch (std::exception &e) {
        log(ERROR, "Cannot load image from", annotation.image_path, ":", e.what());
        return false;
    }

    return true;
}

bool Faces::Dataset::get_next(Annotation &annotation, cv::Mat &img) {
    if (!get_sample(current_file, annotation, img)) {
        return false;
    }
    current_file++;
    return true;
}
*/