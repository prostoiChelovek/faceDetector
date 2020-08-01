/**
 * @file DlibSvmClassifier.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DLIBSVMCLASSIFIER_H
#define FACES_DLIBSVMCLASSIFIER_H

#include "DlibResnetDescriptor.h"
#include <Recognizer/Descriptors/DescriptorsClassifier.hpp>

namespace faces {

    using KernelType = dlib::histogram_intersection_kernel<DescriptorType>;
    using TrainerType =  dlib::svm_c_trainer<KernelType>;

    class SingleSvmClassifier {
    public:
        int negativeLabel;
        int positiveLabel;

        dlib::decision_function<KernelType> classifier;

        SingleSvmClassifier() = default;

        SingleSvmClassifier(int negativeLabel, int positiveLabel,
                            dlib::decision_function<KernelType> const &classifier)
                : negativeLabel(negativeLabel), positiveLabel(positiveLabel),
                  classifier(classifier) {}

        double classify(DescriptorType const &descriptor, double threshold = 0.3) {
            double prediction = classifier(descriptor);
            if (fabs(prediction) < threshold)
                return -1;
            if (prediction < 0)
                return negativeLabel;
            return positiveLabel;
        }

    };

    inline void serialize(const SingleSvmClassifier &item, std::ostream &out) {
        try {
            dlib::serialize(item.negativeLabel, out);
            dlib::serialize(item.positiveLabel, out);
            dlib::serialize(item.classifier, out);
        } catch (dlib::serialization_error &e) {
            throw dlib::serialization_error(e.info + "\n while serializing an object of type MyClassifier");
        }
    }

    inline void deserialize(SingleSvmClassifier &item, std::istream &in) {
        try {
            dlib::deserialize(item.negativeLabel, in);
            dlib::deserialize(item.positiveLabel, in);
            dlib::deserialize(item.classifier, in);
        } catch (dlib::serialization_error &e) {
            throw dlib::serialization_error(e.info + "\n while deserializing an object of type MyClassifier");
        }
    }

    class DlibSvmClassifier : public DescriptorsClassifier {
    public:
        FACES_OVERRIDE_ATTRIBUTE(threshold, 0.3)

        FACES_MAIN_CONSTRUCTOR(explicit DlibSvmClassifier, std::string const &classifiersFile);

        void train(std::map<int, std::vector<double>> const &samples) override;

    protected:
        int _classifyDescriptors(const std::vector<double> &descriptors) override;

        bool _save(std::string const &dst) override;

        bool _load(std::string const &classifiersFile);

    private:
        std::vector<SingleSvmClassifier> _classifiers;

    };

    FACES_REGISTER_SUBCLASS(DescriptorsClassifier, DlibSvmClassifier, DlibSvm)

}

#endif //FACES_DLIBSVMCLASSIFIER_H
