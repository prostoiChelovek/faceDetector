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

    class NewFaceHandlerBase {
    public:
        virtual void handle(Face &face) = 0;

    };

    template<typename DatabaseT>
    class NewFaceDatabaseRegistrar : public NewFaceHandlerBase {
        static_assert(is_base_template_of<Database, std::remove_pointer_t<DatabaseT>>::value,
                      "DatabaseT should be an implementation of the Database");
        static_assert(std::is_convertible_v<Face const &, typename DatabaseT::EntryType>,
                      "DatabaseT's EntryT should be initialisable with Face");

    public:
        inline static bool saveDatabase = true;

        explicit NewFaceDatabaseRegistrar(DatabaseT &database) : _database(database) {}

        void handle(Face &face) override {
            auto entry = static_cast<typename DatabaseT::EntryType>(face);
            typename DatabaseT::IdentifierType newId = _database.add(entry);
            if (saveDatabase) {
                _database.save();
            }

            face.label = static_cast<int>(newId);
        }

    private:
        DatabaseT _database;

    };

    class DefaultManager : public Manager {
    public:
        inline static int handleNewFaceAfter = 15;

        NewFaceHandlerBase *newFaceHandler;

        explicit DefaultManager(Config const &config, NewFaceHandlerBase *newFaceHandler = nullptr)
                : newFaceHandler(newFaceHandler),
                  _random(std::random_device()()), _randomDistribution(INT_MIN, INT_MIN / 2) {}

    protected:
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
            // execute a handler for the face and stop counting frames for it
            // if label is unknown, label wasn't determined for handleNewFaceAfter frames and handler was set
            if (actualFace.label < 0 && faceFramesCount > handleNewFaceAfter && newFaceHandler) {
                _framesCount.erase(actualFace.label);
                newFaceHandler->handle(actualFace);
            } else {
                faceFramesCount++;
            }
        }

    private:
        std::map<int, int> _framesCount;

        std::mt19937 _random;
        std::uniform_int_distribution<int> _randomDistribution;

        [[nodiscard]] int _getTempLabel();

    };

    int DefaultManager::_getTempLabel() {
        return _randomDistribution(_random);
    }

}

#endif //FACES_DEFAULTMANAGER_HPP
