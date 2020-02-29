//
// Created by prostoichelovek on 29.02.2020.
//

#ifndef FACES_DATASET_SOF_H
#define FACES_DATASET_SOF_H


#include <tuple>

#include <nlohmann/json.hpp>

#include "Dataset.h"

#include "../../utils/utils.hpp"
#include "../../utils/Serialization.hpp"

namespace Faces {
    namespace Datasets {

        enum Occlusion_SoF {
            EYE, EYE_AND_NOSE, EYE_AND_MOUTH
        };

        enum Image_filters_SoF {
            NORMAL, NOISE, SMOOTH, POSTERIZE
        };

        enum Difficulty_SoF {
            ORIGINAL, EASY, MEDIUM, HARD
        };

        class Annotation_object_SoF : public Annotation_object {
        public:
            std::string id, seq, part, filename;
            int year, age;
            bool headscarf, illumination;
            std::vector<cv::Point> landmarks;
            std::vector<bool> estimated_landmarks;
            cv::Rect rect, glasses_roi;
            enum Cropped {
                YES, NOT
            } cropped;
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
                    Serialization::property(&Annotation_object_SoF::id, "/id"),
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

            void draw(cv::Mat &img, const cv::Scalar &color) const override;
        };

        class Image_loader_SoF : public Image_loader<Occlusion_SoF, Image_filters_SoF, Difficulty_SoF> {
        public:
            cv::Mat load(Occlusion_SoF occlusion, Image_filters_SoF filters, Difficulty_SoF difficulty) override;
        };

        class Annotation_SoF : public Annotation<Annotation_object_SoF, Image_loader_SoF, nlohmann::json> {
        public:
            explicit Annotation_SoF(const nlohmann::json &source, const std::string &images_directory);

            bool load() override;

            void draw(cv::Mat &img, const cv::Scalar &color = cv::Scalar(0, 255, 0)) const override;

        private:
            std::string images_directory;

            std::string get_image_path();
        };

        class Dataset_SoF : public Dataset<Annotation_SoF> {
        public:
            explicit Dataset_SoF(const std::string &metadata_path, const std::string &images_directory);

            Annotation_SoF get_annotation(int num) override;

            static Dataset<Annotation_SoF> *create(const std::string &path, const std::string &images_directory);

        private:
            std::string images_directory;
            nlohmann::json metadata;
        };

        // Serialization stuff ->
        NLOHMANN_JSON_SERIALIZE_ENUM(Occlusion_SoF, {
            { Occlusion_SoF::EYE, "e0" },
            { Occlusion_SoF::EYE_AND_NOSE, "en" },
            { Occlusion_SoF::EYE_AND_MOUTH, "em" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Image_filters_SoF, {
            { Image_filters_SoF::NORMAL, "nl" },
            { Image_filters_SoF::NOISE, "Gn" },
            { Image_filters_SoF::SMOOTH, "Gs" },
            { Image_filters_SoF::POSTERIZE, "Ps" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Difficulty_SoF, {
            { Difficulty_SoF::ORIGINAL, "o" },
            { Difficulty_SoF::EASY, "e" },
            { Difficulty_SoF::MEDIUM, "m" },
            { Difficulty_SoF::HARD, "h" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Cropped, {
            { Annotation_object_SoF::Cropped::YES, "cr" },
            { Annotation_object_SoF::Cropped::NOT, "nc" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Gender, {
            { Annotation_object_SoF::Gender::MALE, "m" },
            { Annotation_object_SoF::Gender::FEMALE, "f" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Lightning, {
            { Annotation_object_SoF::Lightning::INDOOR, "i" },
            { Annotation_object_SoF::Lightning::OUTDOOR, "o" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::View, {
            { Annotation_object_SoF::View::FRONT, "fr" },
            { Annotation_object_SoF::View::NEAR_FRONT, "nf" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Emotion, {
            { Annotation_object_SoF::Emotion::NORMAL, "no" },
            { Annotation_object_SoF::Emotion::HAPPY, "hp" },
            { Annotation_object_SoF::Emotion::SAD, "sd" },
            { Annotation_object_SoF::Emotion::SURPRISED, "sr" }
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(Annotation_object_SoF::Glasses, {
            { Annotation_object_SoF::Glasses::EYEGLASSES, 1 },
            { Annotation_object_SoF::Glasses::SEMI_TRANSPARENT_SUNGLASSES, 2 },
            { Annotation_object_SoF::Glasses::OPAQUE_SUNGLASSES, 3 },
            { Annotation_object_SoF::Glasses::OTHER, 4 }
        })
        // <- Serialization stuff
    }
}


#endif //FACES_DATASET_SOF_H
