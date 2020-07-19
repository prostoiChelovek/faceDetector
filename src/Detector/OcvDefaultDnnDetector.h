/**
 * @file OcvDefaultDnnDetecor.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 19 Jul 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_OCVDEFAULTDNNDETECTOR_H
#define FACES_OCVDEFAULTDNNDETECTOR_H

#include "OcvDnnDetector.h"


namespace faces {

    class OcvDefaultDnnDetector : public OcvDnnDetector {
    protected:
        FACES_OVERRIDE_ATTRIBUTE(inSize, 300, 300);
        FACES_OVERRIDE_ATTRIBUTE(inScaleFactor, 1.0)

        FACES_OVERRIDE_ATTRIBUTE(meanVal, 104.0, 177.0, 123.0)

    public:
        FACES_OVERRIDE_ATTRIBUTE(confidenceThreshold, 0.7)

    };

    FACES_REGISTER_SUBCLASS(IDetector, OcvDefaultDnnDetector, OcvDefaultDnn)

}

#endif //FACES_OCVDEFAULTDNNDETECTOR_H
