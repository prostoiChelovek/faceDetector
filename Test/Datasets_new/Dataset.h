//
// Created by prostoichelovek on 29.02.2020.
//

#ifndef FACES_DATASET_H
#define FACES_DATASET_H


#include <string>

#include <opencv2/opencv.hpp>

namespace Faces {
    namespace Datasets {

        class Annotation_object {
        public:
            virtual void draw(cv::Mat &img, const cv::Scalar &color = cv::Scalar(0, 255, 0)) const = 0;
        };

        template<typename ID_t = int, ID_t Default_id = 0>
        class Image_loader {
        public:
            std::string path;

            Image_loader() = default;

            explicit Image_loader(const std::string &path) : path(path) {}

            virtual cv::Mat load(ID_t id = Default_id) = 0;
        };

        template<typename Annotation_object_t = Annotation_object, typename Image_loader_t = Image_loader<>, typename Source_t = std::string>
        class Annotation {
        public:
            std::vector<Annotation_object_t> annotations;
            Image_loader_t images_loader;

            Source_t source;

            explicit Annotation(const Source_t &source) : source(source) {}

            virtual bool load() = 0;

            virtual void draw(cv::Mat &img, const cv::Scalar &color = cv::Scalar(0, 255, 0)) const = 0;
        };

        template<typename Annotation_t = Annotation<>>
        class Dataset {
        public:
            int current_num = 1;

            std::string path;

            explicit Dataset(const std::string &path) : path(path) {}

            virtual Annotation_t get_annotation(int num) = 0;
        };

    }
}


#endif //FACES_DATASET_H
