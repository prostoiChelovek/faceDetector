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

    class Manager {
    public:
        Manager() = default;

        virtual void update(std::vector<Face> const &detected, Tracker::TrackedT const &tracked) = 0;

        void update(Tracker::TrackedT const &tracked) {
            update(_faces, tracked);
        }

        std::vector<Face> &getFaces() {
            return _faces;
        }

        std::vector<Face> const &getPreviousFaces() {
            return _previousFaces;
        }

    protected:
        std::vector<Face> _faces, _previousFaces;

    };

}

#endif //FACES_MANAGER_HPP
