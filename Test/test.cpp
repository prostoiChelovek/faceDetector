//
// Created by prostoichelovek on 19.02.2020.
//

#include "Dataset.h"

int main() {
    Faces::Dataset dataset("/home/prostoichelovek/Документы/datasets/faces/dataset_from_dataturks/annotations");
    Faces::Annotation annotation;
    cv::Mat img;
    while (dataset.get_next(annotation, img)) {
        log(INFO, annotation);
        annotation.draw(img);
        cv::imshow("img", img);
        cv::waitKey(0);
    }


    return EXIT_SUCCESS;
}