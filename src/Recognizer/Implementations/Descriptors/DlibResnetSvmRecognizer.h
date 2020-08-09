/**
 * @file DlibResnetSvmRecognizer.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief This file contains a descriptor-based face recognizer with dlib`s ResNet and OvO SVM
 */

#ifndef FACES_DLIBRESNETSVMRECOGNIZER_H
#define FACES_DLIBRESNETSVMRECOGNIZER_H

#include <Recognizer/Descriptors/DescriptorsRecognizer.h>
#include <Config/Config.h>
#include "DlibResnetDescriptor.h"
#include "DlibSvmClassifier.h"

namespace faces {

    /**
     * An implementation of the descriptor-based face recognizer,
     * which uses dlib`s ResNet-based descriptor and a OvO SVM classifier
     */
    class DlibResnetSvmRecognizer : public DescriptorsRecognizer {
    public:
        FACES_MAIN_CONSTRUCTOR(explicit DlibResnetSvmRecognizer, Config const &config) {
            descriptor = FACES_CREATE_INSTANCE(Descriptor, DlibResnet, config);
            classifier = FACES_CREATE_INSTANCE(DescriptorsClassifier, DlibSvm, config);
            _checkOk();
        }

    };

    FACES_REGISTER_SUBCLASS(Recognizer, DlibResnetSvmRecognizer, DlibResnetSvm)

}

#endif //FACES_DLIBRESNETSVMRECOGNIZER_H
