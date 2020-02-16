//
// Created by prostoichelovek on 25.07.19.
//

#include "recognition.h"

namespace Faces {
    namespace Recognition {

        recognition::recognition(Callbacks *callbacks, cv::Size faceSize,
                                 std::string classifierFile, std::string descriptorFile)
                : callbacks(callbacks), faceSize(faceSize) {
            load(classifierFile, descriptorFile);
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
            if (!ok)
                return;

            face.minLabelNotChanged = minLabelNotChanged;

            getDescriptors(face);

            std::map<double, int> votes;
            for (auto &faceClassifier : classifiers) {
                double prediction = faceClassifier(face.descriptor, threshold);
                if (prediction == -1)
                    continue;
                votes[prediction]++;
            }

            auto max = std::max_element(votes.begin(), votes.end(),
                                        [](const std::pair<int, int> &p1, const std::pair<int, int> &p2) {
                                            return p1.second < p2.second;
                                        });

            int label = votes.empty() ? -1 : max->first;
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

        void recognition::train(std::string samplesDir) {
            std::map<std::string, int> files = getSamples(samplesDir);

            std::vector<descriptor_type> descriptors;
            std::vector<int> labels;

            // Read samples ->
            for (auto &file : files) {
                std::ifstream descFS(samplesDir + "/" + file.first);

                std::vector<std::string> lines;
                std::string str;
                while (getline(descFS, str)) {
                    if (!str.empty())
                        lines.push_back(str);
                }

                std::vector<std::vector<double>> descs;
                for (std::string &line : lines) {
                    descs.emplace_back(std::vector<double>{});
                    std::vector<std::string> nums = split(line, " ");
                    for (std::string &num : nums) {
                        try {
                            descs[descs.size() - 1].emplace_back(stod(num));
                        } catch (std::exception &e) {
                            log(ERROR, "Cannot read face descriptor from", file.first, ":", e.what());
                            continue;
                        }
                    }
                }

                for (std::vector<double> &desc : descs) {
                    labels.emplace_back(file.second);
                    descriptors.emplace_back(dlib::mat(desc));
                }
            }

            // <- Read samples

            // Unique labels ->
            std::vector<int> total_labels;
            for (int &label : labels) {
                if (find(total_labels.begin(), total_labels.end(), label) == total_labels.end())
                    total_labels.emplace_back(label);
            }
            // <- Unique labels

            // Init trainers ->

            std::vector<trainer_type> trainers;
            unsigned long num_trainers = total_labels.size() * (total_labels.size() - 1) / 2;

            for (int i = 0; i < num_trainers; i++) {
                trainers.emplace_back(trainer_type());
                trainers[i].set_kernel(kernel_type());
                trainers[i].set_c(10);
            }

            // <- Init trainers

            // Train and init classifiers ->

            int label1 = 0, label2 = 1;
            for (trainer_type &trainer : trainers) {
                std::vector<descriptor_type> samples4pair;
                std::vector<double> labels4pair;

                for (int i = 0; i < descriptors.size(); i++) {
                    if (labels[i] == total_labels[label1]) {
                        samples4pair.emplace_back(descriptors[i]);
                        labels4pair.emplace_back(-1);
                    }
                    if (labels[i] == total_labels[label2]) {
                        samples4pair.emplace_back(descriptors[i]);
                        labels4pair.emplace_back(+1);
                    }
                }

                classifiers.emplace_back(total_labels[label1],
                                         total_labels[label2],
                                         trainer.train(samples4pair, labels4pair));

                label2++;
                if (label2 == total_labels.size()) {
                    label1++;
                    label2 = label1 + 1;
                }
            }

            // <- Train and init classifiers
        }

        void recognition::getDescriptors(Face &face) {
            dlib::cv_image<dlib::rgb_pixel> faceImg;
            cv::Mat rgbImg;
            cv::cvtColor(face.img, rgbImg, cv::COLOR_BGR2RGB);
            faceImg = dlib::cv_image<dlib::rgb_pixel>(rgbImg);

            std::vector<dlib::matrix<float, 0, 1>> face_descriptorsF =
                    descriptor(std::vector<dlib::cv_image<dlib::rgb_pixel>>{faceImg});
            std::vector<descriptor_type> face_descriptors;
            for (auto &desc : face_descriptorsF) {
                std::vector<double> descVec;
                for (unsigned int r = 0; r < desc.nr(); r += 1) {
                    descVec.emplace_back(desc(r, 0));
                }
                face_descriptors.emplace_back(dlib::mat(descVec));
            }

            face.descriptor = face_descriptors[0];
        }

        void recognition::load(std::string classifierFile, std::string descriptorFile) {
            try {
                dlib::deserialize(classifierFile) >> classifiers;
                dlib::deserialize(descriptorFile) >> descriptor;
                ok = true;
            } catch (dlib::serialization_error &e) {
                log(ERROR, "Cannot read load classifier or descriptor from", classifierFile,
                    "and", descriptorFile, ":\n", e.what());
            }
            if (classifiers.empty()) {
                ok = false;
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
            try {
                dlib::serialize(file) << classifiers;
                return true;
            } catch (dlib::serialization_error &e) {
                log(ERROR, "Cannot save face descriptor classifiers to", file, ":", e.what());
                return false;
            }
        }

        std::map<std::string, int> recognition::getSamples(std::string dir) {
            std::vector<std::string> names = list_directory(dir, "csv");

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
            std::string path = storage + "/" + std::to_string(currentLabel) + ".csv";

            std::ofstream descFs(path, std::ios::app);
            std::stringstream descSS;
            for (double d : face.descriptor) {
                descSS << d << " ";
            }
            std::string descStr = descSS.str();
            descStr.pop_back();

            descFs << descStr << std::endl;
            descFs.flush();

            imgNum[currentLabel]++;
            return path;
        }
    }
}