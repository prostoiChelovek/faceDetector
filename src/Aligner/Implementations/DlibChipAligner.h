/**
 * @file DlibChipAligner.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 15 Aug 2020
 * @copyright MIT License
 *
 * @brief This file contains a face aligner based on dlibs chip_details
 */

#ifndef FACES_DLIBCHIPALIGNER_H
#define FACES_DLIBCHIPALIGNER_H

#include <dlib/opencv/cv_image.h>
#include <dlib/image_transforms/interpolation.h>

#include <utils/utils.h>

#include <Aligner/Aligner.hpp>

namespace faces {

    /**
     * A face aligner based on dlibs chip_details, which uses landmarks to align faces
     */
    class DlibChipAligner : public Aligner {
    public:
        FACES_MAIN_CONSTRUCTOR(explicit DlibChipAligner, Config const &config);

    protected:
        cv::Mat _align(Face const &face, cv::Mat const &wholeImg) override;

    };

    FACES_REGISTER_SUBCLASS(Aligner, DlibChipAligner, DlibChip)

}

#endif //FACES_DLIBCHIPALIGNER_H
