/**
 * @file DefaultManager.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 13 Sep 2020
 * @copyright MIT License
 *
 * @brief
 */

#include "DefaultManager.h"

namespace faces {

    DefaultManager::DefaultManager(const Config &config, NewFaceHandlerBase *newFaceHandler)
            : newFaceHandler(newFaceHandler),
              _random(std::random_device()()), _randomDistribution(INT_MIN, INT_MIN / 2) {}

    void DefaultManager::_processNewFace(Face &face) {
        if (face.label == -1) {
            face.label = _getTempLabel();
        }

        _framesCount[face.label] = 0;
    }

    void DefaultManager::_processLostFace(Face &face) {
        _framesCount[face.label] = 0;
    }

    void DefaultManager::_processTrackedFace(Face &prevFace, Face &actualFace) {
        if (actualFace.label >= 0 && prevFace.label != actualFace.label) { // label was determined
            _framesCount.erase(prevFace.label);
        } else {
            actualFace.label = prevFace.label;
        }

        int &faceFramesCount = _framesCount[actualFace.label];
        // execute a handler for the face and stop counting frames for it
        // if label is unknown, label wasn't determined for handleNewFaceAfter frames and handler was set
        if (actualFace.label < 0 && faceFramesCount > handleNewFaceAfter && newFaceHandler) {
            _framesCount.erase(actualFace.label);
            newFaceHandler->handle(actualFace);
        } else {
            faceFramesCount++;
        }
    }

    int DefaultManager::_getTempLabel() {
        return _randomDistribution(_random);
    }


}