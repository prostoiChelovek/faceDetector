//
// Created by prostoichelovek on 29.02.2020.
//

#ifndef FACES_DATASET_VOC_H
#define FACES_DATASET_VOC_H


#include "Dataset.h"

#include "../../modules/pugixml/src/pugixml.hpp"

#include "../../utils/utils.hpp"

namespace Faces {
    namespace Datasets {

        class Annotation_object_voc : public Annotation_object {
        public:
            cv::Rect rect;
            std::string label;

            Annotation_object_voc(cv::Rect rect, std::string label) : rect(rect), label(label) {}

            void draw(cv::Mat &img, const cv::Scalar &color) const override;
        };

        class Image_loader_voc : public Image_loader<> {
        public:
            cv::Mat load() override;
        };

        class Annotation_voc : public Annotation<Annotation_object_voc, Image_loader_voc, std::string> {
        public:
            using Annotation::Annotation;

            bool load() override;

            void draw(cv::Mat &img, const cv::Scalar &color = cv::Scalar(0, 255, 0)) const override;
        };

        class Dataset_voc : public Dataset<Annotation_voc> {
        public:
            using Dataset::Dataset;

            Annotation_voc get_annotation(int num) override;

            static Dataset<Annotation_voc> *create(const std::string &path);
        };

    }
}

#endif //FACES_DATASET_VOC_H
