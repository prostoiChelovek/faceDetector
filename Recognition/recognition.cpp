//
// Created by prostoichelovek on 25.07.19.
//

#include "recognition.h"

namespace Faces {
    namespace Recognition {

        recognition::recognition(Callbacks *callbacks, cv::Size faceSize,
                                 std::string classifierFile, std::string descriptorFile)
                : callbacks(callbacks), faceSize(faceSize), descriptor(descriptorFile, classifierFile) {
        }

        recognition::~recognition() {
            labelsFs.close();
        }

        bool recognition::readLabels(std::string file) {
            if (!getFileContent(file, labels))
                return false;
            labelsFs = std::ofstream(file, std::ios::app);
            if (!labelsFs) {
                log(ERROR, "Could not open labels file for write", file);
                return false;
            }
            return true;
        }

        void recognition::recognize(Face &face) {
            face.minLabelNotChanged = minLabelNotChanged;

            getDescriptors(face);

            if (!descriptor.classifier.ok)
                return;

            float label = descriptor.classifier.predict(face.descriptor);
            face.setLabel(label);

            if (callbacks != nullptr) {
                if (face.getLabel() > -1)
                    callbacks->call("faceRecognized", &face);
                if (face.getLabel() == -1)
                    callbacks->call("unknownFace", &face);
                if (face.getLabel() != -2 && face.last != nullptr) {
                    if (face.last->getLabel() == -2) {
                        callbacks->call("labelConfirmed", &face);
                    }
                }
            }
        }

        void recognition::recognize(std::vector<Face> &faces) {
            for (Face &f : faces) {
                recognize(f);
            }
        }

        void recognition::train(std::string samplesDir, int persons_limit, int samples_limit) {
            std::map<std::string, int> files = getSamples(samplesDir);

            cv::Mat samples;
            std::vector<int> labels;

            // Read samples ->
            int num_files = 0;
            for (auto &file : files) {
                if (num_files > persons_limit && persons_limit > 0) {
                    break;
                }

                cv::FileStorage fs(samplesDir + "/" + file.first, cv::FileStorage::READ);
                if (!fs.isOpened()) {
                    log(ERROR, "Cannot open samples file", file.first, ", skipping it");
                    continue;
                }
                cv::Mat descriptors;
                fs["descriptors"] >> descriptors;
                fs.release();

                if (samples.empty()) {
                    samples = descriptors;
                } else {
                    cv::vconcat(samples, descriptors, samples);
                }

                for (int i = 0; i < samples.rows; i++) {
                    labels.emplace_back(file.second);
                }

                num_files++;
            }
            // <- Read samples

            descriptor.classifier.train(samples, labels);
        }

        void recognition::getDescriptors(Face &face) {
            cv::Mat img_copy = face.img;
            descriptor.preprocess_input_mat(img_copy);
            tensorflow::Tensor input_tensor = descriptor.create_input_tensor(img_copy);
            tensorflow::Tensor phase_tensor = descriptor.create_phase_tensor();
            cv::Mat output = descriptor.run(input_tensor, phase_tensor);
            face.descriptor = output;
        }

        void recognition::load(std::string classifierFile, std::string descriptorFile) {
            descriptor.classifier.load(classifierFile);
            if (!descriptor.classifier.ok) {
                log(ERROR, "Cannot read classifiers from", classifierFile);
            }
        }

        void recognition::addLabel(std::string &label) {
            labels.emplace_back(label);
            currentLabel = labels.size() - 1;
            imgNum[currentLabel] = 0;
            labelsFs << label << "\n";
            labelsFs.flush();
        }

        bool recognition::save(std::string file) {
            descriptor.classifier.save(file);
        }

        std::map<std::string, int> recognition::getSamples(std::string dir) {
            std::vector<std::string> names = list_directory(dir, "yml");

            std::map<std::string, int> samples;

            for (int i = 0; i < labels.size(); i++) {
                for (std::string &name : names) {
                    if (name.find(std::to_string(i) + ".") != std::string::npos) {
                        samples[name] = i;
                    }
                }
            }

            return samples;
        }

        std::string recognition::addSample(std::string storage, Face &face) {
            createDirNotExists(storage);
            std::string path = storage + "/" + std::to_string(currentLabel) + ".yml";

            cv::FileStorage fs(path, cv::FileStorage::READ);
            cv::Mat samples;
            if (fs.isOpened()) {
                fs["descriptors"] >> samples;
            }
            fs.open(path, cv::FileStorage::WRITE);
            log(INFO, face.descriptor);
            if (!samples.empty()) {
                cv::vconcat(samples, face.descriptor, samples);
            } else {
                samples = face.descriptor;
            }
            fs << "descriptors" << samples;

            imgNum[currentLabel]++;
            return path;
        }
    }
}