/**
 * @file DefaultManager.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 30 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DEFAULTMANAGER_HPP
#define FACES_DEFAULTMANAGER_HPP

#include <random>

#include <utils/utils.h>
#include <Manager/Manager.hpp>
#include <Database/Database.hpp>
#include <Config/Config.h>

namespace faces {

    template<typename DatabaseT>
    class DefaultManager : public Manager {
        static_assert(is_base_template_of<Database, DatabaseT>::value,
                      "DatabaseT should be an implementation of the Database");
        static_assert(std::is_convertible_v<Face const &, typename DatabaseT::EntryType>,
                      "DatabaseT's EntryT should be initialisable with Face");

    public:
        inline static int registerFaceAfter = 15;

        inline static bool saveDatabase = true;

        explicit DefaultManager(Config const &config, DatabaseT *database)
                : _database(database),
                  _random(std::random_device()()), _randomDistribution(INT_MIN, INT_MIN / 2) {}

    protected:
        DatabaseT *_database;

        void _processNewFace(Face &face) override {
            if (face.label == -1) {
                face.label = _getTempLabel();
            }

            _framesCount[face.label] = 0;
        }

        void _processLostFace(Face &face) override {
            _framesCount[face.label] = 0;
        }

        void _processTrackedFace(Face &prevFace, Face &actualFace) override {
            if (actualFace.label >= 0 && prevFace.label != actualFace.label) { // label was determined
                _framesCount.erase(prevFace.label);
            } else {
                actualFace.label = prevFace.label;
            }

            int &faceFramesCount = _framesCount[actualFace.label];
            if (actualFace.label < 0 && faceFramesCount > registerFaceAfter) {
                _framesCount.erase(actualFace.label);

                actualFace.label = _addToDb(actualFace);
            } else {
                faceFramesCount++;
            }
        }

    private:
        std::map<int, int> _framesCount;

        std::mt19937 _random;
        std::uniform_int_distribution<int> _randomDistribution;

        [[nodiscard]] int _getTempLabel();

        int _addToDb(Face const &face);

    };

    template<typename DatabaseT>
    int DefaultManager<DatabaseT>::_getTempLabel() {
        return _randomDistribution(_random);
    }

    template<typename DatabaseT>
    int DefaultManager<DatabaseT>::_addToDb(Face const &face) {
        auto entry = static_cast<typename DatabaseT::EntryType>(face);
        typename DatabaseT::IdentifierType newId = _database->add(entry);
        if (saveDatabase) {
            _database->save();
        }

        return static_cast<int>(newId);
    }

}

#endif //FACES_DEFAULTMANAGER_HPP
