//
// Created by prostoichelovek on 28.02.2020.
//

#ifndef FACES_DATASET_SOF_H
#define FACES_DATASET_SOF_H


#include <nlohmann/json.hpp>

#include "Dataset.h"

#include "../../utils/Serialization.hpp"


namespace Faces {

    class Annotation_object_SoF : public Annotation_object {
    public:
        std::string seq, part, filename;
        int year, age;
        bool cropped, headscarf, illumination;
        std::vector<cv::Point> landmarks;
        std::vector<bool> estimated_landmarks;
        cv::Rect glasses_roi;
        enum Gender {
            MALE, FEMALE
        } gender;
        enum Lightning {
            INDOOR, OUTDOOR
        } lightning;
        enum View {
            FRONT, NEAR_FRONT
        } view;
        enum Emotion {
            NORMAL, HAPPY, SAD, SURPRISED
        } emotion;
        enum Glasses {
            EYEGLASSES, SEMI_TRANSPARENT_SUNGLASSES, OPAQUE_SUNGLASSES, OTHER
        } glasses;

        constexpr static auto properties = std::make_tuple( // For serialization
                Serialization::property(&Annotation_object_SoF::label, "/id"),
                Serialization::property(&Annotation_object_SoF::seq, "/sequence"),
                Serialization::property(&Annotation_object_SoF::gender, "/gender"),
                Serialization::property(&Annotation_object_SoF::age, "/age"),
                Serialization::property(&Annotation_object_SoF::lightning, "/lightning"),
                Serialization::property(&Annotation_object_SoF::view, "/view"),
                Serialization::property(&Annotation_object_SoF::cropped, "/cropped"),
                Serialization::property(&Annotation_object_SoF::emotion, "/emotion"),
                Serialization::property(&Annotation_object_SoF::year, "/year"),
                Serialization::property(&Annotation_object_SoF::part, "/part"),
                Serialization::property(&Annotation_object_SoF::glasses, "/glasses"),
                Serialization::property(&Annotation_object_SoF::headscarf, "/headscarf"),
                Serialization::property(&Annotation_object_SoF::illumination, "/illumination"),
                Serialization::property(&Annotation_object_SoF::filename, "/filename"),
                Serialization::property(&Annotation_object_SoF::landmarks, "/landmarks"),
                Serialization::property(&Annotation_object_SoF::estimated_landmarks, "/estimated_landmarks"),
                Serialization::property(&Annotation_object_SoF::rect, "/face_ROI"),
                Serialization::property(&Annotation_object_SoF::glasses_roi, "/glasses_ROI")
        );

        explicit Annotation_object_SoF(const nlohmann::json &obj);

        void draw(cv::Mat &img, const cv::Scalar &color = cv::Scalar(0, 255, 0)) const;

    };

    class Annotation_SoF : public Annotation {
    public:
        std::vector<Annotation_object_SoF> objects;

        bool load() = delete;

        bool load(const nlohmann::json &obj);

    };

    class Dataset_SoF : public Dataset {
    public:
        explicit Dataset_SoF(const std::string &metadata_path);

        Annotation_SoF get_annotation(int num);

        bool get_sample(int num, Annotation_SoF &annotation, cv::Mat &img);

        bool get_next(Annotation_SoF &annotation, cv::Mat &img);


    private:
        nlohmann::json metadata_j;
    };

    // Serialization stuff ->
    NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Gender, {
        { Annotation_object_SoF::Gender::MALE, "m" },
        { Annotation_object_SoF::Gender::FEMALE, "f" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Lightning, {
        { Annotation_object_SoF::Lightning::INDOOR, "i" },
        { Annotation_object_SoF::Lightning::OUTDOOR, "o" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::View, {
        { Annotation_object_SoF::View::FRONT, "f" },
        { Annotation_object_SoF::View::NEAR_FRONT, "n" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Emotion, {
        { Annotation_object_SoF::Emotion::NORMAL, 1 },
        { Annotation_object_SoF::Emotion::HAPPY, 2 },
        { Annotation_object_SoF::Emotion::SAD, 3 },
        { Annotation_object_SoF::Emotion::SURPRISED, 4 }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Glasses, {
        { Annotation_object_SoF::Glasses::EYEGLASSES, 1 },
        { Annotation_object_SoF::Glasses::SEMI_TRANSPARENT_SUNGLASSES, 2 },
        { Annotation_object_SoF::Glasses::OPAQUE_SUNGLASSES, 3 },
        { Annotation_object_SoF::Glasses::OTHER, 4 }
    })

}


namespace cv {
    void to_json(nlohmann::json &j, const Point &p);

    void from_json(const nlohmann::json &j, Point &p);

    void to_json(nlohmann::json &j, const Rect &p);

    void from_json(const nlohmann::json &j, Rect &p);
}

namespace std {
    void to_json(nlohmann::json &j, const vector<cv::Point> &p);

    void from_json(const nlohmann::json &j, vector<cv::Point> &p);
}

// <- Serialization stuff

#endif //FACES_DATASET_SOF_H
