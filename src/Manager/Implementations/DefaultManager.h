/**
 * @file DefaultManager.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 30 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DEFAULTMANAGER_H
#define FACES_DEFAULTMANAGER_H

#include <Manager/Manager.hpp>
#include <Database/Database.hpp>
#include <Config/Config.h>

namespace faces {

    template<typename DatabaseT>
    class DefaultManager : public Manager {
    public:
        explicit DefaultManager(Config const &config, DatabaseT *database)
                : _database(database) {

        }

        void update(std::vector<Face> const &detected, Tracker::TrackedT const &tracked) override {
            _faces = detected;

            for (auto const &[prevIdx, actualIdx] : tracked) {
                if (prevIdx == -1) { // new face

                } else if (actualIdx == -1) { // face lost

                }
            }

            _previousFaces = _faces;
        }

    protected:
        DatabaseT *_database;

    };

}

#endif //FACES_DEFAULTMANAGER_H
