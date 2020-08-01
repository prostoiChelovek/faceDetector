/**
 * @file DlibResnetDescriptor.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 01 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DLIBRESNETDESCRIPTOR_H
#define FACES_DLIBRESNETDESCRIPTOR_H

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/opencv.h>

#include <spdlog/spdlog.h>

#include "utils/utils.h"
#include <Recognizer/Descriptors/Descriptor.hpp>

namespace faces {

    // The next bit of code defines a ResNet network.  It's basically copied
    // and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
    // layer with loss_metric and made the network somewhat smaller.  Go read the introductory
    // dlib DNN examples to learn what all this stuff means.
    //
    // Also, the dnn_metric_learning_on_images_ex.cpp example shows how to train this network.
    // The dlib_face_recognition_resnet_model_v1 model used by this example was trained using
    // essentially the code shown in dnn_metric_learning_on_images_ex.cpp except the
    // mini-batches were made larger (35x15 instead of 5x5), the iterations without progress
    // was set to 10000, and the training dataset consisted of about 3 million images instead of
    // 55.  Also, the input layer was locked to images of size 150.
    template<template<int, template<typename> class, int, typename> class block, int N,
            template<typename> class BN, typename SUBNET>
    using residual = dlib::add_prev1<block<N, BN, 1, dlib::tag1<SUBNET>>>;

    template<template<int, template<typename> class, int, typename> class block, int N,
            template<typename> class BN, typename SUBNET>
    using residual_down = dlib::add_prev2<dlib::avg_pool<2, 2, 2, 2,
            dlib::skip1<dlib::tag2<block<N, BN, 2, dlib::tag1<SUBNET>>>>>>;

    template<int N, template<typename> class BN, int stride, typename SUBNET>
    using block  = BN<dlib::con<N, 3, 3, 1, 1, dlib::relu<BN<dlib::con<N, 3, 3, stride, stride, SUBNET>>>>>;

    template<int N, typename SUBNET> using ares      = dlib::relu<residual<block, N, dlib::affine, SUBNET>>;
    template<int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block, N, dlib::affine, SUBNET>>;

    template<typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
    template<typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
    template<typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
    template<typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
    template<typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

    using anet_type = dlib::loss_metric<dlib::fc_no_bias<128, dlib::avg_pool_everything<
            alevel0<
                    alevel1<
                            alevel2<
                                    alevel3<
                                            alevel4<
                                                    dlib::max_pool<3, 3, 2, 2,
                                                            dlib::relu<dlib::affine<dlib::con<
                                                                    32, 7, 7, 2, 2,
                                                                    dlib::input_rgb_image_sized<150>
                                                            >>>>>>>>>>>>;

    typedef dlib::matrix<double, 128, 1> DescriptorType;


    class DlibResnetDescriptor : public Descriptor {
    public:
        FACES_MAIN_CONSTRUCTOR(explicit DlibResnetDescriptor, std::string const &model);

    protected:
        FACES_OVERRIDE_ATTRIBUTE(faceSize, 150, 150)

        std::vector<double> _computeDescriptors(cv::Mat const &faceImg) override;

        bool _load(std::string const &src);

    private:
        anet_type _descriptor;

    };

    FACES_REGISTER_SUBCLASS(Descriptor, DlibResnetDescriptor, DlibResnet)

}

#endif //FACES_DLIBRESNETDESCRIPTOR_H
