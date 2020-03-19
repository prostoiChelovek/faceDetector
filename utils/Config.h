//
// Created by prostoichelovek on 16.02.2020.
//

#ifndef FACES_CONFIG_H
#define FACES_CONFIG_H


#include <opencv2/opencv.hpp>

#include "Serialization.hpp"

namespace Faces {

    class Config {
    public:
        // Paths ->
        std::string data_directory;

        std::string detection_netConfig, detection_netWeights;
        std::string landmarks_model;

        std::string recognition_descriptorsNet;
        std::string recognition_classifier;

        std::string checker_histogramChecker;

        std::string samples_labelsFile;
        std::string samples_directory;
        // <- Paths

        constexpr static auto properties = std::make_tuple( // For serialization
                Serialization::property(&Config::data_directory, "/paths/data_directory"),
                Serialization::property(&Config::detection_netConfig, "/paths/detection/net/config"),
                Serialization::property(&Config::detection_netWeights, "/paths/detection/net/weights"),
                Serialization::property(&Config::landmarks_model, "/paths/landmarks_model"),
                Serialization::property(&Config::recognition_descriptorsNet, "/paths/recognition/descriptors_net"),
                Serialization::property(&Config::recognition_classifier, "/paths/recognition/classifier"),
                Serialization::property(&Config::checker_histogramChecker, "/paths/checker/histogram_checker"),
                Serialization::property(&Config::samples_labelsFile, "/paths/samples/labels"),
                Serialization::property(&Config::samples_directory, "/paths/samples/directory")
        );

        explicit Config(const std::string &config_file);

        nlohmann::json serialize();

        void deserialize(const nlohmann::json &data);

        bool save();

        bool load();

    private:
        std::fstream config_fs;
        std::string config_file;

        // prefix paths with data_directory
        void prefix_paths();

    };

}

#endif //FACES_CONFIG_H
