//
// Created by prostoichelovek on 19.02.2020.
//

#ifndef FACES_DATASET_H
#define FACES_DATASET_H


#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "pugixml.hpp"

#include "../utils/utils.hpp"

namespace Faces {

    struct Annotation_object {
        cv::Rect rect;
        std::string label;

        Annotation_object(cv::Rect rect, std::string label) : rect(rect), label(label) {}

        friend std::ostream &operator<<(std::ostream &os, const Annotation_object &a);
    };

    class Annotation {
    public:
        std::string annotation_path;

        std::vector<Annotation_object> objects;
        std::string image_path;

        bool ok = false;

        explicit Annotation(const std::string &annotation_path) : annotation_path(annotation_path) {}

        Annotation() = default;

        bool load();

        void draw(cv::Mat &img);

        explicit operator bool() const;

        friend std::ostream &operator<<(std::ostream &os, const Annotation &a);

    };

    class Dataset {
    public:
        std::string annotations_directory = "annotations";

        int current_file = 1;

        explicit Dataset(const std::string &annotations_directory) : annotations_directory(annotations_directory) {}

        Annotation get_annotation(int num);

        bool get_sample(int num, Annotation &annotation, cv::Mat &img);

        bool get_next(Annotation &annotation, cv::Mat &img);

    };

}


#endif //FACES_DATASET_H
