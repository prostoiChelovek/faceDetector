//
// Created by prostoichelovek on 25.07.19.
//

#include "Recognizer.h"

namespace Faces {

    Recognizer::Recognizer(Callbacks *callbacks, cv::Size faceSize)
            : callbacks(callbacks), faceSize(faceSize) {}

    Recognizer::~Recognizer() {
        labelsFs.close();
    }

    bool Recognizer::readLabels(std::string file) {
        if (!getFileContent(file, labels))
            return false;
        labelsFs = std::ofstream(file, std::ios::app);
        if (!labelsFs) {
            log(ERROR, "Could not open labels file for write", file);
            return false;
        }
        return true;
    }

    void Recognizer::operator()(Face &face) {
        recognize(face);

        if (callbacks != nullptr) {
            if (face.getLabel() > -1)
                callbacks->call("faceRecognized", &face);
            if (face.getLabel() == -1)
                callbacks->call("unknownFace", &face);
            if (face.getLabel() != -2 && face.last->getLabel() == -2)
                callbacks->call("labelConfirmed", &face);
        }
    }

    void Recognizer::operator()(std::vector<Face> &faces) {
        for (Face &f : faces) {
            operator()(f);
        }
    }

    void Recognizer::addLabel(std::string &label) {
        labels.emplace_back(label);
        currentLabel = labels.size() - 1;
        imgNum[currentLabel] = 0;
        labelsFs << label << "\n";
        labelsFs.flush();
    }

}