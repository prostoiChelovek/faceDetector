/**
 * @file Manager.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 30 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_MANAGER_HPP
#define FACES_MANAGER_HPP

#include <Face/Face.h>
#include <Tracker/Tracker.hpp>

namespace faces {

    class NewFaceHandlerBase {
    public:
        virtual void handle(Face &face) = 0;

    };

    class Manager {
    public:
        inline static bool autoUpdatePrevious = true;

        Manager() = default;

        void update(std::vector<Face> const &detected, Tracker::TrackedT const &tracked) {
            _faces = detected;
            update(tracked);
        }

        void update(Tracker::TrackedT const &tracked) {
            for (auto const &[prevIdx, actualIdx] : tracked) {
                if (prevIdx == -1) { // new face
                    _processNewFace(_faces.at(actualIdx));
                } else if (actualIdx == -1) { // face lost
                    _processLostFace(_previousFaces.at(prevIdx));
                } else { // face tracked
                    _processTrackedFace(_previousFaces.at(prevIdx), _faces.at(actualIdx));
                }
            }

            if (tracked.empty() && !_faces.empty()) {
                for (Face &face : _faces) {
                    _processNewFace(face);
                }
            }

            if (autoUpdatePrevious) {
                updatePrevious();
            }
        }

        void updatePrevious() {
            _previousFaces = _faces;
        }

        std::vector<Face> &getFaces() {
            return _faces;
        }

        std::vector<Face> const &getPreviousFaces() {
            return _previousFaces;
        }

    protected:
        std::vector<Face> _faces, _previousFaces;

        virtual void _processNewFace(Face &face) = 0;

        virtual void _processLostFace(Face &face) = 0;

        virtual void _processTrackedFace(Face &prevFace, Face &actualFace) = 0;

    };

}

#endif //FACES_MANAGER_HPP
