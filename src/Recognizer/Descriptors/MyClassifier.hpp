//
// Created by prostoichelovek on 26.07.19.
//

#ifndef FACES_MYCLASSIFIER_HPP
#define FACES_MYCLASSIFIER_HPP


#include <dlib/svm.h>

namespace Faces {

    typedef dlib::matrix<double, 128, 1> descriptor_type;
    typedef dlib::histogram_intersection_kernel<descriptor_type> kernel_type;
    typedef dlib::svm_c_trainer<kernel_type> trainer_type;

    class MyClassifier {
    public:
        int negativeLabel;
        int positiveLabel;

        dlib::decision_function<kernel_type> classifier;

        MyClassifier() = default;

        MyClassifier(int negativeLabel, int positiveLabel,
                     dlib::decision_function<kernel_type> classifier)
                : negativeLabel(negativeLabel), positiveLabel(positiveLabel),
                  classifier(classifier) {
        }

        double operator()(descriptor_type &descriptor, double threshold = 0.3) {
            double prediction = classifier(descriptor);
            if (fabs(prediction) < threshold)
                return -1;
            if (prediction < 0)
                return negativeLabel;
            return positiveLabel;
        }

    };

    inline void serialize(const MyClassifier &item, std::ostream &out) {
        try {
            dlib::serialize(item.negativeLabel, out);
            dlib::serialize(item.positiveLabel, out);
            dlib::serialize(item.classifier, out);
        } catch (dlib::serialization_error &e) {
            throw dlib::serialization_error(e.info + "\n while serializing an object of type MyClassifier");
        }
    }

    inline void deserialize(MyClassifier &item, std::istream &in) {
        try {
            dlib::deserialize(item.negativeLabel, in);
            dlib::deserialize(item.positiveLabel, in);
            dlib::deserialize(item.classifier, in);
        } catch (dlib::serialization_error &e) {
            throw dlib::serialization_error(e.info + "\n while deserializing an object of type MyClassifier");
        }
    }

}


#endif //FACES_MYCLASSIFIER_HPP
